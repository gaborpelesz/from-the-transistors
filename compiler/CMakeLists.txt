cmake_minimum_required(VERSION 3.19)

project(Compiler VERSION 0.0.1
               DESCRIPTION "Scanner (lexical analyzer) generator"
               LANGUAGES C)

set(CMAKE_C_STANDARD 11)

option(BUILD_TESTING "Build for unittesting." OFF)

# TESTING
if(BUILD_TESTING)
    include(CTest)
    include(cmake/FetchCMocka.cmake)
    add_compile_definitions(UNIT_TESTING=1)
endif()

# add utility library
add_subdirectory(cutils)

add_subdirectory(scanner)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()