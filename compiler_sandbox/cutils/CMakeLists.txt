cmake_minimum_required(VERSION 3.19)

project(cUtils VERSION 0.0.1
               DESCRIPTION "C utilities to be used in other projects"
               LANGUAGES C)

add_library(cstring STATIC cstring.cpp cstring.h)

target_include_directories(check PUBLIC include)