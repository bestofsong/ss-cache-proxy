#ifndef CACHEPROXY_TYPEDEFS_H
#define CACHEPROXY_TYPEDEFS_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>


namespace smartstudy {

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
using boost::beast::string_view;


template <typename Body>
using request_t = http::request<Body>;

template <typename Body>
using response_t = http::response<Body>;

typedef std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>> strand_ptr;

template <typename RequestBody, typename ResponseBody>
using request_callback_t = std::function<void(const request_t<RequestBody>&, const response_t<ResponseBody>&, const boost::system::error_code&)>;

template <typename RequestBody>
class handler_params_t {
  public:
    handler_params_t(
        request_t<RequestBody> &&r,
        string_view &&d,
        tcp::socket &s,
        strand_ptr ps):
      req(r), doc_root(d), sock(s), p_strand(ps) {}

    handler_params_t(handler_params_t &&h_params):
      handler_params_t(
          std::move(h_params).req,
          std::move(h_params).doc_root,
          std::move(h_params).sock,
          std::move(h_params).p_strand
          ){}

  public:
    request_t<RequestBody> req;
    string_view doc_root;
    tcp::socket& sock;
    strand_ptr p_strand;

  private:
    handler_params_t(handler_params_t &) = delete;
    handler_params_t& operator=(handler_params_t &) = delete;
};

typedef std::function<void(boost::system::error_code, std::size_t, bool)> on_send_response_t;

template <typename RequestBody>
using handler_function_t =
  std::function<void(handler_params_t<RequestBody>&&, on_send_response_t&&)>;

}

#endif
