//
// Created by wansong on 11/03/2018.
//

#ifndef CACHE_PROXY_STRING_H
#define CACHE_PROXY_STRING_H

#include <string>
#include <set>

namespace smartstudy {

void snakecase(std::string &ret);

std::string snakecase(const std::string &ret);

void split(const std::string &str, const char delim, std::vector<std::string> &ret);

std::string escape_using(const std::string &str, const char &escape, std::set<char> &charsToEscape);

}


#endif //CACHE_PROXY_STRING_H
