#include "libasyik/service.hpp"
#include "libasyik/http.hpp"
#include "db.cpp"
#include "miscutils.cpp"
#include "routes/routes.cpp"

#include <stdio.h>
#include <string>

int main() {
  // Load environment variables
  const char* host = env_or(getenv("HOST"), "127.0.0.1");
  const char* db_location = env_or(getenv("DB_LOC"), "/var/covid_ebin.db");
  int port = std::stoi(env_or(getenv("PORT"), "8080"));
  int refresh_time = std::stoi(env_or(getenv("REFRESH_TIME"), "7200"));

  // Configures asyik stuffs
  auto as = asyik::make_service();
  auto server = asyik::make_http_server(as, host, port);
  printf("Starting server on %s:%d\n", host, port);

  // Initialize DB
  init_db(as, db_location);
  register_routes(as, server);

  printf("Asyik service start\n");
  as->run(); // So this causes the program to be stuck here
  printf("Asyik service stopped\n");

  return 0;
}