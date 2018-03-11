//
// Created by wansong on 11/03/2018.
//

#include <cacheproxy/http/header.h>
#include <cacheproxy/utils/string.h>
#include <regex>


namespace smartstudy {

void parse_vary(const std::string &vary, std::vector<std::string> &ret) {
  std::stringstream ss(vary);
  std::string item;

  static const char delim = ',';

  while (std::getline(ss, item, delim)) {
    if (item == "") continue;

    std::cout << "a line: " << item << "|" << std::endl;
    boost::trim(item);
    snakecase(item);
    ret.emplace_back(std::move(item));
  }

  std::cout << "done" << std::endl;
}


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

}

