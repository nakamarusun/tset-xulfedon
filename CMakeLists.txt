cmake_minimum_required(VERSION 3.14)
project(xulfedon_jasoncoding_covid_api)
set(CMAKE_CXX_STANDARD 17)

# Very important to be able to build
add_compile_options(-g -O0 -fprofile-arcs -ftest-coverage)
add_link_options(-lgcov --coverage)

add_subdirectory(src)