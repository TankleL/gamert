#gamert level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > gamert ------------------------------------------------")

#Look for Vulkan.
message("look for vulkan -----------------------------------------------------")
find_package(Vulkan REQUIRED)

message("Vulkan found? " ${VULKAN_FOUND})
message("Vulkan libraries: " ${Vulkan_LIBRARIES})
message("Vulkan include: " ${Vulkan_INCLUDE_DIRS})

include_directories( ${TINYXML2_INC} )
include_directories( ${LUA535_INC} )
include_directories( ${LOTUS_INC} )

#Define macros
add_definitions(-DTEST_RESOURCES_ROOT="../../../test-env/gamert-dev-root")

#Add inc and src files
if(${GRT_TARGET_OS_PLATFORM} STREQUAL "Windows")
	include_directories(inc/win)
	file(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/win/*.hpp")
	aux_source_directory (src/win GAMERT_OS_RELATED_SRC_FILES)
elseif(${GRT_TARGET_OS_PLATFORM} STREQUAL "Android")
	include_directories(inc/android)
	file(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/android/*.hpp")
	aux_source_directory (src/android GAMERT_OS_RELATED_SRC_FILES)
elseif(${GRT_TARGET_OS_PLATFORM} STREQUAL "IOS")
	include_directories(inc/ios)
	file(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/ios/*.hpp")
	aux_source_directory (src/ios GAMERT_OS_RELATED_SRC_FILES)
endif()

include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(inc)

file(GLOB_RECURSE GAMERT_VKLAYER_INC_FILES "inc/vklayer/*.hpp")
include_directories(inc/vklayer)
aux_source_directory(src/vklayer GAMERT_VKLAYER_SRC_FILES)

file(GLOB_RECURSE GAMERT_GRAPHICS_INC_FILES "inc/graphics/*.hpp")
include_directories(inc/graphics)
aux_source_directory(src/graphics GAMERT_GRAPHICS_SRC_FILES)

file(GLOB_RECURSE GAMERT_LOGIC_INC_FILES "inc/logic/*.hpp")
include_directories(inc/logic)
aux_source_directory(src/logic GAMERT_LOGIC_SRC_FILES)

file(GLOB_RECURSE GAMERT_FILESYS_INC_FILES "inc/fs/*.hpp")
include_directories(inc/fs)
aux_source_directory(src/fs GAMERT_FILESYS_SRC_FILES)

file(GLOB_RECURSE GAMERT_OSYS_INC_FILES "inc/osys/*.hpp")
include_directories(inc/osys)
aux_source_directory(src/osys GAMERT_OSYS_SRC_FILES)

file(GLOB_RECURSE GAMERT_UTILS_INC_FILES "inc/utils/*.hpp")
include_directories(inc/utils)
aux_source_directory(src/utils GAMERT_UTILS_SRC_FILES)

file(GLOB_RECURSE GAMERT_NETWORKS_INC_FILES "inc/networks/*.hpp")
include_directories(inc/networks)
aux_source_directory(src/networks GAMERT_NETWORKS_SRC_FILES)

file(GLOB_RECURSE GAMERT_DYNOPT_LUA_INC_FILES "inc/dynopt-lua/*.hpp")
include_directories(inc/dynopt-lua)
aux_source_directory(src/dynopt-lua GAMERT_DYNOPT_LUA_SRC_FILES)

file(GLOB_RECURSE GAMERT_DYNOPT_LUA_EXP_INC_FILES "inc/dynopt-lua-export/*.hpp")
include_directories(inc/dynopt-lua-export)
aux_source_directory(src/dynopt-lua-export GAMERT_DYNOPT_LUA_EXP_SRC_FILES)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${GRT_TARGET_OS_PLATFORM} STREQUAL "Windows")
	message("[gamert] Add executable and link libs -------------------------------")
	add_executable( gamert WIN32
		#Header files:
		"inc/pre-req.hpp"
		"inc/singleton.hpp"
		"inc/gamert-application.hpp"
		${GAMERT_VKLAYER_INC_FILES}
		${GAMERT_GRAPHICS_INC_FILES}
		${GAMERT_FILESYS_INC_FILES}
		${GAMERT_LOGIC_INC_FILES}
		${GAMERT_OS_RELATED_INC_FILES}
		${GAMERT_OSYS_INC_FILES}
		${GAMERT_UTILS_INC_FILES}
		${GAMERT_NETWORKS_INC_FILES}
		${GAMERT_DYNOPT_LUA_INC_FILES}
		${GAMERT_DYNOPT_LUA_EXP_INC_FILES}
		#Source files:
		${GAMERT_VKLAYER_SRC_FILES}
		${GAMERT_GRAPHICS_SRC_FILES}
		${GAMERT_LOGIC_SRC_FILES}
		${GAMERT_FILESYS_SRC_FILES}
		${GAMERT_OS_RELATED_SRC_FILES}
		${GAMERT_OSYS_SRC_FILES}
		${GAMERT_UTILS_SRC_FILES}
		${GAMERT_NETWORKS_SRC_FILES}
		${GAMERT_DYNOPT_LUA_SRC_FILES}
		${GAMERT_DYNOPT_LUA_EXP_SRC_FILES}
		"src/gamert-application.cpp"
	)
	target_link_libraries( gamert ${Vulkan_LIBRARIES} )
	target_link_libraries( gamert "xinput.lib" )
	target_link_libraries( gamert tinyxml2_a )
	target_link_libraries( gamert lotus_a )
	target_link_libraries( gamert lua_a )
else()
# TODO: Support other platforms
endif()

message("leave-level < gamert ------------------------------------------------")
