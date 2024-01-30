cmake_minimum_required(VERSION 3.12)
project(libCATS)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

file(GLOB SOURCES lib/buffer.c
                lib/error.c
                lib/interleaver.c
                lib/ldpc_matrices.c
                lib/ldpc.c
                lib/packet.c
                lib/utf8.c
                lib/whisker.c
                lib/whitener.c
)
add_library(CATS STATIC ${SOURCES})
target_link_libraries(CATS ${CMAKE_SOURCE_DIR}/liblabrador_ldpc/liblabrador_ldpc.a) # Todo make this less stupid
target_include_directories(CATS PUBLIC include liblabrador_ldpc/include)
set_property(TARGET CATS PROPERTY C_STANDARD 11)