#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

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
#include <cacheproxy/cache/utils.h>


namespace smartstudy {
using boost::asio::io_context;
using std::string;
using std::shared_ptr;


class connection_manager
{
  public:
  explicit connection_manager(io_context& ioc): m_ioc(ioc) {}

  template <typename RequestBody, typename ResponseBody>
  void get(
    request_t <RequestBody> &&req,
      const string &port,
      request_callback_t<RequestBody, ResponseBody>&& callback){
    const string key = make_request_key(req, port);

    shared_ptr<void> conn = get_connection(key);
    connection<RequestBody, ResponseBody> *ptr = nullptr;

    if (conn) {
      ptr = static_cast<connection<RequestBody, ResponseBody> *>(conn.get());
      ptr ->register_callback(std::move(callback));
      return;
    }

    auto new_conn = add_connection<RequestBody, ResponseBody>(
      key,
      [=, &conn](
        const request_t<RequestBody>& req,
        const response_t<ResponseBody>& resp,
        const error_code& ec) {
        callback(req, resp, ec);
        delete_connection(key);
      }
    );

    conn.swap(new_conn);
    ptr = static_cast<connection<RequestBody, ResponseBody> *>(conn.get());

    ptr ->run(std::move(req), port);
  }


  private:
  io_context &m_ioc;
  std::map<string, shared_ptr<void>> m_connections;
  connection_manager(connection_manager& ioc) = delete;
  connection_manager&operator=(connection_manager &) = delete;

  shared_ptr<void> get_connection(const string&key) {
    auto it = m_connections.find(key);
    shared_ptr<void> ret;
    if (it != m_connections.end()) {
      ret = it ->second;
    }
    return ret;
  }

  template <typename RequestBody, typename ResponseBody>
  shared_ptr<void> add_connection(const string &key, request_callback_t<RequestBody, ResponseBody> && cb) {
    auto conn = std::make_shared<connection<RequestBody, ResponseBody>>(m_ioc, std::move(cb));
    m_connections[key] = conn;
    return conn;
  }

  void delete_connection(const string &key) {
    m_connections.erase(key);
  }
};

}

#endif
