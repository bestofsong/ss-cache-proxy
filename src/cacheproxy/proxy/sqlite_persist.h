//
// Created by wansong on 16/03/2018.
//

#ifndef CACHE_PROXY_SQLITE_PERSIST_H
#define CACHE_PROXY_SQLITE_PERSIST_H


#include "persist.h"
#include <cacheproxy/config.h>
#include <SQLiteCpp/Database.h>
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace smartstudy {


class sqlite_persist: public persist {
public:
  typedef std::shared_ptr<sqlite_persist> Ptr;

  sqlite_persist(const char *path);

  void initialize();

  void set_cache_response(
    const cache_request_t &req, const cache_response_t &resp);

  void get_cache_response(
    const cache_request_t &req, std::vector<cache_response_t> &resp);

  sqlite_persist(sqlite_persist &) = delete;
  sqlite_persist&operator=(sqlite_persist &) = delete;


#ifdef SS_TEST
  std::shared_ptr<SQLite::Database> db;
#endif

private:
#ifndef SS_TEST
  std::shared_ptr<SQLite::Database> db;
#endif

  void create_record_table();
  void create_vary_table();

  std::string query_vary(const cache_request_t &req);

  long long save_vary(
    const cache_request_t &req, const cache_response_t &resp);
};


};


#endif //CACHE_PROXY_SQLITE_PERSIST_H
