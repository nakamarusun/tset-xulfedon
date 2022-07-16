cmake_minimum_required(VERSION 3.14)
project(covid_tuils)

add_library(${PROJECT_NAME} miscutils.cpp)
add_library(covid::utils ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR})

# Json
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
