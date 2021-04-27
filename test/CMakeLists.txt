cmake_minimum_required(VERSION 3.17)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.12.1
)
FetchContent_MakeAvailable(Catch2)

#find_package(Catch2 REQUIRED)

add_executable(tests tests.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(tests Catch2::Catch2)

add_test(
  NAME all-tests
  COMMAND tests)
