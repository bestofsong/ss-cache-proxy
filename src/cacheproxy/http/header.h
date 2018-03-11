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

void snakecase(std::string &ret) {
  for(auto &c: ret) {
    if (c == '-') {
      c = '_';
    } else if (c >= 'A' && c <= 'Z') {
      c = c - 'A' + 'a';
    } else {
      if (c < 'a' || c > 'z') {
        std::cout << "invalid character in header name: " << ret << ": " << c << std::endl;
        assert(false);
      }
    }
  }
}


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

}

#endif //CACHE_PROXY_HEADER_H
