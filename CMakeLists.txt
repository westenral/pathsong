cmake_minimum_required(VERSION 3.27)
project(pathsong)

set(CMAKE_CXX_STANDARD 23)

add_executable(pathsong src/main.cpp
        include/test.h
        src/songgraph.h
        src/song.h
        src/song.cpp
        src/songgraph.cpp)
