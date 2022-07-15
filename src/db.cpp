#include "libasyik/sql.hpp"

#include <stdio.h>

void init_db() {
  auto pool = asyik::make_sql_pool(asyik::sql_backend_sqlite3, "covid.db", 1);
}