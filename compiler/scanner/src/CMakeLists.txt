add_library(scanner_utils regex_parser.c ../include/scanner_utils/regex_parser.h
                          regex_tree.c ../include/scanner_utils/regex_tree.h
                          fa.c ../include/scanner_utils/fa.h)
target_include_directories(scanner_utils PUBLIC ../include)
target_link_libraries(scanner_utils cutils)

add_executable(scanner scanner.c)
target_link_libraries(scanner cutils scanner_utils)

add_executable(generator generator.c)
target_link_libraries(generator cutils scanner_utils)