#include "libasyik/http.hpp"
#include "nlohmann/json.hpp"

void register_routes(asyik::http_server_ptr<asyik::http_stream_type> server) {
  server->on_http_request("/ping", "GET", [](auto req, auto args){
    nlohmann::json j = R"(
      {
        "answer": "pong"
      }
    )"_json;

    req->response.body = j.dump();
    req->response.headers.set("Content-Type", "application/json");
    req->response.result(200);
  });
}