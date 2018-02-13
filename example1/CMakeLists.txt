cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_FLAGS "-O2")

add_executable(example1 example1.cpp)
add_executable(tree_executor tree_executor.cpp)
add_executable(multi_thread_example multi_thread_example.cpp)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
  target_link_libraries(multi_thread_example pthread)
endif()

