cmake_minimum_required(VERSION 3.8)
project(genetic_programming)

include_directories(./include)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(example1)

enable_testing()
add_library(
        boost_unit_test_framework_static
        STATIC
        IMPORTED
)
set_target_properties(
        boost_unit_test_framework_static
        PROPERTIES
        IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
)
add_subdirectory(test)
