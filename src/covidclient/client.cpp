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
    printf("Covid API Hit\n");

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

boost::fibers::future<bool> refresh_database(asyik::service_ptr as, 
  asyik::sql_session_ptr ses) {
  auto fiber = as->execute([as, ses]() -> bool {
    // Fetch data

    model::http_result data = get_covid_status_api(as);

    if (!data.success) return false;

    // Harian json
    nlohmann::json harian = data.result["update"]["harian"];

    // Build the query
    std::string query_builder = R"(
      INSERT INTO historical_data(date, positive, recovered, deaths, active)
      VALUES 
    )";

    // Add inserts
    for (nlohmann::json::iterator it = harian.begin(); it != harian.end(); ++it) {
      auto j2 = *it;
      // Get only the date part
      const char* date = j2["key_as_string"].get<std::string>().substr(0, 10).c_str();
      int pos = j2["jumlah_positif"]["value"].get<int>();
      int rec = j2["jumlah_sembuh"]["value"].get<int>();
      int dth = j2["jumlah_meninggal"]["value"].get<int>();
      int act = j2["jumlah_dirawat"]["value"].get<int>();

      char buf[100];
      int n = sprintf(buf, "(\"%s\", %d, %d, %d, %d),", date, pos, rec, dth, act);
      // If this is the last element, make the last comma disappear
      if (harian.end() - it == 1) {
        std::string temp = buf;
        query_builder += temp.substr(0, n-1);
      } else {
        query_builder += buf;
      }
    }

    // Add the rest of the query
    query_builder += R"(
      ON CONFLICT(date) DO UPDATE SET
      positive=excluded.positive,
      recovered=excluded.recovered,
      deaths=excluded.deaths,
      active=excluded.active;
    )";

    // Insert to DB
    asyik::sql_transaction tr(ses);
    ses->query(query_builder);

    std::string time_done = std::to_string(time(0));
    ses->query(R"(
        UPDATE states SET value=:datee WHERE key="last_fetch";
      )",
      soci::use(time_done, "datee")
    );
    tr.commit();
    return true;
  });

  return fiber;
}

namespace data {
  class query_result {
    public:
    std::string result;
    bool success;
    std::string reason;

    query_result(): success(true) {}
  };

  /**
   * Check database for entries from today and yesterday.
   * If both does not exist, then refresh database and return.
   */
  query_result get_current_day(asyik::service_ptr as) {
    // Data from DB will be in these variables.
    // t_ prefix is total cases
    query_result result;
    std::string case_date;
    std::string new_date;
    int pos, rec, dth, act, t_pos, t_rec, t_dth, t_act;

    // Get current date and yesterdays date
    const std::string date = get_current_date();
    const std::string date_yesterday = get_current_date(-(24*60*60));

    std::string message = "latest data";
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
        SELECT value FROM states WHERE key="last_fetch";
      )", soci::into(last_success_fetch));

      message = "old data api haven not updated";
      if (time(0) > refresh_time + last_success_fetch ||
        get_date_from_time(last_success_fetch) < date) {

        // If it is time to refresh by refresh time, or we are on another date
        // Refetch from API
        if (refresh_database(as, ses).get()) {
          message = "data refetched";
        } else {
          result.success = false;
          result.reason = "error refreshing database";
        };
      }
      // Get the latest covid data from DB regardless if we refreshed or not.
      ses->query(R"(
          SELECT date, positive, recovered, deaths, active
          FROM historical_data
          ORDER BY date DESC
          LIMIT 1;
        )",
        soci::into(new_date),
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
    if (new_date.empty()) {
      cases.date = date;
    } else {
      cases.date = new_date;
    }
    cases.positive = pos;
    cases.recovered = rec;
    cases.deaths = dth;
    cases.active = act;
    cases.total_positive = t_pos;
    cases.total_recovered = t_rec;
    cases.total_deaths = t_dth;
    cases.total_active = t_act;

    result.result = model::make_response(true, cases.to_json(), message);
    return result;
  }

  query_result get_data_day(asyik::service_ptr as, int day, int month, int year) {
    std::string date;
    std::string new_date;
    std::string message;
    query_result result;
    int pos, rec, dth, act;
    auto ses = get_db_sess(as);

    char query_date[15];
    sprintf(query_date, "%04d-%02d-%02d", year, month, day);
    std::string query_date_str = std::string(query_date);

    // Get the date of the event
    ses->query(R"(
        SELECT date, positive, recovered, deaths, active
        FROM historical_data
        WHERE date=:dater;
      )", soci::use(query_date_str),
      soci::into(date),
      soci::into(pos), soci::into(rec), soci::into(dth), soci::into(act)
    );

    if (date.empty()) {
      if (refresh_database(as, ses).get()) {
        message = "retrieved data";
          ses->query(R"(
          SELECT date, positive, recovered, deaths, active
          FROM historical_data
          WHERE date=:daters;
        )", soci::use(query_date_str),
        soci::into(new_date),
        soci::into(pos), soci::into(rec), soci::into(dth), soci::into(act)
      );
      } else {
        result.success = false;
        result.reason = "error refreshing data";
      }
    } else {
      message = "cached data";
    }

    model::historical_case cases;

    // If still empty
    if (new_date.empty()) {
      result.success = false;
      result.reason = "unknown error";
    } else {
      if (new_date.empty()) {
        cases.date = date;
      } else {
        cases.date = new_date;
      }
      cases.positive = pos;
      cases.recovered = rec;
      cases.deaths = dth;
      cases.active = act;

      result.result = model::make_response(true, cases.to_json(), message);
    }

    return result;
  }

  query_result get_data_month(asyik::service_ptr as, int month, int year) {
    query_result result;
    auto ses = get_db_sess(as);

    return result;
  }

  query_result get_data_year(asyik::service_ptr as, int year) {
    query_result result;
    auto ses = get_db_sess(as);

    return result;
  }
}

#endif