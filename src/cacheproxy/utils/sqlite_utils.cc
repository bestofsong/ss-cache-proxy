//
// Created by wansong on 18/03/2018.
//

#include "sqlite_utils.h"
#include <cacheproxy/utils/string.h>
#include <map>
#include <cassert>

namespace smartstudy {

typedef std::map<std::string, std::string> sqlite_field_transform_map;

std::string to_sqlite_identifier(const std::string &name) {
  assert(!name.empty());
  if (name[0] == '"') {
    return name;
  }

  return '"' + name + '"';
}

const std::string from_sqlite_identifier(const std::string &name) {
  assert(!name.empty());
  std::string ret = name;
  auto it = ret.begin();
  while (*it == '"') {
    it = ret.erase(it);
  }

  if (ret.empty()) {
    return ret;
  }

  auto rit = ret.end() - 1;
  while (*rit == '"') {
    auto next = rit - 1;
    ret.erase(rit);
    rit = next;
  }

  return ret;
}




}