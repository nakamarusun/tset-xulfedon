#include "libasyik/sql.hpp"
#include "libasyik/service.hpp"

#include <stdio.h>
#include "miscutils.cpp"

void init_db(asyik::service_ptr as, const char* db_location) {
  bool db_exists = file_exists(db_location);
  auto pool = asyik::make_sql_pool(asyik::sql_backend_sqlite3, db_location, 1);
  
  // If the DB didn't exist initially, Initialize some table innit
  if (!db_exists) {
    auto ses = pool->get_session(as);
    ses->query("CREATE TABLE IF NOT EXISTS query_time (lastquery TEXT);");
    ses->query(R"(CREATE TABLE IF NOT EXISTS historical_data (
      date TEXT,
      positive INT,
      recovered INT,
      deaths INT,
      active INT);
    )");
    ses->commit();
  }
}