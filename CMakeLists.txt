cmake_minimum_required(VERSION 3.12)
project(libCATS)

file(GLOB SOURCES "src/*.c")

add_executable(catstest ${SOURCES})
