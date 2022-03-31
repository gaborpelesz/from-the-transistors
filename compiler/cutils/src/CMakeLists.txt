add_library(cutils "string.c" "../include/cutils/string.h"
                   "arrayi.c" "../include/cutils/arrayi.h"
                   "set.c"    "../include/cutils/set.h")

# linking <math.h> library
target_link_libraries(cutils m)

target_include_directories(cutils PUBLIC ../include)