#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <cacheproxy/client/connection_manager.h>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

using boost::system::error_code;

int main(int argc, char** argv) {
  if(argc != 5) {
    std::cerr << "arguments: host, port, target, content" << std::endl;
    return -1;
  }

  auto const host = argv[1];
  auto const port = argv[2];
  auto const target = argv[3];
  auto const content = argv[4];
  int version = 11;

  boost::asio::io_context ioc{1};
  smartstudy::connection_manager cm(ioc);

  std::string resp_text;
  smartstudy::request_callback_t<http::empty_body, http::string_body> cb = [&](
        const smartstudy::request_t<http::empty_body> &req,
        const smartstudy::response_t<http::string_body> &resp,
        const error_code &ec) {
    std::cout << "host: " << host << ", port: " << port << ", target: " << target << ": " << std::endl;
    if (ec) {
      std::cout << "load failed: " << ec.message() << std::endl;
    } else {
      std::cout << "load succeded: " << resp.body() << std::endl;
    }
    resp_text = resp.body();
  };

  std::map<std::string, std::string> headers;

  smartstudy::request_t <http::empty_body> req;
  {
    req.set(http::field::host, host);
    req.method(http::verb::get);
    req.target(target);
    req.version(version);
    for (const auto &kv: headers) {
      req.set(http::string_to_field(kv.first), kv.second);
    }
  }

  cm.get(std::move(req), port, std::move(cb));
  ioc.run();
  return resp_text == content ? 0 : -1;
}

