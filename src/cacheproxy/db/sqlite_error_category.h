//
// Created by wansong on 12/03/2018.
//

#ifndef CACHE_PROXY_SQLITE_ERROR_CATEGORY_H
#define CACHE_PROXY_SQLITE_ERROR_CATEGORY_H


#include <boost/system/error_code.hpp>

namespace smartstudy {
using boost::system::error_category;

enum class sqlite_error {
  SQLITE_OK         =   0,   /* Successful result */
  SQLITE_ERROR      =   1,   /* Generic error */
  SQLITE_INTERNAL   =   2,   /* Internal logic error in SQLite */
  SQLITE_PERM       =   3,   /* Access permission denied */
  SQLITE_ABORT      =   4,   /* Callback routine requested an abort */
  SQLITE_BUSY       =   5,   /* The database file is locked */
  SQLITE_LOCKED     =   6,   /* A table in the database is locked */
  SQLITE_NOMEM      =   7,   /* A malloc() failed */
  SQLITE_READONLY   =   8,   /* Attempt to write a readonly database */
  SQLITE_INTERRUPT  =   9,   /* Operation terminated by sqlite3_interrupt()*/
  SQLITE_IOERR      =  10,   /* Some kind of disk I/O error occurred */
  SQLITE_CORRUPT    =  11,   /* The database disk image is malformed */
  SQLITE_NOTFOUND   =  12,   /* Unknown opcode in sqlite3_file_control() */
  SQLITE_FULL       =  13,   /* Insertion failed because database is full */
  SQLITE_CANTOPEN   =  14,   /* Unable to open the database file */
  SQLITE_PROTOCOL   =  15,   /* Database lock protocol error */
  SQLITE_EMPTY      =  16,   /* Not used */
  SQLITE_SCHEMA     =  17,   /* The database schema changed */
  SQLITE_TOOBIG     =  18,   /* String or BLOB exceeds size limit */
  SQLITE_CONSTRAINT =  19,   /* Abort due to constraint violation */
  SQLITE_MISMATCH   =  20,   /* Data type mismatch */
  SQLITE_MISUSE     =  21,   /* Library used incorrectly */
  SQLITE_NOLFS      =  22,   /* Uses OS features not supported on host */
  SQLITE_AUTH       =  23,   /* Authorization denied */
  SQLITE_FORMAT     =  24,   /* Not used */
  SQLITE_RANGE      =  25,   /* 2nd parameter to sqlite3_bind out of range */
  SQLITE_NOTADB     =  26,   /* File opened that is not a database file */
  SQLITE_NOTICE     =  27,   /* Notifications from sqlite3_log() */
  SQLITE_WARNING    =  28,   /* Warnings from sqlite3_log() */
  SQLITE_ROW        =  100,  /* sqlite3_step() has another row ready */
  SQLITE_DONE       =  101,  /* sqlite3_step() has finished executing */
  SQLITE_IOERR_READ               = (sqlite_error::SQLITE_IOERR| (1<<8)),
  SQLITE_IOERR_SHORT_READ         = (sqlite_error::SQLITE_IOERR | (2<<8)),
  SQLITE_IOERR_WRITE              = (sqlite_error::SQLITE_IOERR | (3<<8)),
  SQLITE_IOERR_FSYNC              = (sqlite_error::SQLITE_IOERR | (4<<8)),
  SQLITE_IOERR_DIR_FSYNC          = (sqlite_error::SQLITE_IOERR | (5<<8)),
  SQLITE_IOERR_TRUNCATE           = (sqlite_error::SQLITE_IOERR | (6<<8)),
  SQLITE_IOERR_FSTAT              = (sqlite_error::SQLITE_IOERR | (7<<8)),
  SQLITE_IOERR_UNLOCK             = (sqlite_error::SQLITE_IOERR | (8<<8)),
  SQLITE_IOERR_RDLOCK             = (sqlite_error::SQLITE_IOERR | (9<<8)),
  SQLITE_IOERR_DELETE             = (sqlite_error::SQLITE_IOERR | (10<<8)),
  SQLITE_IOERR_BLOCKED            = (sqlite_error::SQLITE_IOERR | (11<<8)),
  SQLITE_IOERR_NOMEM              = (sqlite_error::SQLITE_IOERR | (12<<8)),
  SQLITE_IOERR_ACCESS             = (sqlite_error::SQLITE_IOERR | (13<<8)),
  SQLITE_IOERR_CHECKRESERVEDLOCK  = (sqlite_error::SQLITE_IOERR | (14<<8)),
  SQLITE_IOERR_LOCK               = (sqlite_error::SQLITE_IOERR | (15<<8)),
  SQLITE_IOERR_CLOSE              = (sqlite_error::SQLITE_IOERR | (16<<8)),
  SQLITE_IOERR_DIR_CLOSE          = (sqlite_error::SQLITE_IOERR | (17<<8)),
  SQLITE_IOERR_SHMOPEN            = (sqlite_error::SQLITE_IOERR | (18<<8)),
  SQLITE_IOERR_SHMSIZE            = (sqlite_error::SQLITE_IOERR | (19<<8)),
  SQLITE_IOERR_SHMLOCK            = (sqlite_error::SQLITE_IOERR | (20<<8)),
  SQLITE_IOERR_SHMMAP             = (sqlite_error::SQLITE_IOERR | (21<<8)),
  SQLITE_IOERR_SEEK               = (sqlite_error::SQLITE_IOERR | (22<<8)),
  SQLITE_IOERR_DELETE_NOENT       = (sqlite_error::SQLITE_IOERR | (23<<8)),
  SQLITE_IOERR_MMAP               = (sqlite_error::SQLITE_IOERR | (24<<8)),
  SQLITE_IOERR_GETTEMPPATH        = (sqlite_error::SQLITE_IOERR | (25<<8)),
  SQLITE_IOERR_CONVPATH           = (sqlite_error::SQLITE_IOERR | (26<<8)),
  SQLITE_IOERR_VNODE              = (sqlite_error::SQLITE_IOERR | (27<<8)),
  SQLITE_IOERR_AUTH               = (sqlite_error::SQLITE_IOERR | (28<<8)),
  SQLITE_LOCKED_SHAREDCACHE       = (sqlite_error::SQLITE_LOCKED |  (1<<8)),
  SQLITE_BUSY_RECOVERY            = (sqlite_error::SQLITE_BUSY   |  (1<<8)),
  SQLITE_BUSY_SNAPSHOT            = (sqlite_error::SQLITE_BUSY   |  (2<<8)),
  SQLITE_CANTOPEN_NOTEMPDIR       = (sqlite_error::SQLITE_CANTOPEN | (1<<8)),
  SQLITE_CANTOPEN_ISDIR           = (sqlite_error::SQLITE_CANTOPEN | (2<<8)),
  SQLITE_CANTOPEN_FULLPATH        = (sqlite_error::SQLITE_CANTOPEN | (3<<8)),
  SQLITE_CANTOPEN_CONVPATH        = (sqlite_error::SQLITE_CANTOPEN | (4<<8)),
  SQLITE_CORRUPT_VTAB             = (sqlite_error::SQLITE_CORRUPT | (1<<8)),
  SQLITE_READONLY_RECOVERY        = (sqlite_error::SQLITE_READONLY | (1<<8)),
  SQLITE_READONLY_CANTLOCK        = (sqlite_error::SQLITE_READONLY | (2<<8)),
  SQLITE_READONLY_ROLLBACK        = (sqlite_error::SQLITE_READONLY | (3<<8)),
  SQLITE_READONLY_DBMOVED         = (sqlite_error::SQLITE_READONLY | (4<<8)),
  SQLITE_ABORT_ROLLBACK           = (sqlite_error::SQLITE_ABORT | (2<<8)),
  SQLITE_CONSTRAINT_CHECK         = (sqlite_error::SQLITE_CONSTRAINT | (1<<8)),
  SQLITE_CONSTRAINT_COMMITHOOK    = (sqlite_error::SQLITE_CONSTRAINT | (2<<8)),
  SQLITE_CONSTRAINT_FOREIGNKEY    = (sqlite_error::SQLITE_CONSTRAINT | (3<<8)),
  SQLITE_CONSTRAINT_FUNCTION      = (sqlite_error::SQLITE_CONSTRAINT | (4<<8)),
  SQLITE_CONSTRAINT_NOTNULL       = (sqlite_error::SQLITE_CONSTRAINT | (5<<8)),
  SQLITE_CONSTRAINT_PRIMARYKEY    = (sqlite_error::SQLITE_CONSTRAINT | (6<<8)),
  SQLITE_CONSTRAINT_TRIGGER       = (sqlite_error::SQLITE_CONSTRAINT | (7<<8)),
  SQLITE_CONSTRAINT_UNIQUE        = (sqlite_error::SQLITE_CONSTRAINT | (8<<8)),
  SQLITE_CONSTRAINT_VTAB          = (sqlite_error::SQLITE_CONSTRAINT | (9<<8)),
  SQLITE_CONSTRAINT_ROWID         = (sqlite_error::SQLITE_CONSTRAINT |(10<<8)),
  SQLITE_NOTICE_RECOVER_WAL       = (sqlite_error::SQLITE_NOTICE | (1<<8)),
  SQLITE_NOTICE_RECOVER_ROLLBACK  = (sqlite_error::SQLITE_NOTICE | (2<<8)),
  SQLITE_WARNING_AUTOINDEX        = (sqlite_error::SQLITE_WARNING | (1<<8)),
  SQLITE_AUTH_USER                = (sqlite_error::SQLITE_AUTH | (1<<8)),
  SQLITE_OK_LOAD_PERMANENTLY      = (sqlite_error::SQLITE_OK | (1<<8)),
};

struct sqlite_error_category : error_category {
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};

std::error_code make_error_code(sqlite_error e);
}


#endif //CACHE_PROXY_SQLITE_ERROR_CATEGORY_H
