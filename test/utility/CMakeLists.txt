cmake_minimum_required(VERSION 3.8)

add_executable(variable variable.cpp)
target_link_libraries(variable boost_unit_test_framework_static)
add_test(
        NAME variable_test
        COMMAND $<TARGET_FILE:variable>
)

set_property(TEST variable_test PROPERTY LABELS variable)
