cmake_minimum_required (VERSION 3.0)
project (type_utils)

set (CMAKE_CXX_STANDARD 23)

set(type_utils_includes
  ${type_utils_SOURCE_DIR}/include/
)

option(BUILD_TYPE_UTILS_EXAMPLE "Build the type utils example project." false)

if (${BUILD_TYPE_UTILS_EXAMPLE})
if (TARGET type_utils_example)
else()
add_executable(type_utils_example
  "${type_utils_SOURCE_DIR}/example.cpp"
)

target_include_directories(type_utils_example PRIVATE
  ${type_utils_includes}
)
endif()
endif()