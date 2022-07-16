#include "libasyik/http.hpp"
#include "nlohmann/json.hpp"
#include "miscutils.cpp"
#include "client.cpp"
#include "libasyik/service.hpp"

#include <stdio.h>

asyik::service_ptr __as;

void ping(asyik::http_request_ptr req, asyik::http_route_args args) {
  // Deserialize raw string to json object
  nlohmann::json j = R"(
    {
      "answer": "pong"
    }
  )"_json;

  // Dumps json to string
  req->response.body = j.dump();
  req->response.headers.set("Content-Type", "application/json");
  req->response.result(200);
}

void register_routes(asyik::service_ptr as,
  asyik::http_server_ptr<asyik::http_stream_type> server) {
  __as = as;

  server->on_http_request("/ping", "GET", &ping);
}