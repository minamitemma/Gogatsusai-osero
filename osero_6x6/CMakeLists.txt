cmake_minimum_required(VERSION 3.5)
project(reversi6 CXX)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic -Wconversion -Wcast-qual")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(reversi6
    main.cpp
    board.cpp
    option_parser.cpp
    player/human_player.cpp
    player/sample_computer_player.cpp
    player/minmax_player.cpp)

target_include_directories(reversi6 PRIVATE .)
