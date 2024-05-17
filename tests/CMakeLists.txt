cmake_minimum_required(VERSION 3.12)
project(libCATS)

# Build tests
add_executable(test-interleaver interleaver.c)
target_include_directories(test-interleaver PUBLIC include)
target_link_libraries(test-interleaver PUBLIC CATS)

add_executable(test-ldpc ldpc.c)
target_include_directories(test-ldpc PUBLIC include)
target_link_libraries(test-ldpc PUBLIC CATS)

add_executable(test-packet packet.c)
target_include_directories(test-packet PUBLIC include)
target_link_libraries(test-packet PUBLIC CATS)

add_executable(test-whisker whisker.c)
target_include_directories(test-whisker PUBLIC include)
target_link_libraries(test-whisker PUBLIC CATS)

add_executable(test-whitener whitener.c)
target_include_directories(test-whitener PUBLIC include)
target_link_libraries(test-whitener PUBLIC CATS)

add_executable(test-cats cats.c)
target_include_directories(test-cats PUBLIC include)
target_link_libraries(test-cats PUBLIC CATS)

if(OPT_BUILD_RADIO_IFACE)
    add_executable(radio_iface radio_iface.c)
    target_include_directories(radio_iface PUBLIC include)
    target_link_libraries(radio_iface PUBLIC CATS)
endif(OPT_BUILD_RADIO_IFACE)

# Run tests
add_test(NAME Interleaver
         COMMAND test-interleaver)
set_property(TEST Interleaver PROPERTY WILL_FAIL Interleaver)

add_test(NAME LDPC
         COMMAND test-ldpc)
set_property(TEST LDPC PROPERTY WILL_FAIL LDPC)

add_test(NAME Packet
         COMMAND test-packet)
set_property(TEST Packet PROPERTY WILL_FAIL Packet)

add_test(NAME Whisker
         COMMAND test-whisker)
set_property(TEST Whisker PROPERTY WILL_FAIL Whisker)

add_test(NAME Whitener
         COMMAND test-whitener)
set_property(TEST Whitener PROPERTY WILL_FAIL Whitener)

add_test(NAME CATS
         COMMAND cats)
set_property(TEST CATS PROPERTY WILL_FAIL CATS)

if(OPT_BUILD_RADIO_IFACE)
    add_test(NAME RadioInterface
        COMMAND radio_iface)
    set_property(TEST RadioInterface PROPERTY WILL_FAIL RadioInterface)
endif(OPT_BUILD_RADIO_IFACE)
