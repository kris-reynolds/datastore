cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
  DataStore
  LANGUAGES CXX
  VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_EXAMPLES "OFF to disable example compilation" OFF)
option(ENABLE_TESTS "OFF to disable test compilation" OFF)

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
