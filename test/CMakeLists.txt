cmake_minimum_required(VERSION 3.8)

add_executable(manual_construction_and_write manual_construction_and_write.cpp)
add_test(
        NAME manual_construction_and_write_test
        COMMAND $<TARGET_FILE:manual_construction_and_write>
)

set_property(TEST manual_construction_and_write_test PROPERTY LABELS manual_construction_and_write)

add_subdirectory(utility)
