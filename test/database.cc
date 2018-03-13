//
// Created by wansong on 12/03/2018.
//

#define CATCH_CONFIG_MAIN
#include <third-party/catch/catch.hpp>
#include <cacheproxy/metadb/metadb.h>
#include <iostream>
#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <assert.h>
#include <string>
#include <stdio.h>
#include <memory>

smartstudy::table_descriptor createStudentSql() {
  smartstudy::field_descriptor id = { "id", "INTEGER PRIMARY KEY" };
  smartstudy::field_descriptor name = { "name", "text unique" };
  smartstudy::field_descriptor height = { "height", "int" };
  return { "user", true, { id, name, height }, "" };
}

std::shared_ptr<SQLite::Database> createTable(const char *path) {
  return std::make_shared<SQLite::Database>(path, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
}

SCENARIO( "metadb tests", "[metadb]" ) {
  const char *path = "test.db";
  remove(path);

  GIVEN("create table") {
    smartstudy::table_descriptor schema = createStudentSql();
    std::string sql = smartstudy::build_sql(schema);
    auto db = createTable(path);
    REQUIRE(!db ->tableExists("user"));
    db ->exec(sql.c_str());
    REQUIRE(db ->tableExists("user"));
  }
}

