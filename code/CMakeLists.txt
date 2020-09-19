#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level ---------------------------------------------")

project( gamert )

#Specify the target operating system, this could be "Windows", "IOS" ,"Android", "Linux", "MacOS"
set(GRT_TARGET_OS_PLATFORM "Windows")

set(TINYXML2_INC    ${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2/inc)
set(LUA535_INC      ${CMAKE_CURRENT_SOURCE_DIR}/deps/lua535/src)
#set(ANTENNA_INC     ${CMAKE_CURRENT_SOURCE_DIR}/deps/antenna/code/antenna/inc)
set(LOTUS_INC       ${CMAKE_CURRENT_SOURCE_DIR}/deps/lotus/code/lotus/inc)

add_subdirectory( deps/tinyxml2 )
add_subdirectory( deps/lua535 )
#add_subdirectory( deps/antenna/code/antenna )
add_subdirectory( deps/lotus/code)
add_subdirectory( gamert )

message("leave-level < top level ---------------------------------------------")

