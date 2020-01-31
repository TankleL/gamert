cmake_minimum_required (VERSION 3.0)

project(lua535)

aux_source_directory(src LUA_SRC_TOTAL)
include_directories(src)

add_definitions(-DLUA_COMPAT_5_2)

#prepare lua library sources, store them in LUA_LIB_SRC
set(LUA_LIB_SRC ${LUA_SRC_TOTAL})
list(REMOVE_ITEM LUA_LIB_SRC lua.c, luac.c)

#prepare lua vm sources, store them in LUA_VM_SRC
set(LUA_VM_SRC src/lua.c)

#prepare lua compiler sources, store them in LUA_COMPILER_SRC
set(LUA_COMPILER_SRC src/luac.c)

#make lua static library
add_library(lua_a STATIC ${LUA_LIB_SRC})

#make lua vm
add_executable(luavm ${LUA_VM_SRC})
target_link_libraries(luavm lua_a)

#make lua compiler
add_executable(luac ${LUA_COMPILER_SRC})
target_link_libraries(luac lua_a)




