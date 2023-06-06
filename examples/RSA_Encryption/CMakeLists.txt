cmake_minimum_required(VERSION 3.10)

project(RSA_Encryption VERSION 1.0.0 LANGUAGES CXX)

add_executable(RSA_Encryption "main.cpp")

target_link_libraries(RSA_Encryption PUBLIC lint)
target_include_directories(RSA_Encryption PUBLIC "${PROJECT_SOURCE_DIR}/lint/include")