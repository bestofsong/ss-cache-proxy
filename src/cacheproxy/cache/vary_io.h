//
// Created by wansong on 11/03/2018.
//

#ifndef CACHE_PROXY_VARY_IO_H
#define CACHE_PROXY_VARY_IO_H
#include <SQLiteCpp/Database.h>
#include <string>
#include <vector>
#include <cacheproxy/http/header.h>

namespace smartstudy {

class vary_io {
public:
  static void init() {
    // todo: 创建更新表：vary, version
    SQLite::Database db("test.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
  }

// 三类返回结果：
// { '\0' }代表没有vary（缺省）；
// { 'null' }代表不支持缓存；
// 其他取值代表支持相应的content-negotiation
  static void query(const std::string &url, std::vector<std::string> &ret) {
    std::string vary = get_vary(url);
    if (vary == "null") {
      ret = std::vector<std::string>();
    }

    if (vary == "") {
      ret = { "" };
    }

    parse_vary(vary, ret);
  }

private:
  static std::string get_vary(const std::string &url) {
    // todo: 查询数据库
    return "";
  }
};


}

#endif //CACHE_PROXY_VARY_IO_H
