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

static const unsigned long RESERVED_MAX_UNSIGNED_LONG = (unsigned long)-1;

typedef std::map<std::string, std::string> string_map;
typedef std::vector<std::string> string_vector;
typedef std::vector<string_map> field_value;

const std::regex &get_http_field_re();

void parse_range(const std::string& str, std::vector<std::pair<unsigned long, unsigned long>> &ret);
void parse_content_range(const std::string& range, unsigned long& first, unsigned long& last, unsigned long& total);
void parse_http_field(const std::string &value, field_value &ret);

bool is_same_header(const std::string &a, const std::string &b);

}


#endif //CACHE_PROXY_HEADER_H
