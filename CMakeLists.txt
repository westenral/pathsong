cmake_minimum_required(VERSION 3.27)
project(pathsong)

set(CMAKE_CXX_STANDARD 23)

add_executable(pathsong src/main.cpp
        include/test.h
        include/songgraph.h
        include/song.h
        src/song.cpp
        src/songgraph.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:SFML/include/SFML)
target_link_libraries(pathsong sfml-system sfml-window sfml-graphics sfml-audio)



