# TEST SCANNER
target_link_libraries(scanner_utils cmocka-static)

# TEST REGEX TREE
add_executable(test_regex_tree test_regex_tree.c)
target_link_libraries(test_regex_tree PRIVATE scanner_utils cmocka-static)
add_test(scanner_utils test_regex_tree)

# TEST REGEX PARSER
add_executable(test_regex_parser test_regex_parser.c)
target_link_libraries(test_regex_parser PRIVATE scanner_utils cmocka-static)
add_test(scanner_unittests test_regex_parser)

# TEST SCANNER FA
add_executable(test_scanner_fa test_scanner_fa.c)
target_link_libraries(test_scanner_fa PRIVATE scanner_utils cmocka-static)
add_test(scanner_unittests test_scanner_fa)