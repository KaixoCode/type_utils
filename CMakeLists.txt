cmake_minimum_required (VERSION 3.0)
project (type_utils)

set (CMAKE_CXX_STANDARD 23)

add_library(type_utils STATIC
  "${type_utils_SOURCE_DIR}/include/impl/string_literal.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/type_traits.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/qualifiers.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/functions.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/concepts.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/helpers.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/members.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/struct.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/switch.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/tuple.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/pack.hpp"
  "${type_utils_SOURCE_DIR}/include/impl/info.hpp"
  "${type_utils_SOURCE_DIR}/include/type_utils.hpp"
)

set_target_properties(type_utils PROPERTIES LINKER_LANGUAGE CXX)

add_executable(type_utils_example
  "${type_utils_SOURCE_DIR}/example.cpp"
)