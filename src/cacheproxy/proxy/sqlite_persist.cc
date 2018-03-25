//
// Created by wansong on 16/03/2018.
//

#include "sqlite_persist.h"
#include <cacheproxy/proxy/fs_error_category.h>
#include <cacheproxy/config.h>
#include <cacheproxy/db/metadb.h>
#include <cassert>
#include <iostream>
#include <cacheproxy/utils/string.h>
#include <string>
#include <cacheproxy/utils/sqlite_utils.h>
#include <cacheproxy/http/header.h>
#include <third-party/sqlite3cpp/include/SQLiteCpp/Transaction.h>


namespace smartstudy {
// fixme: beast需要手动解码除transfer-encoding之外的其他encoding，以后做，所以现在只能支持transfer-encoding = identify
// fixme: 检查把field value内部的逗号当成field value之间的分隔符的可能性，目前仅对user-agent做了特殊处理

typedef std::pair<std::string, std::string> map_entry;

std::string escape_for_sqlite(const std::string &str) {
  static std::set<char> CHARACTERS_TO_ESCAPE_FOR_SQLITE{ '%', '_' };
  return escape_using(str, '\\', CHARACTERS_TO_ESCAPE_FOR_SQLITE);
}


static std::map<std::string, std::map<std::string, std::string>> HEADER_FIELDS = {
  { "version", {} },
  { "method", {} },
  { "host", {} },
  { "path", {} },
  { "vary", { { "type", "INTEGER" }, { "constraint", "UNIQUE" } } },
  { "age", {} },
  { "allow", {} },

  { "access-control-allow-credentials", {} },
  { "access-control-allow-methods", {} },
  { "access-control-allow-origin", {} },
  { "access-control-expose-headers", {} },
  { "access-control-max-age", {} },


  { "forwarded", {} },
  { "x-forwarded-host", {} },

  { "cache-control", {}},
  { "connection", {}},
  { "keep-alive", {}},
  // begin: supported content negotiation
  { "content-type", {}},
  { "content-language", {}},
  { "content-encoding", {} },
  { "cookie", {} },
  { "content-range0", {} },
  { "content-range1", {} },
  { "content-range-total", {} },
  // end: supported content negotiation
  { "content-disposition", {} },
  { "content-length", {} },
  { "content-location", {} },
  { "content-security-policy", {} },
  { "content-security-policy-report-only", {} },
  { "sourcemap", {} },
  { "strict-transport-security", {} },
  { "set-cookie", {} },
  { "last-modified", {} },
  { "public-key-pins", {} },
  { "public-key-pins-report-only", {} },
  { "timing-allow-origin", {} },
  { "x-content-type-options", {} },

  { "tk", {} },
  { "x-frame-options", {} },
  { "x-dns-prefetch-control", {} },
  { "trailer", {} },
  { "proxy-authenticate", {} },
  { "user-agent", {} },
  { "referer", {} },
  { "date", {} },
  { "etag", {} },
  { "from", {} },
  { "location", {} },
  { "origin", {} },
  { "pragma", {} },
  { "server", {} },
  { "via", {} },
  { "warning", {} },
};

static table_descriptor vary_schema = {
  "vary",
  {
    { "id", "INTEGER PRIMARY KEY" },
    { "host", "TEXT" },
    { "path", "TEXT" },
    { "value", "TEXT" },
  },
  true,
  "CONSTRAINT resource_unique UNIQUE (host, path)"
};

std::string to_response_counterpart(const std::string &request_header) {
  static std::map<std::string, std::string> ACCEPT_CONTENT_PAIRS = {
    { "accept", "content-type" },
    { "range", "content-range" },
    { "accept-encoding", "content-encoding" },
    { "accept-language", "content-language" },
  };

  auto it = std::find_if(ACCEPT_CONTENT_PAIRS.cbegin(), ACCEPT_CONTENT_PAIRS.cend(),
  [&request_header](const std::pair<std::string, std::string> &pair) { return request_header == pair.first; });

  if (it != ACCEPT_CONTENT_PAIRS.cend()) {
    return it ->second;
  }

  return request_header;
}


sqlite_persist::sqlite_persist(const char *path):
  db(new SQLite::Database(path, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)) {}


void sqlite_persist::initialize() {
  create_record_table();
}


void sqlite_persist::create_vary_table() {
  std::string sql = build_sql(vary_schema);
  this ->db ->exec(sql);
}


void sqlite_persist::create_record_table() {
  std::vector<field_descriptor> fields{ { "id", "INTEGER PRIMARY KEY" } };
  for (const auto& kv: HEADER_FIELDS) {
    const std::string &key = kv.first;
    const std::map<std::string, std::string> &value = kv.second;
    if (value.empty()) {
      fields.push_back({ key, "TEXT" });
    } else {
      const auto& type = kv.second.find("type") ->second;
      const auto& p_constraint = kv.second.find("constraint");
      fields.push_back({
                         key,
                         type,
                         p_constraint == kv.second.end() ? "" : p_constraint ->second
                       });
    }
  }
  table_descriptor record = { SS_CACHEPROXY_RESPONSE_TABLE, std::move(fields) };
  auto sql = build_sql(record);
  db ->exec(sql);
}


std::string sqlite_persist::query_vary(const cache_request_t &req) {
  std::string sql = "SELECT value FROM vary WHERE ";
  sql += "host = '" + req.host + "' AND ";
  sql += "path = '" + req.path + "';";
  return db ->execAndGet(sql);
}

template <typename Callback>
void traverse_vary_fields(
  const field_value& vary_parsed,
  const cache_request_t& req,
  Callback cb) {

  for (auto& a_vary: vary_parsed) {
    // e.g. accept
    const std::string &variable_header = std::find_if(
      a_vary.cbegin(),
      a_vary.cend(),
      [](const map_entry&pair){ return pair.first == "value"; }) ->second;
    assert(!variable_header.empty());

    const auto p_vary_value = std::find_if(
      req.headers.cbegin(),
      req.headers.cend(),
      [&variable_header](const std::pair<std::string, std::string>& pair) { return pair.first == variable_header; }
    );
    if (p_vary_value == req.headers.cend()) {
      continue;
    }

    bool done = cb(variable_header, p_vary_value ->second);
    if (done) {
      return;
    }
  }
}

void add_where_clause(
  const field_value& vary_parsed,
  const cache_request_t& req,
  std::string &ret) {
  ret.append("WHERE ");
  ret.append("host = '");
  ret.append(req.host + "' AND ");
  ret.append("path = '");
  ret.append(req.path + "' AND ");
  ret.append("method = '");
  ret.append(req.method + "' AND ");


  traverse_vary_fields(vary_parsed, req, [&ret, &req](const std::string& vary_field, const std::string &_) {
    if (is_same_header(vary_field, "range")) {
      return false;
    }
    const std::string vary_field_resp = to_response_counterpart(vary_field);
    ret += to_sqlite_identifier(vary_field_resp) + " LIKE ? ESCAPE '\\' AND ";
    return false;
  });

  const auto p_vary_value = std::find_if(
    req.headers.cbegin(),
    req.headers.cend(),
    [](const std::pair<std::string, std::string>& pair) { return pair.first == "range"; }
  );
  char buf[100];
  // 如果request不指定range，就不对range筛选
  if (p_vary_value != req.headers.cend()) {
    unsigned long first, last;
    parse_range(p_vary_value ->second, first, last);
    sprintf(buf, "\"content-range1\" >= %lu AND \"content-range0\" <= %lu AND ", first, last);
    ret += buf;
  }

  if (ret.substr(ret.size() - 5) == " AND ") {
    ret.replace(ret.size() - 5, 5, ";");
  }
}



template <typename Callback>
bool permute_bind(
  const cache_request_t& req,
  SQLite::Statement& stmt,
  const std::vector<std::pair<std::string, field_value>> &vary_kvs,
  std::vector<string_map>& field_value_piece,
  int whichField,
  Callback cb) {

  if (whichField == vary_kvs.size()) {
    int j = 0;
    for(auto i = 0; i < field_value_piece.size(); i++) {
      std::string& value = field_value_piece[i]["value"];
      assert(!value.empty());
      // 对于range，数值不需要bing，已经包含在sql
      if (vary_kvs[i].first != "range") {
        stmt.bind(++j, "%" + escape_for_sqlite(value) + "%");
      }
    }

    bool done = cb();
    stmt.clearBindings();
    return done;
  }

  const field_value& fv = vary_kvs[whichField].second;
  for (auto i = 0; i < fv.size(); i++) {
    if (field_value_piece.size() <= whichField) {
      field_value_piece.push_back(fv[i]);
    } else {
      field_value_piece[i] = fv[i];
    }

    bool done = permute_bind(req, stmt, vary_kvs, field_value_piece, whichField + 1, cb);
    if (done) {
      return true;
    }
  }
  return false;
}


std::vector<string_map> extract_row(SQLite::Statement &stmt) {
  std::vector<string_map> ret;
  while(stmt.executeStep()) {
    string_map it;
    auto cnt = stmt.getColumnCount();
    for (auto i = 0; i < cnt; i++) {
      const std::string value = stmt.getColumn(i);
      std::cout << "value : " << value << ", for column: " << stmt.getColumn(i) << std::endl;
      it[stmt.getColumnName(i)] = value;
    }
    ret.emplace_back(std::move(it));
  }
  return ret;
}


void filter_result_set(const cache_request_t& req,
                       const field_value& varies,
                       std::vector<string_map>& row) {
  for (auto it = row.begin(); it != row.end();) {
    bool rejected = false;
    for (const string_map& f: varies) {
      const std::string header = f.find("value") ->second;
      const std::string header_resp = to_response_counterpart(header);

      auto p_request_value = req.headers.find(header);
      if (p_request_value == req.headers.cend()) {
        continue;
      }
      if (p_request_value ->second == "*" || p_request_value ->second.empty()) {
        continue;
      }

      field_value fv;
      parse_http_field(p_request_value ->second, fv);

      auto p_response_value = it ->find(header_resp);

      if (p_response_value == it ->cend() || p_response_value ->second.empty()) {
        rejected = true;
        break;
      }

      field_value fv_resp;
      parse_http_field(p_response_value ->second, fv_resp);

      if (std::all_of(fv.cbegin(), fv.cend(), [&fv_resp](const string_map& option) {
        const string_map& real_field = *fv_resp.cbegin();
        return std::any_of(option.cbegin(), option.cend(), [&real_field](const map_entry& pair) {
          if (pair.first == "q") {
            return false;
          }

          const auto find = real_field.find(pair.first);
          if (find == real_field.cend() && pair.second != "*" && !pair.second.empty()) {
            return true;
          }

          return find ->second != pair.second;
        });
      })) {
        rejected = true;
        break;
      }
    }

    if (rejected) {
      it = row.erase(it);
    } else {
      ++it;
    }
  }
}


void sqlite_persist::get_cache_response(
  const cache_request_t &req, std::vector<cache_response_t> &resp) {
  resp.clear();

  // 查表得到vary值，*代表不支持缓存
  std::string vary = query_vary(req);
  if (vary == "*") {
    return;
  }

  field_value vary_parsed;
  // todo: sort against q
  parse_http_field(vary, vary_parsed);

  // 拼接sql
  std::string sql = std::string("SELECT * FROM ") + SS_CACHEPROXY_RESPONSE_TABLE + " ";
  if (!vary.empty()) {
    sql.append("LEFT JOIN vary ");
    sql.append("ON ");
    sql.append(SS_CACHEPROXY_RESPONSE_TABLE);
    sql.append(".vary = vary.id ");
  }

  add_where_clause(vary_parsed, req, sql);

  SQLite::Statement stmt(*db, sql);
  if (vary.empty()) {
    std::vector<string_map> rows = extract_row(stmt);
    filter_result_set(req, vary_parsed, rows);
    resp = rows;
    return;
  }


  // 获取vary列表每一项及其在request里得值的pair
  std::vector<std::pair<std::string, field_value>> vary_field_value_pairs;
  traverse_vary_fields(vary_parsed, req, [&vary_field_value_pairs](const std::string& field, const std::string& value) {
    assert(!value.empty());
    field_value fv;
    parse_http_field(value, fv);
    vary_field_value_pairs.emplace_back(std::string(field), std::move(fv));
    return false;
  });


  // bind参数：只考虑vary每一项对应的header值，如果有参数的话还需要利用filter_result_set进一步筛选
  try {
    std::vector<string_map> field_value_pieces;
    permute_bind(req, stmt, vary_field_value_pairs, field_value_pieces, 0, [&stmt, &req, &resp, &vary_parsed]() {
      std::vector<string_map> rows = extract_row(stmt);
      filter_result_set(req, vary_parsed, rows);
      if (!rows.empty()) {
        resp = std::move(rows);
        return true;
      }
      return false;
    });
  } catch (std::exception& e) {
    // todo: maintain an in-memory cache of current schema
    std::cout << "failed to get_cache_response, error: " << e.what() << std::endl;
  }
}


void sqlite_persist::set_cache_response(
  const cache_request_t &req, const cache_response_t &resp) {
  SQLite::Transaction transaction(*this ->db);
  bool has_vary = resp.find("vary") != resp.end();
  unsigned vary_id = 0;
  if (has_vary) {
    vary_id = save_vary(req, resp);
  }
  auto p_host = resp.find("host");
  record_values rvs = {
    { "method", req.method },
    { "host", p_host == resp.end() ? req.host : p_host ->second },
    { "path", resp.find("path") ->second },
  };

  auto p_version = resp.find("version");
  if (p_version != resp.end()) {
    rvs.emplace_back("version", p_version ->second);
  }

  if (has_vary) {
    if (vary_id == 0) {
      return;
    }
    rvs.emplace_back("vary", req.headers.find("vary") ->second);
  }

  auto p_range = resp.find("content-range");
  if (p_range != resp.end()) {
    unsigned long beg, nd, total;
    parse_content_range(p_range ->second, beg, nd, total);
    char buf[100];
    sprintf(buf, "%lu", beg);
    rvs.emplace_back("content-range0", buf);
    sprintf(buf, "%lu", nd);
    rvs.emplace_back("content-range1", buf);
    sprintf(buf, "%lu", total);
    rvs.emplace_back("content-range-total", buf);
  }

  for (const auto& pair: resp) {
    const std::string& h = pair.first;
    const std::string& v = pair.second;
    if (h == "vary" || h == "content-range") {
      continue;
    }

    rvs.emplace_back(h, v);
  }

  insert_update_descriptor insert = { SS_CACHEPROXY_RESPONSE_TABLE, std::move(rvs) };
  std::string sql = build_sql(insert);
  auto n_rows = this ->db ->exec(sql);
  if (n_rows <= 0) {
    std::cout << "failed to insert resp" << std::endl;
    return;
  }
  transaction.commit();
}


long long sqlite_persist::save_vary(
  const cache_request_t &req, const cache_response_t &resp) {
  auto vary = resp.find("vary") ->second;

  record_values rvs{ { "value", vary } };
  std::vector<std::string> fields { "host", "path" };
  for (const auto& f: fields) {
    const auto& v = resp.find(f) ->second;
    rvs.emplace_back(f, v);
  }

  insert_update_descriptor ins{ "vary", std::move(rvs) };
  auto sql = build_sql(ins);
  this ->db ->exec(sql);

  auto ret = this ->db ->getLastInsertRowid();
  if (ret > 0) {
    return ret;
  }

  std::string get_vary_sql = "SELECT id from vary where host = '";
  get_vary_sql.append(resp.find("host") ->second);
  get_vary_sql.append("' AND path = '");
  get_vary_sql.append(resp.find("path") ->second);
  get_vary_sql.append("';");
  return this ->db ->execAndGet(get_vary_sql);
}

}