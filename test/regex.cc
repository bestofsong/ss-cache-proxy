//
// Created by wansong on 11/03/2018.
//

#include <iostream>
#include <regex>
#include <cacheproxy/http/regex.h>

int main() {
  std::string str = " text/plain ; q = 0.5 ; charset = \"utf-16\"";
  std::smatch sm;
  return 0;
  while (true) {
    std::cout << "result for: " << str << std::endl;
    if (std::regex_match(str, sm, smartstudy::get_http_field_re())) {
      for (auto i = 0; i < sm.size(); i++) {
        std::cout << i << ": " << sm[i] << std::endl;
      }
    } else {
      std::cout << "failed to match" << std::endl;
    }

    std::cout << "Type in str like \"" << str << "\": " << std::endl;
    if (!getline (std::cin, str)) {
      break;
    }
  }
}