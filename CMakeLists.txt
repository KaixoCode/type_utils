cmake_minimum_required (VERSION 3.0)
project (type_utils)

set (CMAKE_CXX_STANDARD 23)

option(BUILD_TYPE_UTILS_EXAMPLE "Build the type utils example project." false)
option(BUILD_TYPE_UTILS_TESTS "Build the type utils tests project." false)

if (TARGET type_utils)
else()
file(GLOB_RECURSE SOURCE
  "${type_utils_SOURCE_DIR}/include/*.hpp"
)

add_library(type_utils ${SOURCE})
set_target_properties(type_utils PROPERTIES LINKER_LANGUAGE CXX)
endif()

if (${BUILD_TYPE_UTILS_EXAMPLE})
if (TARGET type_utils_example)
else()
add_executable(type_utils_example
  "${type_utils_SOURCE_DIR}/example.cpp"
)

target_include_directories(type_utils_example PRIVATE
  ${type_utils_SOURCE_DIR}/include/
)
endif()
endif()

if (${BUILD_TYPE_UTILS_TESTS})
if (TARGET type_utils_tests)
else()
file(GLOB_RECURSE TESTS_SOURCE
  "${type_utils_SOURCE_DIR}/tests/*.cpp"
  "${type_utils_SOURCE_DIR}/tests/*.hpp"
)

add_executable(type_utils_tests
  ${TESTS_SOURCE}
)

target_include_directories(type_utils_tests PRIVATE
  ${type_utils_SOURCE_DIR}/include/
  ${type_utils_SOURCE_DIR}/tests/
)
endif()
endif()