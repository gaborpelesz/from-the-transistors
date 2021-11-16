cmake_minimum_required(VERSION 3.19)

project(Scanner VERSION 0.0.1
               DESCRIPTION "Scanner (lexical analyzer) generator"
               LANGUAGES C)

option(BUILD_TESTING "Build for unittesting." OFF)

# add utility library
add_subdirectory(cutils)

add_subdirectory(scanner)