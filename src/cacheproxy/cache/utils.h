#ifndef CACHE_UTILS_H
#define CACHE_UTILS_H
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include<string>
#include <cacheproxy/typedefs.h>

namespace smartstudy {
  using std::string;
template <typename RequestBody>
  string make_request_key(
      const request_t <RequestBody> &req,
      const string &port,
      const string &vary = ""
      ) {
  return req[http::field::host].to_string() + "-"
         + port + '-'
         + req.target().to_string();
}
}
#endif
