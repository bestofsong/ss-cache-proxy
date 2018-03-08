#ifndef CACHEPROXY_H
#define CACHEPROXY_H
#include <cacheproxy/config.h>
#include <cacheproxy/server/server.h>
#include <cacheproxy/handlers/file_handler.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <boost/asio/signal_set.hpp>
#include <exception>


namespace convenience_std {
  using std::make_shared;
  using std::string;
  using std::shared_ptr;
}

namespace convenience_boost {
  using boost::asio::io_context;
  using namespace boost::asio::ip;
}

namespace smartstudy {
  using namespace convenience_std;
  using namespace convenience_boost;

  class CacheProxy {
  public:
    bool start(const string& addr, unsigned short port, const string& docroot) {
      create_server(addr, port, docroot);
      io_context& ioc = *pioc;
      register_cleanup();
      ioc.run();
      return true;
    }

    bool start_async(const string& addr, unsigned short port, const string& docroot) {
      create_server(addr, port, docroot);
      io_context& ioc = *pioc;
      th = make_shared<std::thread>([&ioc]{ ioc.run(); });
      th ->detach();
      register_cleanup();
      return true;
    }


    void stop() {
      std::cout << "will stop" << std::endl;
      try {
        svr ->stop();
        pioc ->stop();
      } catch (boost::exception &e) {
        std::cerr << "writing lots of zeros failed\n" << std::flush;
        throw;
      }
    }


    unsigned short get_port() const {
      return svr ->endpoint().port();
    }

    typedef smartstudy::server<http::string_body> file_server;

  private:
    shared_ptr<std::thread> th;
    shared_ptr<file_server> svr;
    shared_ptr<io_context> pioc;
    shared_ptr<string> doc_root;
    shared_ptr<boost::asio::signal_set> m_signal_set;


    void create_server(const string& addr, unsigned short port, const string& docroot) {
      auto const address = make_address(addr);
      doc_root = make_shared<string>(docroot);
      // The io_context is required for all I/O
      pioc = make_shared<io_context>(1);

      // Create and launch a listening port
      svr = make_shared<file_server>(
                                 *pioc,
                                 tcp::endpoint{address, port},
                                 *doc_root,
                                 handle_file_request<http::string_body>
                                 );
      svr ->run();
      std::cout << "listening on port: " << get_port() << std::endl;
    }


    void register_cleanup() {
      m_signal_set = make_shared<boost::asio::signal_set>(*pioc, SIGINT, SIGTERM);
      m_signal_set ->async_wait(std::bind(&CacheProxy::stop, this));
    }
  };


}

#endif
