#ifndef CONNECTION_H
#define CONNECTION_H

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
#include <cacheproxy/client/connection_base.h>


namespace smartstudy {

template <typename RequestBody, typename ResponseBody>
class connection: public connection_base<RequestBody, ResponseBody> {
  using connection_base<RequestBody, ResponseBody>::connection_base;
};


static const int HTTP_RESPONSE_READ_BUFFER_SIZE = 1024 * 512;

template <typename RequestBody>
class connection <RequestBody, http::buffer_body>
{
  public:
  typedef http::buffer_body ResponseBody;
  typedef request_callback_t<RequestBody, ResponseBody> request_callback_type; 

  public:
  explicit
    connection(boost::asio::io_context& ioc, request_callback_type &&callback)
    : m_resolver(ioc), m_socket(ioc) {
      register_callback(std::move(callback));
      m_resp.body().data = &body_parser_buffer[0];
      m_resp.body().size = HTTP_RESPONSE_READ_BUFFER_SIZE;
    }

  void run(const string &host, const string &port, const string &target, int version) {
    // fixme: for post/put request, need overload
    m_req.version(version);
    m_req.method(http::verb::get);
    m_req.target(target);
    m_req.set(http::field::host, host);
    m_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    m_resolver.async_resolve(
        host,
        port,
        std::bind(
          &connection::on_resolve,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
  }


  void register_callback(request_callback_type &&cb) {
    m_callbacks.push_back(cb);
  }


  void on_resolve(boost::system::error_code ec, tcp::resolver::results_type results) {
    if(ec) {
      notify_request_end(m_req, m_resp, ec);
      return;
    }

    boost::asio::async_connect(
        m_socket,
        results.begin(),
        results.end(),
        std::bind(
          &connection::on_connect,
          this,
          std::placeholders::_1));
  }


  void on_connect(boost::system::error_code ec) {
    if(ec) {
      notify_request_end(m_req, m_resp, ec);
      return;
    }

    http::async_write(m_socket, m_req,
        std::bind(
          &connection::on_write,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
  }


  void on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec) {
      notify_request_end(m_req, m_resp, ec);
      return;
    }

    http::async_read(m_socket, m_buffer, m_resp,
        std::bind(
          &connection::on_read,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
  }


  void on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    m_resp.body().data = &body_parser_buffer[0];
    m_resp.body().size = HTTP_RESPONSE_READ_BUFFER_SIZE - m_resp.body().size;

    // consume requested data
    notify_request_end(m_req, m_resp, ec);

    m_socket.shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes so don't bother dealing with it.
    if(ec && ec != boost::system::errc::not_connected) {
      std::cerr << "maybe server closed: " << ec.message() << std::endl;
    }
    // If we get here then the connection is closed gracefully
  }

  private:
  void notify_request_end(
      const request_t<RequestBody> &req,
      const response_t<ResponseBody> &resp,
      const error_code &ec) {
    auto immutable = std::vector<request_callback_type>(
         m_callbacks.begin(), m_callbacks.end());
    m_callbacks.clear();
    for(auto it = immutable.rbegin(); it != immutable.rend(); ++it) {
      (*it)(req, resp, ec);
    }

    // handle nested request
    if (!m_callbacks.empty()) {
      notify_request_end(req, resp, ec);
    }
  }


  private:
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;

  char body_parser_buffer[HTTP_RESPONSE_READ_BUFFER_SIZE];
  tcp::resolver m_resolver;
  tcp::socket m_socket;
  boost::beast::flat_buffer m_buffer; // (Must persist between reads)
  request_t<RequestBody> m_req;
  response_t<ResponseBody> m_resp;
  std::vector<request_callback_type> m_callbacks;
};


}

#endif
