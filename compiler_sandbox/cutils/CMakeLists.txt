# MAIN CMAKE

cmake_minimum_required(VERSION 3.19)

project(cUtils VERSION 0.0.1
               DESCRIPTION "C utilities to be used in other projects"
               LANGUAGES C)

# TESTING
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()


add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

#add_library(cutils STATIC src/cstring.c include/cutils/cstring.h)

#target_include_directories(cutils PUBLIC include)

#add_library(cutils STATIC src/cstring.c include/cutils/cstring.h src/arrayi.c include/cutils/arrayi.h)
#target_include_directories(cutils PUBLIC include)