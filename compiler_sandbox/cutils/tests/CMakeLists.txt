# TEST CSTRING
add_executable(test_string test_string.c)
target_link_libraries(test_string PRIVATE cutils cmocka)
add_test(cutils_unittests test_string)

# TEST ARRAYI
add_executable(test_arrayi test_arrayi.c)
target_link_libraries(test_arrayi PRIVATE cutils cmocka)
add_test(cutils_unittests test_arrayi)
