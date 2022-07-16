#ifndef COVID_CLIENT_CLIENT
#define COVID_CLIENT_CLIENT

/**
 * Bunch of code where we can query data from
 */
#include "models.cpp"
#include "db.cpp"
#include "libasyik/service.hpp"
#include "libasyik/http.hpp"
#include "libasyik/sql.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <boost/fiber/future/future.hpp>

const std::string API_BASE = "https://data.covid19.go.id";
int refresh_time = std::stoi(env_or(getenv("REFRESH_TIME"), "7200"));

/**
 * Gets covid data from endpoint
 */
model::http_result get_covid_status_api(asyik::service_ptr as) {
  auto fiber = as->execute([as]() -> model::http_result {
    printf("Covid Query begin\n");
    model::http_result res;
    auto req = asyik::http_easy_request(
      as,
      "GET",
      API_BASE + "/public/api/update.json"
    );

    if (req->response.result() == 200) {
      // HTTP request successful
      res.success = true;
      res.result = nlohmann::json::parse((std::string) req->response.body);
    } else {
      // HTTP request fail
      res.success = false;
    }
    printf("Covid Query: %s\n", res.success ? "SUCCESS" : "FAIL");
    return res;
  });

  // Waits for the response and then return :D
  return fiber.get();
}

// TODO
boost::fibers::future<void> refresh_database(asyik::sql_session_ptr ses) {

}

// TODO: GMT+7
namespace data {
  /**
   * Check database for entries from today and yesterday.
   * If both does not exist, then refresh database and return.
   */
  std::string get_current_day(asyik::service_ptr as) {
    // Data from DB will be in these variables.
    // t_ prefix is total cases
    std::string case_date;
    int pos, rec, dth, act, t_pos, t_rec, t_dth, t_act;

    // Get current date and yesterdays date
    const std::string date = get_current_date();
    const std::string date_yesterday = get_current_date(-(24*60*60));

    std::string message;
    auto ses = get_db_sess(as);

    // First, we should see if today's data is here
    ses->query(R"(
        SELECT date, positive, recovered, deaths, active
        FROM historical_data
        WHERE date=:date;
      )",
      soci::use(date),
      soci::into(case_date),
      soci::into(pos), soci::into(rec), soci::into(dth), soci::into(act)
    );
    
    if (case_date.empty()) {
      // If data is not here, then lets first check when's the last time we
      // successfully fetched the data.
      int last_success_fetch;
      ses->query(R"(
        SELECT last_fetch FROM states;
      )", soci::into(last_success_fetch));

      if (time(0) > refresh_time + last_success_fetch ||
        get_date_from_time(last_success_fetch) < date) {
        // If it is time to refresh by refresh time, or we are on another date
        // Refetch from API
        refresh_database(ses).wait();
      }

      // Get the latest covid data from DB regardless if we refreshed or not.
      ses->query(R"(
          SELECT positive, recovered, deaths, active
          FROM historical_data
          ORDER BY date DESC
          LIMIT 1;
        )",
        soci::use(date),
        soci::into(pos), soci::into(rec), soci::into(dth), soci::into(act)
      );
    }

    // Get the cumulative total
    ses->query(R"(
      SELECT SUM(positive), SUM(recovered), SUM(deaths), SUM(active)
      FROM historical_data;
    )",
    soci::into(t_pos), soci::into(t_rec), soci::into(t_dth), soci::into(t_act));

    model::current_case cases;
    cases.positive = pos;
    cases.recovered = rec;
    cases.deaths = dth;
    cases.active = act;
    cases.total_positive = t_pos;
    cases.total_recovered = t_rec;
    cases.total_deaths = t_dth;
    cases.total_active = t_act;

    return model::make_response(true, cases.to_json(), message);
  }

  // std::string get_data_day(int day, int month, int year, asyik::service_ptr as) {

  // }

  // std::string get_data_month(int month, int year, asyik::service_ptr as) {

  // }

  // std::string get_data_year(int year, asyik::service_ptr as) {

  // }
}

#endif