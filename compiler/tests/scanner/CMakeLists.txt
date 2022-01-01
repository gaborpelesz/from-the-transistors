# TEST SCANNER 
add_executable(test_regex_scanner test_regex_scanner.c)
target_link_libraries(test_regex_scanner PRIVATE scanner_utils cmocka-static)
add_test(scanner_unittests test_regex_scanner)