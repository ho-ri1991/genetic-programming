cmake_minimum_required(VERSION 3.8)
SET(CMAKE_CXX_COMPILER /usr/local/bin/g++-7)
project(genetic_programming)

include_directories(/usr/local/Cellar/boost/1.63.0/include)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)
add_executable(genetic_programming ${SOURCE_FILES})