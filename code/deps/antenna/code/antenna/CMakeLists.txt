#antenna level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " antenna)

FILE(GLOB_RECURSE ANTENNA_INC_FILES "inc/*.hpp")
aux_source_directory (src ANTENNA_SRC_FILES)

include_directories(inc)
include_directories(deps/libuv/include)

add_subdirectory(deps/libuv)

add_library( antenna_a STATIC ${ANTENNA_INC_FILES} ${ANTENNA_SRC_FILES} )
target_link_libraries( antenna_a uv_a )

message("leave-level < antenna")
