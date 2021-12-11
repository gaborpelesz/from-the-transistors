add_library(scanner_utils regex_scanner.c ../include/scanner_utils/regex_scanner.h)
target_include_directories(scanner_utils PUBLIC ../include)
target_link_libraries(scanner_utils cutils)

add_executable(scanner scanner.c)
target_link_libraries(scanner cutils scanner_utils)

add_executable(generator generator.c)
target_link_libraries(generator cutils scanner_utils)