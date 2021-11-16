#ifndef CUTILS_ARRAYI_H
#define CUTILS_ARRAYI_H

#define CUTILS_ARRAYI_INITIAL_CAPACITY 12
#define CUTILS_ARRAYI_GROWTH_FACTOR 2
#define CUTILS_ARRAYI_SHRINK_FACTOR 2.f/3.f

#include "../include/cutils/common.h"

struct cutils_arrayi {
    unsigned int size;
    unsigned int _capacity;
    int* _arr;
};

struct cutils_arrayi *cutils_arrayi_create();

void cutils_arrayi_destroy(struct cutils_arrayi *arr);

void cutils_arrayi_push(struct cutils_arrayi *const arr, int val);

void cutils_arrayi_empty(struct cutils_arrayi *const arr);

int cutils_arrayi_pop(struct cutils_arrayi *const arr);

int cutils_arrayi_at(const struct cutils_arrayi *const arr, const unsigned int i);

/**
 * Removes an element from the array at a given index.
 * Retains the order of the elements.
 * Implements a backwards removal algorithm
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index where we want to remove an element
 */
void cutils_arrayi_remove_at(struct cutils_arrayi *const arr, unsigned int j);


struct cutils_arrayi *_cutils_arrayi_create_allocate(const unsigned int arr_size);

unsigned int _cutils_arrayi_calc_capacity(unsigned int size);

/**
 * Growing/shrinking the internal array if necessary
 */
_CUTILS_REALLOC_ERROR _cutils_arrayi_realloc(struct cutils_arrayi * const arr, const unsigned int new_arr_size);

#endif // CUTILS_ARRAYI_H