cmake_minimum_required(VERSION 3.8)

add_executable(variable variable.cpp)
target_link_libraries(variable boost_unit_test_framework_static)
add_test(
        NAME variable_test
        COMMAND $<TARGET_FILE:variable>
)
set_property(TEST variable_test PROPERTY LABELS variable)

add_executable(reference reference.cpp)
target_link_libraries(reference boost_unit_test_framework_static)
add_test(
        NAME reference_test
        COMMAND $<TARGET_FILE:reference>
)
set_property(TEST reference_test PROPERTY LABELS reference)

add_executable(lvalue left_hand_value.cpp)
target_link_libraries(lvalue boost_unit_test_framework_static)
add_test(
        NAME lvalue_test
        COMMAND $<TARGET_FILE:lvalue>
)
set_property(TEST lvalue_test PROPERTY LABELS lvalue)

add_executable(evaluation_context evaluation_context.cpp)
target_link_libraries(evaluation_context boost_unit_test_framework_static)
add_test(
        NAME evaluation_context_test
        COMMAND $<TARGET_FILE:evaluation_context>
)
set_property(TEST evaluation_context_test PROPERTY LABELS evaluation_context)
