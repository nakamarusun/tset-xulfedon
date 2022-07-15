#include <stdlib.h>

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