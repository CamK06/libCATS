cmake_minimum_required(VERSION 3.12)
project(libCATS)

# Tests
add_executable(test-cpp-wrapper test.cpp)
target_include_directories(test-cpp-wrapper PUBLIC include)
target_link_libraries(test-cpp-wrapper PUBLIC CATS)

add_test(NAME C++-Wrapper
	 COMMAND test-cpp-wrapper)
 set_property(TEST C++-Wrapper PROPERTY WILL_FAIL C++-Wrapper)
