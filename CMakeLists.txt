cmake_minimum_required(VERSION 3.6)
project(numerical)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES biginteger.h rational.h main.cpp)
add_executable(numerical ${SOURCE_FILES})