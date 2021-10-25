# TEST STRING
add_executable(test_cstring test_cstring.c)

#target_compile_features(test_cstring PRIVATE)

target_link_libraries(test_cstring PRIVATE cutils cmocka)

add_test(test_cutils_cstring test_cstring)