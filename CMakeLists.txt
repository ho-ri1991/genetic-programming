cmake_minimum_required(VERSION 3.8)
SET(CMAKE_CXX_COMPILER ~/llvm/build/bin/clang++)
project(genetic_programming)

set(CMAKE_CXX_FLAGS "-stdlib=libc++")
set(CMAKE_CXX_FLAGS "-nostdinc++")

include_directories(/usr/local/Cellar/boost/1.65.0/include)
include_directories(~/llvm/build/include/c++/v1)
include_directories(./include)
link_directories(~/llvm/build/lib)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp)
add_executable(genetic_programming ${SOURCE_FILES})

add_executable(example1 src/example1.cpp)

enable_testing()
add_library(
        boost_unit_test_framework_static
        STATIC
        IMPORTED
)
set_target_properties(
        boost_unit_test_framework_static
        PROPERTIES
        IMPORTED_LOCATION /usr/local/lib/libboost_unit_test_framework.a
)
add_subdirectory(test)
