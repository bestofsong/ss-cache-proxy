//
// Created by wansong on 11/03/2018.
//

#include <cacheproxy/utils/string.h>
#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <third-party/boost_1_66_0/boost/algorithm/string/trim.hpp>

namespace smartstudy {

void snakecase(std::string &ret) {
  for(auto &c: ret) {
    if (c == '-') {
      c = '_';
    } else if (c >= 'A' && c <= 'Z') {
      c = c - 'A' + 'a';
    }
  }
}


void split(const std::string &str, const char delim, std::vector<std::string> &ret) {
  ret.clear();
  if (str.empty()) {
    return;
  }

  std::stringstream ss(str);
  std::string item;

  while (std::getline(ss, item, delim)) {
    boost::trim(item);
    ret.emplace_back(std::move(item));
  }
}

}
