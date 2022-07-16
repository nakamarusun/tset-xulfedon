#ifndef MISC_UTIL_DB
#define MISC_UTIL_DB

#include "libasyik/service.hpp"
#include "libasyik/sql.hpp"

#include <stdio.h>
#include "miscutils.cpp"

asyik::sql_pool_ptr __pool;

void init_db(asyik::service_ptr as, const char* db_location) {
  bool db_exists = file_exists(db_location);
  __pool = asyik::make_sql_pool(asyik::sql_backend_sqlite3, db_location, 1);
  
  // If the DB didn't exist initially, Initialize some table innit
  if (!db_exists) {
    auto ses = __pool->get_session(as);
    // Table for states that we should keep track of
    ses->query("CREATE TABLE IF NOT EXISTS states (key TEXT, value TEXT);");
    ses->query(R"(
      INSERT INTO states VALUES
        ("last_fetch", "0");
    )");
    // Covid data table
    ses->query(R"(CREATE TABLE IF NOT EXISTS historical_data (
      date TEXT NOT NULL PRIMARY KEY,
      positive INT,
      recovered INT,
      deaths INT,
      active INT);
    )");
    ses->commit();
  }
}

asyik::sql_session_ptr get_db_sess(asyik::service_ptr as) {
  return __pool->get_session(as);
}
#endif