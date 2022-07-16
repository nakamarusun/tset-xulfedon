/**
 * Bunch of code where we can query data from
 */
#include "models.cpp"
#include "libasyik/service.hpp"
#include "libasyik/http.hpp"
#include "nlohmann/json.hpp"

#include <string>

const std::string API_BASE = "https://data.covid19.go.id";

/**
 * Gets covid data from endpoint
 */
model::http_result get_covid_status_api(asyik::service_ptr as) {
  auto fiber = as->execute([as]() -> model::http_result {
    model::http_result res;
    auto req = asyik::http_easy_request(
      as,
      "GET",
      API_BASE + "/public/api/update.json"
    );

    if (req->response.result() == 200) {
      // HTTP request successful
      res.success = true;
      res.success = nlohmann::json::parse(req->response.body);
    } else {
      // HTTP request fail
      res.success = false;
    }
    return res;
  });

  fiber.wait();
  return fiber.get();
}

// std::string get_data_day(int day, int month, int year, asyik::service_ptr as) {

// }

// std::string get_data_month(int month, int year, asyik::service_ptr as) {

// }

// std::string get_data_year(int year, asyik::service_ptr as) {

// }