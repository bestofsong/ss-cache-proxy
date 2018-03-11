//
// Created by wansong on 11/03/2018.
//

#ifndef CACHE_PROXY_STRING_H
#define CACHE_PROXY_STRING_H

#include <string>

namespace smartstudy {

void snakecase(std::string &ret);

void split(const std::string &str, const char delim, std::vector<std::string> &ret);

}


#endif //CACHE_PROXY_STRING_H
