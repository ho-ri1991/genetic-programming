cmake_minimum_required(VERSION 3.8)

add_executable(problem problem.cpp)
target_link_libraries(problem boost_unit_test_framework_static)
add_test(
        NAME problem_test
        COMMAND $<TARGET_FILE:problem>
)
set_property(TEST problem_test PROPERTY LABELS problem)
