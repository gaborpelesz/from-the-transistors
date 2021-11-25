add_executable(scanner scanner.c)
target_link_libraries(scanner cutils)

add_executable(generator generator.c)
target_link_libraries(generator cutils)