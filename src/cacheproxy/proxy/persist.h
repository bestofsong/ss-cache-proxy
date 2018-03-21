//
// Created by wansong on 16/03/2018.
//

#ifndef CACHE_PROXY_PERSIST_H
#define CACHE_PROXY_PERSIST_H


#include <system_error>
#include <map>
#include <vector>
#include <string>

typedef std::map<std::string, std::string> cache_response_t;

struct cache_request_t {
  std::string host;
  std::string path;
  std::string method;
  std::string version;
  std::map<std::string, std::string> headers;
};


class persist {
  virtual void set_cache_response(
    const cache_request_t &req, const cache_response_t &resp) = 0;

  virtual void get_cache_response(
    const cache_request_t &req, std::vector<cache_response_t> &resp) = 0;
};


#endif //CACHE_PROXY_PERSIST_H
