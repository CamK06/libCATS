cmake_minimum_required(VERSION 3.12)
project(libCATS)

file(GLOB SOURCES test.c "src/*.c")

add_executable(catstest ${SOURCES})
target_include_directories(catstest PUBLIC include)
