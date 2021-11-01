add_library(cutils cstring.c "../include/cutils/cstring.h" arrayi.c "../include/cutils/arrayi.h")

target_link_libraries(cutils m)

target_include_directories(cutils PUBLIC ../include)