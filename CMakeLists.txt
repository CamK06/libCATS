cmake_minimum_required(VERSION 3.19)
project(libCATS)

option(OPT_BUILD_CPP_WRAPPER ON)
option(OPT_BUILD_TESTS "Build Tests" OFF)
option(OPT_BUILD_RADIO_IFACE "Build PC<->Radio interface functions" ON)

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
                lib/radio_iface.c
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

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

if(OPT_BUILD_CPP_WRAPPER)
	add_subdirectory(cpp-wrapper)
endif(OPT_BUILD_CPP_WRAPPER)

include(FetchContent)
FetchContent_Declare(
    labrador_ldpc
    GIT_REPOSITORY https://github.com/adamgreig/labrador-ldpc.git
    GIT_TAG v1.2.1
)
FetchContent_MakeAvailable(labrador_ldpc)

FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.4
)
FetchContent_MakeAvailable(Corrosion)
corrosion_import_crate(MANIFEST_PATH ${labrador_ldpc_SOURCE_DIR}/capi/Cargo.toml NO_STD PROFILE release)

if(OPT_BUILD_RADIO_IFACE)
    FetchContent_Declare(cbor
                          GIT_REPOSITORY https://github.com/libmcu/cbor.git
                          GIT_TAG main
    )
    FetchContent_MakeAvailable(cbor)
    target_link_libraries(CATS cbor)
    add_definitions(-DBUILD_RADIO_IFACE)
endif(OPT_BUILD_RADIO_IFACE)

if(OPT_BUILD_TESTS)
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        include(CTest)        
        add_subdirectory(tests)
    endif()
endif(OPT_BUILD_TESTS)

target_link_libraries(CATS labrador_ldpc)
target_include_directories(CATS PUBLIC include ${labrador_ldpc_SOURCE_DIR}/capi/include)
set_property(TARGET CATS PROPERTY C_STANDARD 11)