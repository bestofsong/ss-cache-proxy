#include <cacheproxy/cache/utils.h>

namespace smartstudy {
  using std::string;
  string make_request_key(
      const string &host,
      const string &port,
      const string &target
      ) {
    return host + "-" + port + '-' + target;
  }
}
