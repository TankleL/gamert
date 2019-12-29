cmake_minimum_required (VERSION 3.0)

FILE(GLOB_RECURSE TINYXML2_INC_FILES "inc/*.h")
aux_source_directory (src TINYXML2_SRC_FILES)

include_directories(inc)

set(tinyxml_cflags)

if(MSVC)
	list(APPEND tinyxml_cflags /W0)
elseif()
	list(APPEND tinyxml_cflags -Wall -Wextra -Wstrict-prototypes)
endif()

add_library(tinyxml2_a STATIC ${TINYXML2_INC_FILES} ${TINYXML2_SRC_FILES})
target_compile_options(tinyxml2_a PRIVATE ${tinyxml_cflags})

