cmake_minimum_required(VERSION 3.1.3)
project(ClearScreen CXX)

set(SourceDir "${CMAKE_CURRENT_LIST_DIR}/Source")
set(SharedSourceDir "${CMAKE_CURRENT_LIST_DIR}/../Shared")

file(GLOB_RECURSE Headers LIST_DIRECTORIES false "${SourceDir}/*.h")
file(GLOB_RECURSE SharedHeaders LIST_DIRECTORIES false "${SharedSourceDir}/*.h")

file(GLOB_RECURSE Sources LIST_DIRECTORIES false "${SourceDir}/*.cpp")
file(GLOB_RECURSE SharedSources LIST_DIRECTORIES false "${SharedSourceDir}/*.cpp")

file(GLOB_RECURSE Inlines LIST_DIRECTORIES false "${SourceDir}/*.inl")
file(GLOB_RECURSE SharedInlines LIST_DIRECTORIES false "${SharedSourceDir}/*.inl")

set(SharedSourceFiles ${SharedHeaders} ${SharedInlines} ${SharedSources})
foreach(SourceFile IN ITEMS ${SharedSourceFiles})
    get_filename_component(FilePath "${SourceFile}" PATH)
    file(RELATIVE_PATH FilePathRel ${SharedSourceDir} ${FilePath})
    string(REPLACE "/" "\\" GroupPath "${FilePathRel}")
    source_group("${GroupPath}" FILES "${SourceFile}")
endforeach()

set(SourceFiles ${Headers} ${Inlines} ${Sources})
foreach(SourceFile IN ITEMS ${SourceFiles})
    get_filename_component(FilePath "${SourceFile}" PATH)
    file(RELATIVE_PATH FilePathRel ${SourceDir} ${FilePath})
    string(REPLACE "/" "\\" GroupPath "${FilePathRel}")
    source_group("${GroupPath}" FILES "${SourceFile}")
endforeach()

include_directories(AFTER "${SourceDir}")
include_directories(AFTER "${SharedSourceDir}")
include_directories(AFTER "$ENV{VULKAN_SDK}/Include/vulkan")

set(SourceFiles ${SourceFiles} ${SharedSourceFiles})
add_executable(ClearScreen WIN32 "${SourceFiles}")
set_target_properties(ClearScreen PROPERTIES COMPILE_FLAGS "/YcGlobals.h")
