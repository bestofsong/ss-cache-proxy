//
// Created by wansong on 11/03/2018.
//

#ifndef CACHE_PROXY_HEADER_H
#define CACHE_PROXY_HEADER_H

#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace smartstudy {

void parse_vary(const std::string &vary, std::vector<std::string> &ret);

}


#endif //CACHE_PROXY_HEADER_H
