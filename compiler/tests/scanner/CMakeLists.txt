# TEST SCANNER
target_link_libraries(scanner_utils cmocka-static)

add_executable(test_regex_parser test_regex_parser.c)
target_link_libraries(test_regex_parser PRIVATE scanner_utils cmocka-static)
add_test(scanner_unittests test_regex_parser)