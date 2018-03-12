//
// Created by wansong on 12/03/2018.
//

#ifndef CACHE_PROXY_METADB_H
#define CACHE_PROXY_METADB_H

#include <string>

namespace smartstudy {

struct field_descriptor {
  std::string name;
  std::string type;
  std::string foreign_key_to;
};

typedef std::vector<field_descriptor> table_descriptor;


}

#endif //CACHE_PROXY_METADB_H
