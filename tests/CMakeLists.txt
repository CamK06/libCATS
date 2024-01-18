cmake_minimum_required(VERSION 3.12)
project(libCATS)

# Build tests
add_executable(interleaver interleaver.c)
target_include_directories(interleaver PUBLIC include)
target_link_libraries(interleaver PUBLIC CATS)

add_executable(ldpc ldpc.c)
target_include_directories(ldpc PUBLIC include)
target_link_libraries(ldpc PUBLIC CATS)

add_executable(packet packet.c)
target_include_directories(packet PUBLIC include)
target_link_libraries(packet PUBLIC CATS)

add_executable(whisker whisker.c)
target_include_directories(whisker PUBLIC include)
target_link_libraries(whisker PUBLIC CATS)

add_executable(whitener whitener.c)
target_include_directories(whitener PUBLIC include)
target_link_libraries(whitener PUBLIC CATS)

add_executable(cats cats.c)
target_include_directories(cats PUBLIC include)
target_link_libraries(cats PUBLIC CATS)

# Run tests
add_test(NAME Interleaver
         COMMAND interleaver)
set_property(TEST Interleaver PROPERTY WILL_FAIL Interleaver)

add_test(NAME LDPC
         COMMAND ldpc)
set_property(TEST LDPC PROPERTY WILL_FAIL LDPC)

add_test(NAME Packet
         COMMAND packet)
set_property(TEST Packet PROPERTY WILL_FAIL Packet)

add_test(NAME Whisker
         COMMAND whisker)
set_property(TEST Whisker PROPERTY WILL_FAIL Whisker)

add_test(NAME Whitener
         COMMAND whitener)
set_property(TEST Whitener PROPERTY WILL_FAIL Whitener)

add_test(NAME CATS
         COMMAND cats)
set_property(TEST CATS PROPERTY WILL_FAIL CATS)