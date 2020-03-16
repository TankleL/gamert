#antenna level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " antenna-test)

FILE(GLOB_RECURSE ANTENNA_TEST_INC_FILES "inc/*.hpp")
aux_source_directory (src ANTENNA_TEST_SRC_FILES)

include_directories(inc)
include_directories(${ANTENNA_INCLUDE})

add_executable( antenna_test ${ANTENNA_TEST_INC_FILES} ${ANTENNA_TEST_SRC_FILES} )
target_link_libraries( antenna_test antenna_a)

message("leave-level < antenna-test")
