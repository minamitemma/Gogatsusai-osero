cmake_minimum_required(VERSION 2.8)
project(reversi CXX)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic -Wconversion -Wcast-qual")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

add_executable(reversi
    main.cpp
    board.cpp
    player/human_player.cpp
    player/sample_computer_player.cpp
    option_parser.cpp)

target_include_directories(reversi PRIVATE .)
