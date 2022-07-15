#include "nlohmann/json.hpp"

#include <string>

std::string make_response(std::string message, bool ok, nlohmann::json data = "{}"_json) {
  std::string result = "{\"ok\":";
  result += ok ? "true" : "false";
  result += ",\"message\":\"";
  result += message;
  result += "\",\"data\":";
  result += data.dump();
  result += "}";
  return result;
}

class StructToJson {
  public:
  virtual nlohmann::json toJson() = 0;
};

class HistoricalCase: public StructToJson {
  public:
  std::string date;
  int positive;
  int recovered;
  int deaths;
  int active;

  nlohmann::json toJson() {
    return {
      {"date", date},
      {"positive", positive},
      {"recovered", recovered},
      {"deaths", deaths},
      {"active", active}
    };
  }
};

class CurrentCases: public HistoricalCase {
  public:
  int total_positive;
  int total_recovered;
  int total_deaths;
  int total_active;

  nlohmann::json toJson() {
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
