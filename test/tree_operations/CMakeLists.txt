cmake_minimum_required(VERSION 3.8)

add_executable(tree_operations tree_operations.cpp)
target_link_libraries(tree_operations boost_unit_test_framework_static)
add_test(
        NAME tree_operations_test
        COMMAND $<TARGET_FILE:tree_operations>
)
set_property(TEST tree_operations_test PROPERTY LABELS tree_operations)
