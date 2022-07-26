cmake_minimum_required (VERSION 3.0)
project (type_utils)

set (CMAKE_CXX_STANDARD 23)

add_library(type_utils STATIC
  "${type_utils_SOURCE_DIR}/type_utils.hpp"
)

set_target_properties(type_utils PROPERTIES LINKER_LANGUAGE CXX)

add_executable(type_utils_example
  "${type_utils_SOURCE_DIR}/example.cpp"
)