#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <assert.h>

int main() {
  SQLite::Database db("test.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
  assert(!db.tableExists("test"));
  db.exec("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)");
  assert(db.tableExists("test"));
  return 0;
}

