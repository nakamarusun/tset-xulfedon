#ifndef MISC_UTILS_COVID
#define MISC_UTILS_COVID

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string>

/**
 * Like null coalescing in javascript.
 * 
 * If the first value is not null, it is returned. Otherwise, the second
 * value is returned.
 */
template<typename T>
T null_coalesce(T value, T value_if_null) {
  return value == NULL ? value_if_null : value;
}

const char* env_or(char* env, const char* if_null) {
  return env == NULL ? if_null : env; 
}

bool file_exists(const char* name) {
  struct stat stat_buf;
  return stat(name, &stat_buf) == 0;
}

// Thanks to TrungTN and Rashad
// https://stackoverflow.com/a/10467633/12709867

// If offset is specified, will offset the time by seconds.
const std::string get_current_date(int offset = 0) {
  time_t date = time(0) + offset;
  return get_date_from_time(date);
}

const std::string get_date_from_time(time_t time) {
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&time);

  strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
  return buf;
}

#endif