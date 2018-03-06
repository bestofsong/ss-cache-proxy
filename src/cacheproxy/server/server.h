#ifndef CACHEPROXY_SERVER_SERVER_H
#define CACHEPROXY_SERVER_SERVER_H
#include <cacheproxy/handlers/file_handler.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>


using tcp = boost::asio::ip::tcp;

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


namespace smartstudy {
  void fail(boost::system::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
  }


  // Handles an HTTP server connection
  template <typename RequestBody>
  class session : public std::enable_shared_from_this<session<RequestBody>>
  {
    tcp::socket socket_;
    strand_ptr mp_strand;
    boost::beast::flat_buffer buffer_;
    std::string const& doc_root_;
    http::request<http::string_body> req_;
    std::shared_ptr<void> res_;
    handler_function_t<RequestBody> m_handler;

    public:
    // Take ownership of the socket
    explicit
      session(
          tcp::socket&& socket,
          std::string const& doc_root,
          handler_function_t<RequestBody> handler
          )
      : socket_(std::move(socket))
        , mp_strand(new boost::asio::strand<boost::asio::io_context::executor_type>(socket.get_executor()))
        , doc_root_(doc_root)
        , m_handler(handler) {}

    // Start the asynchronous operation
    void run() {
      do_read();
    }

    void do_read() {
      // Read a request
      http::async_read(socket_, buffer_, req_,
          boost::asio::bind_executor(
            *mp_strand.get(),
            std::bind(
              &session<RequestBody>::on_read,
              this ->shared_from_this(),
              std::placeholders::_1,
              std::placeholders::_2)));
    }

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);

      // This means they closed the connection
      if(ec == http::error::end_of_stream) {
        return do_close();
      }
      if(ec) {
        return fail(ec, "read");
      }
      // Send the response
      m_handler(
        handler_params_t<RequestBody>(
          std::move(req_),
          boost::beast::string_view (doc_root_.c_str()),
          socket_,
          mp_strand
        ),
        std::bind(
          &session<RequestBody>::on_write,
          this ->shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2,
          std::placeholders::_3
        )
      );
    }

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred, bool close) {
      boost::ignore_unused(bytes_transferred);

      if(ec) {
        return fail(ec, "write");
      }

      if(close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
      }

      // We're done with the response so delete it
      res_ = nullptr;

      // Read another request
      do_read();
    }

    void do_close() {
      // Send a TCP shutdown
      boost::system::error_code ec;
      socket_.shutdown(tcp::socket::shutdown_send, ec);
      if (ec) {
        std::cerr << "failed to shutdown connection, error: "
                  << ec.message()
                  << std::endl;
      }
      socket_.close(ec);
      if (ec) {
        std::cerr << "failed to close connection socket, error: "
                  << ec.message()
                  << std::endl;
      }
      // At this point the connection is closed gracefully
    }
  };


  bool is_valid_tcp_port(int port) {
    return 1024 <= port && port <= 65535;
  }


  //------------------------------------------------------------------------------
  template <typename RequestBody = http::string_body>
  class server : public std::enable_shared_from_this<server<RequestBody>>
  {
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    tcp::endpoint m_endpoint;
    std::string const& m_doc_root;
    handler_function_t<RequestBody> m_handler;

    public:
    server(
        boost::asio::io_context& ioc,
        tcp::endpoint endpoint,
        std::string const& doc_root,
        handler_function_t<RequestBody> handler
        )
      : m_acceptor(ioc)
        , m_socket(ioc)
        , m_endpoint(endpoint)
        , m_doc_root(doc_root)
        , m_handler(handler) {}

    const tcp::endpoint &endpoint() {
      return m_endpoint;
    }

    // Start accepting incoming connections
    void run(int port = -1) {
      if (is_valid_tcp_port(port)) {
        m_endpoint = tcp::endpoint(m_endpoint.address(), static_cast<unsigned short>(port));
      }

      boost::system::error_code ec;

      // Open the acceptor
      if(!m_acceptor.is_open()) {
        m_acceptor.open(m_endpoint.protocol(), ec);
        if(ec) {
          fail(ec, "open");
          return;
        }
      }

      m_acceptor.set_option(tcp::acceptor::reuse_address(true));

      // Bind to the server address
      m_acceptor.bind(m_endpoint, ec);
      if (ec == boost::system::errc::address_in_use) {
        int next_port = port != -1 ? port - 1 : 65535;
        if (!is_valid_tcp_port(next_port)) {
          fail(ec, "bind");
          return;
        }
        run(next_port);
      } else if (ec){
        fail(ec, "bind");
        return;
      }

      // Start listening for connections
      m_acceptor.listen(
          boost::asio::socket_base::max_listen_connections, ec);
      if(ec) {
        fail(ec, "listen");
        return;
      }

      do_accept();
    }

    void stop() {
      boost::system::error_code ec;
      try {
        m_acceptor.cancel();
        m_acceptor.close();
        m_socket.close(ec);
        std::cout << "did close socket" << std::endl;
      } catch (boost::exception &e) {
        std::cerr << "failed to close socket" << std::endl;
      }
    }

    void do_accept() {
      m_acceptor.async_accept(
          m_socket,
          std::bind(
            &server<RequestBody>::on_accept,
            this ->shared_from_this(),
            std::placeholders::_1));
    }

    void on_accept(boost::system::error_code ec) {
      if(ec) {
        fail(ec, "accept");
      } else {
        tcp::socket new_sock{m_socket.get_io_context()};
        std::make_shared<session<RequestBody>>(
            std::move(m_socket),
            m_doc_root,
            m_handler)->run();
        m_socket = std::move(new_sock);
      }


      // Accept another connection
      do_accept();
    }
  };

}

#endif
