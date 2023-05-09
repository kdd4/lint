cmake_minimum_required(VERSION 3.10)

project(lint VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(HEADERS "include/lint/lint.hpp" "include/lint/RSA.hpp")
set(SOURCES "src/lint.cpp" "src/RSA.cpp")

include_directories("include/")

add_library(lint ${SOURCES} ${HEADERS})
