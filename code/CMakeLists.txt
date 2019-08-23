#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level ---------------------------------------------")

project( gamert )

#Specify the target operating system, this could be "Windows", "IOS" ,"Android", "Linux", "MacOS"
SET(GRT_TARGET_OS_PLATFORM "Windows")

add_subdirectory( gamert )

message("leave-level < top level ---------------------------------------------")

