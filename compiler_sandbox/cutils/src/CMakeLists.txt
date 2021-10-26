add_library(cutils cstring.c "../include/cutils/cstring.h" arrayi.c "../include/cutils/arrayi.h")

target_include_directories(cutils PUBLIC ../include)