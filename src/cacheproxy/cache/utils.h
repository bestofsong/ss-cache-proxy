#ifndef CACHE_UTILS_H
#define CACHE_UTILS_H
#include<string>

namespace smartstudy {
  using std::string;
  string make_request_key(
      const string &host,
      const string &port,
      const string &target
      );
}
#endif
