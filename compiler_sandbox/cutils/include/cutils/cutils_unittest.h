/**
 * UNIT TESTING BUILD ONLY
 * 
 * The purpose of this header is to change the common memory function
 * and other essential/utility or internal functions to have an extended
 * behavior, so they are unit-testable or easily debug-able
 * 
 * Include like this for unit testing builds:
 * ------------------
 *     #ifdef UNIT_TESTING
 *         #include <cutils/cutils_unittest.h>
 *     #endif // UNIT_TESTING
 * 
 */

#ifndef CUTILS_MEMORY_TEST_H
#define CUTILS_MEMORY_TEST_H

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

typedef unsigned long size_t;
extern void* _test_malloc(const size_t size, const char* file, const int line);
extern void* _test_calloc(size_t nmemb, const size_t size, const char* file, const int line);
extern void* _test_realloc(void *ptr, const size_t size, const char* file, const int line);
extern void _test_free(void *ptr, const char* file, const int line);

#ifdef malloc
#undef malloc
#endif
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)

#ifdef calloc
#undef calloc
#endif
#define calloc(nmemb,size) _test_calloc(nmemb, size, __FILE__, __LINE__)

#ifdef realloc
#undef realloc
#endif
#define realloc(ptr,size) _test_realloc(ptr, size, __FILE__, __LINE__)

#ifdef free
#undef free
#endif
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

#endif // CUTILS_MEMORY_TEST_H