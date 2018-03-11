//
// Created by wansong on 11/03/2018.
//

#include <cacheproxy/http/header.h>
#include <cacheproxy/utils/string.h>


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

}

