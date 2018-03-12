//
// Created by wansong on 12/03/2018.
//

#include <cacheproxy/metadb/metadb.h>

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

}