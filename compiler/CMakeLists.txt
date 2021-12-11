cmake_minimum_required(VERSION 3.19)

project(Scanner VERSION 0.0.1
               DESCRIPTION "Scanner (lexical analyzer) generator"
               LANGUAGES C)

option(BUILD_TESTING "Build for unittesting." OFF)

# TESTING
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(CTest)
    add_compile_definitions(COMPILER_UNIT_TESTING=1)
endif()

# add utility library
add_subdirectory(cutils)

add_subdirectory(scanner)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()