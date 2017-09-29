cmake_minimum_required(VERSION 3.8)

add_executable(tree tree.cpp)
target_link_libraries(tree boost_unit_test_framework_static)
add_test(
        NAME tree_test
        COMMAND $<TARGET_FILE:tree>
)
set_property(TEST tree_test PROPERTY LABELS tree)

add_executable(io io.cpp)
target_link_libraries(io boost_unit_test_framework_static)
add_test(
        NAME io_test
        COMMAND $<TARGET_FILE:io>
)
set_property(TEST io_test PROPERTY LABELS io)
