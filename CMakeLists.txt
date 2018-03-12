cmake_minimum_required(VERSION 3.9)
project(cache_proxy VERSION 1.0.0)

set(CACHE_PROXY_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CACHE_PROXY_DIR ${CACHE_PROXY_SRC_DIR}/cacheproxy)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/src/third-party)
set(SQLITE3CPP_DIR ${THIRD_PARTY_DIR}/sqlite3cpp)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)

include_directories(./src)
include_directories(./src/third-party/boost_1_66_0)
include_directories(./src/third-party/sqlite3cpp/sqlite3)
include_directories(./src/third-party/sqlite3cpp/include)

configure_file("${CACHE_PROXY_DIR}/config.h.in" "${CACHE_PROXY_DIR}/config.h")

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions(-DSQLITE_ENABLE_COLUMN_METADATA)


add_executable(client ${TEST_DIR}/client.cc src/cacheproxy/cache/http_cache_manager.h)
add_executable(server ${TEST_DIR}/server.cc src/cacheproxy/cache/http_cache_manager.h)

set(SQLITE3CPP_SRC_DIR ${THIRD_PARTY_DIR}/sqlite3cpp/src)
set(SQLITE3CPP_SRCS ${SQLITE3CPP_SRC_DIR}/Backup.cpp ${SQLITE3CPP_SRC_DIR}/Column.cpp ${SQLITE3CPP_SRC_DIR}/Database.cpp ${SQLITE3CPP_SRC_DIR}/Exception.cpp ${SQLITE3CPP_SRC_DIR}/Statement.cpp ${SQLITE3CPP_SRC_DIR}/Transaction.cpp src/cacheproxy/cache/vary_io.h src/cacheproxy/http/header.h)

add_executable(sqlite ${TEST_DIR}/sqlite.cc ${SQLITE3CPP_DIR}/sqlite3/sqlite3.c ${SQLITE3CPP_SRCS})
add_executable(regex ${TEST_DIR}/regex.cc src/cacheproxy/http/regex.cc src/cacheproxy/http/regex.h)

add_executable(header ${TEST_DIR}/header.cc src/cacheproxy/http/header.h src/cacheproxy/http/header.cc src/cacheproxy/http/regex.cc src/cacheproxy/utils/string.cc src/cacheproxy/utils/string.h)
#target_compile_definitions(header PRIVATE CATCH_CONFIG_MAIN)

add_executable(database ${TEST_DIR}/database.cc src/cacheproxy/metadb/metadb.cc src/cacheproxy/metadb/metadb.h ${SQLITE3CPP_DIR}/sqlite3/sqlite3.c ${SQLITE3CPP_SRCS})
