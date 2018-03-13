//
// Created by wansong on 12/03/2018.
//

#ifndef CACHE_PROXY_METADB_H
#define CACHE_PROXY_METADB_H

#include <string>
#include <vector>
#include <cacheproxy/metadb/sqlite_error_category.h>

namespace smartstudy {

struct field_descriptor {
  std::string name;
  std::string type;
  std::string constraints; // primary key, not null, unique, check ..., default ...
  field_descriptor() = default;
  ~field_descriptor() = default;
};

struct table_descriptor {
  std::string name;
  bool if_not_exists;
  std::vector<field_descriptor> fields;
  std::string constaints; // primary key
  table_descriptor(
    std::string &&n,
    bool i,
    std::vector<field_descriptor> &&f,
    std::string &&c):
      name(std::move(n)), if_not_exists(i), fields(std::move(f)), constaints(std::move(c)) {}
  ~table_descriptor() = default;
};

std::string build_sql(const table_descriptor &schema);

}

#endif //CACHE_PROXY_METADB_H
