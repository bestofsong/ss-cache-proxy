//
// Created by wansong on 12/03/2018.
//

#include "sqlite_error_category.h"

namespace smartstudy {

const char* sqlite_error_category::name() const noexcept {
  return "sqlite_error_category";
}

std::string sqlite_error_category::message(int ev) const {
  switch (static_cast<sqlite_error>(ev)) {
    case sqlite_error::SQLITE_OK         : return "Successful result";
    case sqlite_error::SQLITE_ERROR      : return "Generic error";
    case sqlite_error::SQLITE_INTERNAL   : return "Internal logic error in SQLite";
    case sqlite_error::SQLITE_PERM       : return "Access permission denied";
    case sqlite_error::SQLITE_ABORT      : return "Callback routine requested an abort";
    case sqlite_error::SQLITE_BUSY       : return "The database file is locked";
    case sqlite_error::SQLITE_LOCKED     : return "A table in the database is locked";
    case sqlite_error::SQLITE_NOMEM      : return "A malloc() failed";
    case sqlite_error::SQLITE_READONLY   : return "Attempt to write a readonly database";
    case sqlite_error::SQLITE_INTERRUPT  : return "Operation terminated by sqlite3_interrupt()";
    case sqlite_error::SQLITE_IOERR      : return "Some kind of disk I/O error occurred";
    case sqlite_error::SQLITE_CORRUPT    : return "The database disk image is malformed";
    case sqlite_error::SQLITE_NOTFOUND   : return "Unknown opcode in sqlite3_file_control()";
    case sqlite_error::SQLITE_FULL       : return "Insertion failed because database is full";
    case sqlite_error::SQLITE_CANTOPEN   : return "Unable to open the database file";
    case sqlite_error::SQLITE_PROTOCOL   : return "Database lock protocol error";
    case sqlite_error::SQLITE_EMPTY      : return "Not used";
    case sqlite_error::SQLITE_SCHEMA     : return "The database schema changed";
    case sqlite_error::SQLITE_TOOBIG     : return "String or BLOB exceeds size limit";
    case sqlite_error::SQLITE_CONSTRAINT : return "Abort due to constraint violation";
    case sqlite_error::SQLITE_MISMATCH   : return "Data type mismatch";
    case sqlite_error::SQLITE_MISUSE     : return "Library used incorrectly";
    case sqlite_error::SQLITE_NOLFS      : return "Uses OS features not supported on host";
    case sqlite_error::SQLITE_AUTH       : return "Authorization denied";
    case sqlite_error::SQLITE_FORMAT     : return "Not used";
    case sqlite_error::SQLITE_RANGE      : return "2nd parameter to sqlite3_bind out of range";
    case sqlite_error::SQLITE_NOTADB     : return "File opened that is not a database file";
    case sqlite_error::SQLITE_NOTICE     : return "Notifications from sqlite3_log()";
    case sqlite_error::SQLITE_WARNING    : return "Warnings from sqlite3_log()";
    case sqlite_error::SQLITE_ROW        : return "sqlite3_step() has another row ready";
    case sqlite_error::SQLITE_DONE       : return "sqlite3_step() has finished executing";
    default:
      return "(other error)";
  }
}

const sqlite_error_category sqlite_error_category_inst {};

std::error_code make_error_code(sqlite_error e) {
  return {static_cast<int>(e), sqlite_error_category_inst};
}
}
