//
// Created by wansong on 10/03/2018.
//

#ifndef CACHE_PROXY_HTTP_CACHE_MANAGER_H
#define CACHE_PROXY_HTTP_CACHE_MANAGER_H

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

class http_cache_manager {};

}


#endif //CACHE_PROXY_HTTP_CACHE_MANAGER_H
