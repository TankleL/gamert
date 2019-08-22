#gamert level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > gamert")

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

if(${GRT_TARGET_OS_PLATFORM} STREQUAL "Windows")
	add_executable( gamert WIN32 ${GAMERT_OS_RELATED_INC_FILES} ${GAMERT_OS_RELATED_SRC_FILES} )
else()
# TODO: Support other platforms
endif()

message("leave-level < gamert")
