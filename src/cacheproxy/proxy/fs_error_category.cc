//
// Created by wansong on 16/03/2018.
//

#include <cassert>
#include "fs_error_category.h"

namespace smartstudy {


const char* fs_error_category::name() const {
  return "fs_error_category";
}

std::string fs_error_category::message(int ev) const {
  switch (static_cast<fs_error>(ev)) {
    case fs_error::ok:
      return "ok";
    case fs_error::noent:
      return "no file";
    default:
      assert(false);
      return "unknown";
  }
}


const fs_error_category fs_error_category_inst {};

std::error_code make_error_code(fs_error e) {
  return {static_cast<int>(e), fs_error_category_inst};
}


}