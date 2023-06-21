cmake_minimum_required (VERSION 3.0)
project (type_utils)

set (CMAKE_CXX_STANDARD 23)

set(type_utils_includes
  ${type_utils_SOURCE_DIR}/include/
)

if (TARGET type_utils_example)
else()
add_executable(type_utils_example
  "${type_utils_SOURCE_DIR}/example.cpp"
)

target_include_directories(type_utils_example PRIVATE
  ${type_utils_includes}
)
endif()