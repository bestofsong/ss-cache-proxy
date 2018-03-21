//
// Created by wansong on 10/03/2018.
//

#ifndef CACHE_PROXY_HTTP_CACHE_MANAGER_H
#define CACHE_PROXY_HTTP_CACHE_MANAGER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <cacheproxy/client/connection.h>
#include "sqlite_persist.h"


namespace smartstudy {

using boost::asio::io_context;
using std::string;
using std::shared_ptr;


class http_cache_manager {
public:
  http_cache_manager(const char *path): persist(new sqlite_persist(path)) {}

  void initialize() {
    persist ->initialize();
  }

  template <typename RequestBody>
  static string make_cache_key(
    const request_t <RequestBody> &req,
    const string &port,
    const string &vary = ""
  ) {
    return req[http::field::host].to_string() + "-"
           + port + '-'
           + req.target().to_string();
  }

private:
  std::shared_ptr<sqlite_persist> persist;
};


}


#endif //CACHE_PROXY_HTTP_CACHE_MANAGER_H
