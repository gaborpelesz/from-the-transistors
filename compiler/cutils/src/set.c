#include "../include/cutils/set.h"

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif // UNIT_TESTING

struct cutils_set128 cutils_set128_create(const char element) {
    return cutils_set128_create_fromlist(&element, 1);
}

struct cutils_set128 cutils_set128_create_fromlist(const char * const elements, const unsigned char n) {
    struct cutils_set128 A = {{0, 0, 0, 0}};

    for (unsigned char i = 0; i < n; i++) {
        if (elements[i] < 0) {
            printf("ERROR: cutils_set128_create_fromlist -> can't create set with element outside of 0 <= x <= 128: %d\n", elements[i]);
            exit(EXIT_FAILURE);
        }

        // determine bit in bit vector
        int nth_int = (int)(elements[i] / 32);
        int nth_bit = elements[i] - nth_int * 32;
        A._bitvector[nth_int] |= 1 << nth_bit;
    }

    return A;
}

struct cutils_set128 cutils_set128_empty() {
    return (const struct cutils_set128){{0, 0, 0, 0}};
}

struct cutils_set128 cutils_set128_intersection(const struct cutils_set128 A, const struct cutils_set128 B) {
    struct cutils_set128 C;

    for (unsigned char i = 0; i < 4; i++) {
        C._bitvector[i] = A._bitvector[i] & B._bitvector[i];
    }

    return C;
}

struct cutils_set128 cutils_set128_union(const struct cutils_set128 A, const struct cutils_set128 B) {
    struct cutils_set128 C;

    for (unsigned char i = 0; i < 4; i++) {
        C._bitvector[i] = A._bitvector[i] | B._bitvector[i];
    }

    return C;
}

struct cutils_set128 cutils_set128_difference(const struct cutils_set128 A, const struct cutils_set128 B) {
    struct cutils_set128 C;

    for (unsigned char i = 0; i < 4; i++) {
        C._bitvector[i] = A._bitvector[i] ^ B._bitvector[i];
    }

    return C;
}

struct cutils_set128 cutils_set128_negate(const struct cutils_set128 A) {
    struct cutils_set128 C;

    for (unsigned char i = 0; i < 4; i++) {
        C._bitvector[i] = ~A._bitvector[i];
    }

    return C;
}

unsigned char cutils_set128_isempty(const struct cutils_set128 A) {
    return (A._bitvector[0] | A._bitvector[1] |
           A._bitvector[2] | A._bitvector[3]) == 0;
}

char cutils_set128_smallest(const struct cutils_set128 A) {
    for (unsigned int i = 0; i < 4; i++) {
        if (A._bitvector[i] != 0) {
            unsigned int smallest = 0;

            while (((A._bitvector[i] >> smallest) & 1) == 0) {
                smallest++;
            }

            return smallest + 32*i;
        }
    }

    return -1;
}

unsigned char cutils_set128_has_element(const struct cutils_set128 A, const char element) {
    return !cutils_set128_isempty(
                cutils_set128_intersection(
                    A, 
                    cutils_set128_create(element)
                )
            );
}

unsigned char cutils_set128_size(const struct cutils_set128 A) {
    int total = 0;
    for (unsigned int i = 0; i < 4; i++) {
        // bit-trick for calculating bits in an int
        // http://aggregate.ee.engr.uky.edu/MAGIC/#Population%20Count%20(Ones%20Count)
        int s = A._bitvector[i];
        s = s - ((s >> 1) & 0x55555555);
        s = (s & 0x33333333) + ((s >> 2) & 0x33333333);
        s = (s + (s >> 4)) & 0x0F0F0F0F;
        total += (s * 0x01010101) >> 24;
    }
    return total;
}
