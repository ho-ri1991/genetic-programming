cmake_minimum_required(VERSION 3.8)

add_executable(string_to_node string_to_node.cpp)
target_link_libraries(string_to_node boost_unit_test_framework_static)
add_test(
        NAME string_to_node_test
        COMMAND $<TARGET_FILE:string_to_node>
)
set_property(TEST string_to_node_test PROPERTY LABELS string_to_node)
