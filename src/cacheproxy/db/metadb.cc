//
// Created by wansong on 12/03/2018.
//

#include <cacheproxy/db/metadb.h>
#include <tuple>
#include <cassert>


namespace smartstudy {

std::string build_sql(const table_descriptor &schema) {
  assert(!schema.name.empty());
  assert(!schema.fields.empty());

  std::string ret;
  ret += "CREATE TABLE ";
  if (schema.if_not_exists) {
    ret += "IF NOT EXISTS ";
  }
  ret += schema.name + " ";

  ret += "( ";

  for (int ii = 0; ii < schema.fields.size(); ++ii) {
    const auto& field = schema.fields[ii];
    assert(!field.name.empty());
    assert(!field.type.empty());
    ret += field.name + " " + field.type + field.constraints;
    if (ii != schema.fields.size() - 1) {
      ret += ", ";
    }
  }

  if (!schema.constaints.empty()) {
    ret += ", " + schema.constaints;
  }

  ret += " );";

  return ret;
}


std::string build_sql(const add_column_descriptor &schema) {
  assert(!schema.name.empty());

  std::string ret;
  ret += "ALTER TABLE ";
  ret += schema.name + " ";

  ret += "ADD COLUMN ";
  ret += schema.field.name + " ";
  ret += schema.field.type;
  if (schema.field.constraints.empty()) {
    ret += ";";
  } else {
    ret += " " + schema.field.constraints + ";";
  }

  return ret;
}



std::string build_sql(const insert_update_descriptor &schema) {
  assert(!schema.table.empty());
  assert(!schema.kv_pairs.empty());

  std::string ret;
  ret += "INSERT OR REPLACE INTO " + schema.table + " ";

  ret += "( ";
  for (int ii = 0; ii < schema.kv_pairs.size(); ++ii) {
    const std::string &key = std::get<0>(schema.kv_pairs[ii]);
    assert(!key.empty());
    ret += key;
    if (ii != schema.kv_pairs.size() - 1) {
      ret += ", ";
    }
  }
  ret += " ) VALUES ( ";


  for (int ii = 0; ii < schema.kv_pairs.size(); ++ii) {
    const std::string &value = std::get<1>(schema.kv_pairs[ii]);
    ret += value.empty() ? "'null'" : "'" + value + "'";
    if (ii != schema.kv_pairs.size() - 1) {
      ret += ", ";
    }
  }
  ret += " );";

  return ret;
}
}