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


SCENARIO( "metadb tests", "[metadb]" ) {
  const char *path = "test.db";
  remove(path);

  GIVEN("fresh db file") {
    SQLite::Database db(path, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    smartstudy::field_descriptor id = { "id", "INTEGER PRIMARY KEY" };
    smartstudy::field_descriptor name = { "name", "text" };
    smartstudy::table_descriptor table = { "user", true, { id, name }, "" };
    std::string sql = smartstudy::build_sql(table);

    REQUIRE(!db.tableExists("user"));
    db.exec(sql);
    REQUIRE(db.tableExists("user"));
  }
}

