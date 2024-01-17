cmake_minimum_required(VERSION 3.12)
project(libCATS)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

file(GLOB SOURCES "src/*.c")
add_library(CATS STATIC ${SOURCES})
target_include_directories(CATS PUBLIC include)