#include <cacheproxy/client/connection_error.h>

namespace smartstudy {

  const char* connection_category::name() const noexcept {
    return "connection_category";
  }

  std::string connection_category::message(int ev) const {
    switch (static_cast<connection_error>(ev)) {
      case connection_error::cancel:
        return "connection is cancelled";

      case connection_error::timeout:
        return "connection timeout";

      case connection_error::unknown:
        return "connection error unknown";

      default:
        return "(unrecognized error)";
    }
  }

  const connection_category connection_category_inst {};

  std::error_code make_error_code(connection_error e) {
    return {static_cast<int>(e), connection_category_inst};
  }
}

