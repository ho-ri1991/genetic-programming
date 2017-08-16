cmake_minimum_required(VERSION 3.8)
SET(CMAKE_CXX_COMPILER ~/llvm/build/bin/clang++)
project(genetic_programming)

set(CMAKE_CXX_FLAGS "-stdlib=libc++")
set(CMAKE_CXX_FLAGS "-nostdinc++")

include_directories(/usr/local/Cellar/boost/1.63.0/include)
include_directories(~/llvm/build/include/c++/v1)
include_directories(./include)
link_directories(~/llvm/build/lib)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp src/tree/tree_operations.cpp)
add_executable(genetic_programming ${SOURCE_FILES})