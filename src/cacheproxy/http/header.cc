//
// Created by wansong on 11/03/2018.
//

#include <cacheproxy/http/header.h>
#include <cacheproxy/utils/string.h>
#include <regex>


namespace smartstudy {

// @private
size_t quote_count(const std::string &str) {
  size_t ret = 0;
  for (auto& c: str) {
    if (c == '"') {
      ret += 1;
    }
  }
  return ret;
}


// @private
void split_values(const std::string &value, const char &delim, string_vector &parts) {
  split(value, delim, parts);

  auto j = parts.begin();
  auto nd = parts.end();
  auto i = j != nd ? j + 1 : j;

  while (j != nd && i != nd) {
    if (quote_count(*j) % 2 == 0) {
      *++j = *i;
    } else {
      *j += delim + *i;
    }
    ++i;
  }

  if (j != nd && j + 1 != nd) {
    parts.erase(j + 1, nd);
  }

  for (auto& str: parts) {
    boost::trim(str);
  }
}


void trim_field_kv(std::string &str) {
  boost::trim(str);

  if (str.empty()) {
    return;
  }

  if (str[0] == '"') {
    str.erase(0, 1);
  }

  if (str.empty()) {
    return;
  }

  if (str[str.size() - 1] == '"') {
    str.erase(str.size() - 1);
  }
}


void split_key_value(const std::string &str, string_vector &ret) {
  split(str, '=', ret);
  if (ret.size() != 2) {
    std::cerr << "invalid field param(key value pair): " << str << std::endl;
    assert(false);
  }

  trim_field_kv(ret[0]);
  trim_field_kv(ret[1]);
}


void parse_single_field_params(const std::string &str, string_map &ret) {
  string_vector parts;
  split_values(str, ';', parts);

  for (const auto& part: parts) {
    string_vector kv;
    split_key_value(part, kv);
    if (!kv[0].empty() && !kv[1].empty()) {
      ret.insert({ kv[0], kv[1] });
    }
  }
}


void parse_single_field_value(const std::string &part, string_map &ret) {
  const std::regex &re = get_http_field_re();
  std::smatch sm;
  try {
    if (!std::regex_match(part, sm, re) || sm.size() < 2) {
      return;
    }

    std::string value = sm[1];
    boost::trim(value);
    ret.insert({ SS_HTTP_HEADER_FIELD_VALUE, std::move(value) });
    if (sm.size() == 3) {
      parse_single_field_params(sm[2], ret);
    }

  } catch (const std::exception e) {
    std::cerr << "failed to parse_single_field_value, error: " << e.what() << std::endl;
  }
}

// input: text/plain ; q = 0.9 ; charset = 'utf16'
// output: { {'value', 'text/plain'}, {'q', '0.9'}, {'charset', 'utf16'}}
void parse_http_field(const std::string &value, field_value &ret) {
  string_vector parts;
  split_values(value, ',', parts);

  for (const auto &part: parts) {
    string_map params;
    parse_single_field_value(part, params);
    if (!params.empty()) {
      ret.emplace_back(std::move(params));
    }
  }
}



bool is_same_header(const std::string &a, const std::string &b) {
  if (a.size() != b.size()) {
    return false;
  }

  for (auto i = 0; i < a.size(); i++) {
    if (a[i] == '-' || a[i] == '_') {
      if (b[i] != '-' && b[i] != '_') {
        return false;
      }
      continue;
    }

    if (tolower(a[i]) != tolower(b[i])) {
      return false;
    }
  }

  return true;
}

struct range {
  unsigned long beg;
  unsigned long nd;
  bool has_beg;
  bool has_nd;
};

void parse_range_imp(range &rg, const std::string &str, std::string &buf, std::string::const_iterator &idx) {
  const char& c = *idx;
  if (!rg.has_beg) {
    if (c == '-') {
      rg.beg = buf.empty() ? RESERVED_MAX_UNSIGNED_LONG : (unsigned long)atoi(buf.c_str());
      rg.has_beg = true;
      buf.clear();
    } else if (isnumber(c)) {
      buf += c;
    }
  } else if (!rg.has_nd) {
    if (isnumber(c)) {
      buf += c;
    }
  }

  idx += 1;
}

void parse_range(range &rg, const std::string &str) {
  std::string buf;
  auto idx = str.cbegin();
  while (idx != str.cend()) {
    parse_range_imp(rg, str, buf, idx);
  }

  if (!rg.has_beg) {
    rg.beg = buf.empty() ? RESERVED_MAX_UNSIGNED_LONG : (unsigned long)atoi(buf.c_str());
    rg.nd = RESERVED_MAX_UNSIGNED_LONG;
  } else {
    rg.nd = buf.empty() ? RESERVED_MAX_UNSIGNED_LONG : (unsigned long)atoi(buf.c_str());
  }
}

void parse_range(const std::string& str, std::vector<std::pair<unsigned long, unsigned long>> &ret) {
  field_value parsed;
  parse_http_field(str, parsed);
  for (const auto& piece: parsed) {
    std::vector<std::string> parts;
    range rg{ RESERVED_MAX_UNSIGNED_LONG, RESERVED_MAX_UNSIGNED_LONG, false, false };
    parse_range(rg, piece.find("value") ->second);
    ret.emplace_back(rg.beg, rg.nd);
  }
}


void parse_content_range(const std::string& str, unsigned long& first, unsigned long& last, unsigned long& total) {
  std::vector<std::string> parts;
  split(str, '/', parts);
  if (parts.size() != 2) {
    assert(false);
    return;
  }

  range rg{ RESERVED_MAX_UNSIGNED_LONG, RESERVED_MAX_UNSIGNED_LONG, false, false };
  parse_range(rg, parts[0]);
  first = rg.beg;
  last = rg.nd;
  boost::trim(parts[1]);
  if (parts[1] == "*") {
    total = RESERVED_MAX_UNSIGNED_LONG;
  } else {
    total = (unsigned long)atoi(parts[1].c_str());
  }
}


}

