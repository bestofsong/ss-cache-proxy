//
// Created by wansong on 11/03/2018.
//

#include <regex>
#include "regex.h"

namespace smartstudy {

const std::regex &get_http_field_re() {
  static std::regex http_field_re("^\\s*([^;\\s]+)\\s*(?:;\\s*(.*)\\s*)?$");
  return http_field_re;
}


}

