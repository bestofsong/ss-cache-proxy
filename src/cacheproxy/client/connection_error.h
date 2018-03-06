#ifndef CONNECTION_ERROR_H
#define CONNECTION_ERROR_H

#include <boost/systeom/error_code>
namespace smartstudy {
  using boost::system::error_category;

  enum class connection_error {
    cancel, timeout, unknown,
  };

  struct connection_category : error_category
  {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
  };

  std::error_code make_error_code(connection_error e);
}

#endif
