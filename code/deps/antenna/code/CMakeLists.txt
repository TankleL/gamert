#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level")

project( antenna )

set(ANTENNA_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/antenna/inc)
set(ANTENNA_TEST_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/antenna-test/inc)

add_subdirectory( antenna )
add_subdirectory( antenna-test )

message("leave-level < top level")
