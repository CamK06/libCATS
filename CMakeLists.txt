cmake_minimum_required(VERSION 3.12)
project(libCATS)

option(OPT_BUILD_CPP_WRAPPER ON)

#set(CMAKE_C_FLAGS_RELEASE "-O2")

file(GLOB SOURCES lib/buffer.c
                lib/error.c
                lib/interleaver.c
                lib/ldpc_matrices.c
                lib/ldpc.c
                lib/packet.c
                lib/utf8.c
                lib/whisker.c
                lib/whitener.c
                lib/util.c
                lib/whiskers/destination.c
                lib/whiskers/gps.c
                lib/whiskers/identification.c
                lib/whiskers/nodeinfo.c
                lib/whiskers/repeater.c
                lib/whiskers/route.c
                lib/whiskers/simplex.c
                lib/whiskers/comment.c
                lib/whiskers/arbitrary.c
                lib/whiskers/timestamp.c
)
add_library(CATS STATIC ${SOURCES})
target_link_libraries(CATS ${CMAKE_SOURCE_DIR}/liblabrador_ldpc/liblabrador_ldpc.a) # Todo make this less stupid
target_include_directories(CATS PUBLIC include liblabrador_ldpc/include)
set_property(TARGET CATS PROPERTY C_STANDARD 11)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

if(OPT_BUILD_CPP_WRAPPER)
	add_subdirectory(cpp-wrapper)
endif(OPT_BUILD_CPP_WRAPPER)
