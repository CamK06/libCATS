cmake_minimum_required(VERSION 3.12)
project(CATS)

file(GLOB SOURCES "src/*.c")

add_executable(catstest ${SOURCES})
