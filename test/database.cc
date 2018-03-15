//
// Created by wansong on 12/03/2018.
//

#define CATCH_CONFIG_MAIN
#include <third-party/catch/catch.hpp>
#include <cacheproxy/db/metadb.h>
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

  smartstudy::table_descriptor schema = createStudentSql();
  std::string sql_create_table = smartstudy::build_sql(schema);
  auto db = createTable(path);

  GIVEN("create table") {
    REQUIRE(!db ->tableExists("user"));
    db ->exec(sql_create_table.c_str());
    REQUIRE(db ->tableExists("user"));
  }

  GIVEN("insert or replace into table") {
    db ->exec(sql_create_table.c_str());
    smartstudy::insert_update_descriptor insert{
      "user",
      { { "name", "wansong" }, { "height", "180" } }
    };
    const std::string sql = smartstudy::build_sql(insert);

    SQLite::Statement stmt(*db, sql.c_str());
    REQUIRE(stmt.exec() > 0);
    std::string h = db ->execAndGet("SELECT height FROM user WHERE name='wansong'");
    REQUIRE(h == "180");
  }

  GIVEN("alter table") {
    db ->exec(sql_create_table.c_str());
    smartstudy::add_column_descriptor alter{ "user", { "wealth", "text" } };
    const std::string sql = smartstudy::build_sql(alter);
    SQLite::Statement stmt(*db, sql.c_str());
    REQUIRE(stmt.exec() == 0);

    {
      smartstudy::insert_update_descriptor insert{
        "user",
        { { "name", "wansong" }, { "height", "180" }, { "wealth", "1000000000" } }
      };
      const std::string sql2 = smartstudy::build_sql(insert);
      SQLite::Statement stmt2(*db, sql2.c_str());
      REQUIRE(stmt2.exec() > 0);
      std::string w = db ->execAndGet("SELECT wealth FROM user WHERE name='wansong'");
      REQUIRE(w == "1000000000");
    }
  }
}

