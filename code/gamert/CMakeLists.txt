#gamert level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > gamert ------------------------------------------------")

#Look for Vulkan.
message("look for vulkan -----------------------------------------------------")
find_package(Vulkan REQUIRED)

message("Vulkan found? " ${VULKAN_FOUND})
message("Vulkan libraries: " ${Vulkan_LIBRARIES})
message("Vulkan include: " ${Vulkan_INCLUDE_DIRS})

#Define macros
add_definitions(-DTEST_RESOURCES_ROOT="../../../test-env/gamert-dev-root")

#Add inc and src files
if(${GRT_TARGET_OS_PLATFORM} STREQUAL "Windows")
	include_directories(inc/win)
	FILE(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/win/*.hpp")
	aux_source_directory (src/win GAMERT_OS_RELATED_SRC_FILES)
elseif(${GRT_TARGET_OS_PLATFORM} STREQUAL "Android")
	include_directories(inc/android)
	FILE(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/android/*.hpp")
	aux_source_directory (src/android GAMERT_OS_RELATED_SRC_FILES)
elseif(${GRT_TARGET_OS_PLATFORM} STREQUAL "IOS")
	include_directories(inc/ios)
	FILE(GLOB_RECURSE GAMERT_OS_RELATED_INC_FILES "inc/ios/*.hpp")
	aux_source_directory (src/ios GAMERT_OS_RELATED_SRC_FILES)
endif()

include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(inc)

FILE(GLOB_RECURSE GAMERT_VKLAYER_INC_FILES "inc/vklayer/*.hpp")
include_directories(inc/vklayer)
aux_source_directory(src/vklayer GAMERT_VKLAYER_SRC_FILES)

FILE(GLOB_RECURSE GAMERT_GRAPHICS_INC_FILES "inc/graphics/*.hpp")
include_directories(inc/graphics)
aux_source_directory(src/graphics GAMERT_GRAPHICS_SRC_FILES)

FILE(GLOB_RECURSE GAMERT_LOGIC_INC_FILES "inc/logic/*.hpp")
include_directories(inc/logic)
aux_source_directory(src/logic GAMERT_LOGIC_SRC_FILES)

FILE(GLOB_RECURSE GAMERT_FILESYS_INC_FILES "inc/fs/*.hpp")
include_directories(inc/fs)
aux_source_directory(src/fs GAMERT_FILESYS_SRC_FILES)

FILE(GLOB_RECURSE GAMERT_OSYS_INC_FILES "inc/osys/*.hpp")
include_directories(inc/osys)
aux_source_directory(src/osys GAMERT_OSYS_SRC_FILES)

if(${GRT_TARGET_OS_PLATFORM} STREQUAL "Windows")
	message("[gamert] Add executable and link libs -------------------------------")
	add_executable( gamert WIN32
		#Header files:
		"inc/pre-req.hpp"
		"inc/singleton.hpp"
		${GAMERT_VKLAYER_INC_FILES}
		${GAMERT_GRAPHICS_INC_FILES}
		${GAMERT_FILESYS_INC_FILES}
		${GAMERT_LOGIC_INC_FILES}
		${GAMERT_OS_RELATED_INC_FILES}
		${GAMERT_OSYS_INC_FILES}
		#Source files:
		${GAMERT_VKLAYER_SRC_FILES}
		${GAMERT_GRAPHICS_SRC_FILES}
		${GAMERT_LOGIC_SRC_FILES}
		${GAMERT_FILESYS_SRC_FILES}
		${GAMERT_OS_RELATED_SRC_FILES}
		${GAMERT_OSYS_SRC_FILES}
	)
	target_link_libraries( gamert ${Vulkan_LIBRARIES} )
else()
# TODO: Support other platforms
endif()

message("leave-level < gamert ------------------------------------------------")
