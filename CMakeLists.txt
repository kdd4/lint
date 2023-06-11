cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-Og -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wpedantic")

option(WARNINGS_AS_ERRORS "Warmings cause errors" OFF)
if(WARNINGS_AS_ERRORS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

project(lint VERSION 1.0.0 LANGUAGES CXX)

set(HEADERS "include/lint/lint.hpp" "include/lint/RSA.hpp")
set(SOURCES "src/lint.cpp" "src/RSA.cpp")

include_directories("include/")

add_library(lint STATIC ${SOURCES} ${HEADERS})

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
