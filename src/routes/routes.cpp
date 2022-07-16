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

void responser(asyik::http_request_ptr req, data::query_result res, int error_code) {
  if (res.success) {
    req->response.body = res.result;
    req->response.headers.set("Content-Type", "application/json");
    req->response.result(200);
  } else {
    req->response.body = res.reason;
    req->response.result(error_code);
  }
}

/**
 * Route current covid cases
 */
void current_covid(asyik::http_request_ptr req, asyik::http_route_args args) {
  responser(req, data::get_current_day(__as), 500);
}

void covid_date(asyik::http_request_ptr req, asyik::http_route_args args) {
  responser(req, data::get_data_day(
    __as,
    stoi(args[3]),
    stoi(args[2]),
    stoi(args[1])
  ), 400);
}

void covid_month(asyik::http_request_ptr req, asyik::http_route_args args) {
  responser(req, data::get_data_month(
    __as,
    stoi(args[2]),
    stoi(args[1])
  ), 400);
}

void covid_year(asyik::http_request_ptr req, asyik::http_route_args args) {
  responser(req, data::get_data_year(__as, stoi(args[1])), 400);
}

void register_routes(asyik::service_ptr as,
  asyik::http_server_ptr<asyik::http_stream_type> server) {
  __as = as;

  server->on_http_request("/ping", "GET", &ping);
  server->on_http_request("/", "GET", &current_covid);
  server->on_http_request("/daily/<string>/<string>/<string>", "GET", &covid_date);
  server->on_http_request("/monthly/<string>/<string>", "GET", &covid_month);
  server->on_http_request("/yearly/<string>", "GET", &covid_year);
}