cmake_minimum_required(VERSION 3.8)

add_executable(crossover crossover.cpp)
target_link_libraries(crossover boost_unit_test_framework_static)
add_test(
        NAME crossover_test
        COMMAND $<TARGET_FILE:crossover>
)
set_property(TEST crossover_test PROPERTY LABELS crossover)
