cmake_minimum_required(VERSION 3.21.3 FATAL_ERROR)

project(CMockaTest LANGUAGES C)

# include tests
add_executable(test_cmocka test.c)
target_link_libraries(test_cmocka cmocka)
add_test(test_cmocka test_cmocka)