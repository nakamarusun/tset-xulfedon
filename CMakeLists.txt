cmake_minimum_required(VERSION 3.14)
project(xulfedon_jasoncoding_covid_api)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} test.cpp) # add more source code here

#######
#include other dependencies
#######
find_package(libasyik)
if(libasyik_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${libasyik_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} libasyik)
endif()

find_package(Boost COMPONENTS context fiber REQUIRED)
if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} Boost::fiber Boost::context)
endif()