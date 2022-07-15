#include "libasyik/service.hpp"
#include "libasyik/http.hpp"
#include "db.cpp"
#include "miscutils.cpp"
#include "routes/routes.cpp"

#include <stdio.h>
#include <string>

int main() {
  const char* host = env_or(getenv("HOST"), "127.0.0.1");
  int port = std::stoi(env_or(getenv("PORT"), "8080"));

  auto as = asyik::make_service();
  auto server = asyik::make_http_server(as, host, port);
  printf("Starting server on %s:%d\n", host, port);

  // Initialize DB
  init_db();
  register_routes(server);

  as->run();

  return 0;
}