cmake_minimum_required(VERSION 3.8)

add_executable(variable variable.cpp)
add_test(
        NAME variable_test
        COMMAND $<TARGET_FILE:variable>
)

set_property(TEST variable_test PROPERTY LABELS variable)
