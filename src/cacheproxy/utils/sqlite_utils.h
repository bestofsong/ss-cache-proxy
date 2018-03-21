//
// Created by wansong on 18/03/2018.
//

#ifndef CACHE_PROXY_SQLITE_UTILS_H
#define CACHE_PROXY_SQLITE_UTILS_H

#include <string>
#include <map>

namespace smartstudy {

std::string to_sqlite_identifier(const std::string &name);
const std::string from_sqlite_identifier(const std::string &name);

typedef std::map<std::string, std::string> sqlite_record_row;

sqlite_record_row sqlite_query(const std::string &sql);

}


#endif //CACHE_PROXY_SQLITE_UTILS_H
