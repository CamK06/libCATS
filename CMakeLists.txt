cmake_minimum_required(VERSION 3.19)
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

include(FetchContent)
FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.4
)
FetchContent_MakeAvailable(Corrosion)
corrosion_import_crate(MANIFEST_PATH ${CMAKE_SOURCE_DIR}/labrador-ldpc/capi/Cargo.toml NO_STD PROFILE release)

target_link_libraries(CATS labrador_ldpc)
target_include_directories(CATS PUBLIC include labrador-ldpc/capi/include)

set_property(TARGET CATS PROPERTY C_STANDARD 11)