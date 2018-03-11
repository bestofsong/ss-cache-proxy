//
// Created by wansong on 11/03/2018.
//

#ifndef CACHE_PROXY_HEADER_H
#define CACHE_PROXY_HEADER_H

#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <cacheproxy/http/regex.h>
#include <cacheproxy/http/constants.h>

namespace smartstudy {

typedef std::map<std::string, std::string> string_map;
typedef std::vector<std::string> string_vector;
typedef std::vector<string_map> field_value;

const std::regex &get_http_field_re();
void parse_vary(const std::string &vary, string_vector &ret);
void parse_http_field(const std::string &value, field_value &ret);

}


#endif //CACHE_PROXY_HEADER_H
