// bit vector implementation of sets

#ifndef CUTILS_SET_H
#define CUTILS_SET_H

#include "../include/cutils/common.h"

struct cutils_set128 {
    unsigned int _bitvector[4];
};

struct cutils_set128 cutils_set128_create(const char element);
struct cutils_set128 cutils_set128_create_fromlist(const char * const elements, const unsigned char n);
struct cutils_set128 cutils_set128_empty();

struct cutils_set128 cutils_set128_intersection(const struct cutils_set128 A, const struct cutils_set128 B);
struct cutils_set128 cutils_set128_union(const struct cutils_set128 A, const struct cutils_set128 B);
struct cutils_set128 cutils_set128_difference(const struct cutils_set128 A, const struct cutils_set128 B);
struct cutils_set128 cutils_set128_negate(const struct cutils_set128 A);

unsigned char cutils_set128_isempty(const struct cutils_set128 A);
unsigned char cutils_set128_has_element(const struct cutils_set128 A, const char element);

unsigned char cutils_set128_size(const struct cutils_set128 A);

/**
 * Returns the smallest element that the set contains
 */
char cutils_set128_smallest();

#endif // CUTILS_SET_H