//
// Created by wansong on 12/03/2018.
//

#ifndef CACHE_PROXY_METADB_H
#define CACHE_PROXY_METADB_H

#include <string>
#include <vector>

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
  std::vector<field_descriptor> fields;
  bool if_not_exists;
  std::string constaints; // primary key alike
  table_descriptor(
    std::string &&n,
    std::vector<field_descriptor> &&f,
    bool i = true,
    std::string &&c = ""):
      name(std::move(n)), fields(std::move(f)), if_not_exists(i), constaints(std::move(c)) {}
  ~table_descriptor() = default;
};
// 创建表
std::string build_sql(const table_descriptor &schema) noexcept;



struct add_column_descriptor {
  std::string name;
  // only support add column
  field_descriptor field;
};
// 修改表：增加列
std::string build_sql(const add_column_descriptor &schema) noexcept ;



typedef std::vector<std::tuple<std::string, std::string>> record_values;
struct insert_update_descriptor {
  std::string table;
  record_values kv_pairs;
};
// 插入或更新
std::string build_sql(const insert_update_descriptor &schema) noexcept;



}

#endif //CACHE_PROXY_METADB_H
