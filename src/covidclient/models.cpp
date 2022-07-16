#ifndef COVID_CLIENT_MODEL
#define COVID_CLIENT_MODEL

#include "nlohmann/json.hpp"

#include <string>

namespace model {
  /**
   * Helper function to create a HTTP response
   */
  std::string make_response(bool ok,
        nlohmann::json data="{}"_json,
        std::string message="") {

    std::string result = "{\"ok\":";
    result += ok ? "true" : "false";
    result += ",\"message\":\"";
    result += message;
    result += "\",\"data\":";
    result += data.dump();
    result += "}";
    return result;
  }

  /**
   * Helper struct used when we do a client HTTP request, and we want to return
   * result inspired by Go
   */
  struct http_result {
    bool success;
    nlohmann::json result;
  };

  /**
   * Base class so our class models can be converted to json
   */
  class struct_to_json {
    public:
    virtual nlohmann::json to_json() = 0;
  };

  /**
   * Historical case when we query for past data
   */
  class historical_case: public struct_to_json {
    public:
    std::string date;
    int positive;
    int recovered;
    int deaths;
    int active;

    nlohmann::json to_json() {
      return {
        {"date", date},
        {"positive", positive},
        {"recovered", recovered},
        {"deaths", deaths},
        {"active", active}
      };
    }
  };

  /**
   * Case when we query for the latest case
   */
  class current_case: public historical_case {
    public:
    int total_positive;
    int total_recovered;
    int total_deaths;
    int total_active;

    nlohmann::json to_json() {
      return {
        {"date", date},
        {"total_positive", total_positive},
        {"total_recovered", total_recovered},
        {"total_deaths", total_deaths},
        {"total_active", total_active},
        {"new_positive", positive},
        {"new_recovered", recovered},
        {"new_deaths", deaths},
        {"new_active", active}
      };
    }
  };
}
#endif