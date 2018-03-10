#ifndef CACHEPROXY_HANDLERS_FILE_HANDLER_H
#define CACHEPROXY_HANDLERS_FILE_HANDLER_H
#include <cacheproxy/typedefs.h>
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
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>


namespace smartstudy {

//  template<typename T>
//  void log(T v) {
//    std::cout << "unknown message type for logging" << std::endl;
//  }
//
  void log(const std::string & msg) {
    std::cout << msg << std::endl;
  }


//  void log(const boost::beast::string_view & msg) {
//    std::cout << msg << std::endl;
//  }
//

  using boost::system::error_code;
  // Return a reasonable mime type based on the extension of a file.
  boost::beast::string_view mime_type(boost::beast::string_view path) {
    using boost::beast::iequals;
    auto const pos = path.rfind(".");
    auto const ext = pos == boost::beast::string_view::npos ?
      boost::beast::string_view{} : path.substr(pos);

    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
  }


  // Append an HTTP rel-path to a local filesystem path.
  // The returned path is normalized for the platform.
  std::string path_cat(boost::beast::string_view base, boost::beast::string_view path) {
    if(base.empty()) {
      return path.to_string();
    }

    std::string result = base.to_string();
    char constexpr path_separator = '/';
    if(result.back() == path_separator) {
      result.resize(result.size() - 1);
    }
    result.append(path.data(), path.size());
    return result;
  }


    template <typename RequestBody, typename ResponseBody>
    void do_send_response(handler_params_t<RequestBody> &&h_params, response_t<ResponseBody>&& msg, on_send_response_t &&on_send) {
      auto p_h_params = std::make_shared<handler_params_t<RequestBody>>(std::move(h_params));
      auto pmsg = std::make_shared<response_t<ResponseBody>>(std::move(msg));
      log("");
      http::async_write(
        p_h_params ->sock,
        *pmsg,
        boost::asio::bind_executor(
          *p_h_params ->p_strand.get(),
          [p_h_params, pmsg, on_send](error_code ec, std::size_t bytes_transferred) {
          if (ec) {
            log("did send, error: ");
            log(ec.message());
          } else {
            log("did send ok");
          }
          on_send(ec, bytes_transferred, pmsg ->need_eof());
        })
      );
    }

  template<class RequestBody>
    void handle_file_request(
        handler_params_t<RequestBody> &&h_params,
        on_send_response_t&& send) {
      auto req = h_params.req;
      auto doc_root = h_params.doc_root;
      auto version = req.version();
      auto keep_alive = req.keep_alive();
      auto const bad_request = [version, keep_alive](boost::beast::string_view why) {
        http::response<http::string_body> res{http::status::bad_request, version};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(keep_alive);
        res.body() = why.to_string();
        res.prepare_payload();
        return res;
      };

      auto const not_found = [version, keep_alive](boost::beast::string_view target) {
        http::response<http::string_body> res{http::status::not_found, version};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(keep_alive);
        res.body() = "The resource '" + target.to_string() + "' was not found.";
        res.prepare_payload();
        return res;
      };

      // Returns a server error response
      auto const server_error = [version, keep_alive](boost::beast::string_view what) {
        http::response<http::string_body> res{http::status::internal_server_error, version};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(keep_alive);
        res.body() = "An error occurred: '" + what.to_string() + "'";
        res.prepare_payload();
        return res;
      };

      // Make sure we can handle the method
      if( req.method() != http::verb::get &&
          req.method() != http::verb::head) {
        log("unknown method");
        return do_send_response(std::move(h_params),
            bad_request("Unknown HTTP-method"),
            std::move(send));
      }

      // Request path must be absolute and not contain "..".
      if( req.target().empty() ||
          req.target()[0] != '/' ||
          req.target().find("..") != boost::beast::string_view::npos) {

        log("illigal request target");
        return do_send_response(
            std::move(h_params),
            bad_request("Illegal request-target"),
            std::move(send)
            );
      }

      // Build the path to the requested file
      std::string path = path_cat(doc_root, req.target());
      if(req.target().back() == '/') {
        path.append("index.html");
      }

      // Attempt to open the file
      boost::beast::error_code ec;
      http::file_body::value_type body;
      body.open(path.c_str(), boost::beast::file_mode::scan, ec);

      // Handle the case where the file doesn't exist
      if(ec == boost::system::errc::no_such_file_or_directory) {
        auto target = req.target();
        return do_send_response(
            std::move(h_params),
            not_found(target),
            std::move(send)
            );
      }

      // Handle an unknown error
      if(ec) {
        log("unknown error");
        log(ec.message());
        return do_send_response(
            std::move(h_params),
            server_error(ec.message()),
            std::move(send)
            );
      }

      // Respond to HEAD request
      if(req.method() == http::verb::head) {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(body.size());
        res.keep_alive(req.keep_alive());
        return do_send_response(
            std::move(h_params),
            std::move(res),
            std::move(send)
            );
      }

      // Respond to GET request
      auto sz = body.size();
      http::response<http::file_body> res{
        std::piecewise_construct,
          std::make_tuple(std::move(body)),
          std::make_tuple(http::status::ok, req.version())
      };
      res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(http::field::content_type, mime_type(path));
      res.content_length(sz);
      res.keep_alive(req.keep_alive());

      log("sending file resp");
      return do_send_response(
          std::move(h_params),
          std::move(res),
          std::move(send)
          );
    }
}

#endif
