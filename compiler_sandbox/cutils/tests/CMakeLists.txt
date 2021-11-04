# TEST CSTRING
add_executable(test_cstring test_cstring.c)
target_link_libraries(test_cstring PRIVATE cutils cmocka)
add_test(cutils_unittests test_cstring)

# TEST ARRAYI
add_executable(test_arrayi test_arrayi.c)
target_link_libraries(test_arrayi PRIVATE cutils cmocka)
add_test(cutils_unittests test_arrayi)
