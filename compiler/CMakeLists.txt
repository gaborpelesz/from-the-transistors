cmake_minimum_required(VERSION 3.19)

project(Compiler VERSION 0.0.1
               DESCRIPTION "Scanner (lexical analyzer) generator"
               LANGUAGES C)

option(BUILD_TESTING "Build for unittesting." OFF)

# TESTING
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(CTest)
    include(cmake/FetchCMocka.cmake)
    add_compile_definitions(UNIT_TESTING=1 COMPILER_UNIT_TESTING=1)
endif()

# add utility library
add_subdirectory(cutils)

add_subdirectory(scanner)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()