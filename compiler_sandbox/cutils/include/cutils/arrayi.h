#ifndef ARRAYI_H
#define ARRAYI_H

#define ARRAYI_INITIAL_CAPACITY 12
#define ARRAYI_GROWTH_FACTOR 2
#define ARRAYI_SHRINK_FACTOR 2.f/3.f

typedef enum _ARRAYI_ERROR {
    _REALLOC_ERROR = -1,
    _REALLOC_CHANGED = 0,
    _REALLOC_NO_CHANGE = 1
} _ARRAYI_ERROR;

struct arrayi {
    unsigned int size;
    unsigned int _capacity;
    int* _arr;
};

struct arrayi *arrayi_create();

void arrayi_destroy(struct arrayi *arr);

void arrayi_push(struct arrayi *const arr, int val);

void arrayi_empty(struct arrayi *const arr);

int arrayi_pop(struct arrayi *const arr);

int arrayi_at(const struct arrayi *const arr, const unsigned int i);

/**
 * Removes an element from the array at a given index.
 * Retains the order of the elements.
 * Implements a backwards removal algorithm
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index where we want to remove an element
 */
void arrayi_remove_at(struct arrayi *const arr, unsigned int j);


struct arrayi *_arrayi_create_allocate(const unsigned int arr_size);

unsigned int _arrayi_calc_capacity(unsigned int size);

/**
 * Growing/shrinking the internal array if necessary
 */
_ARRAYI_ERROR _arrayi_realloc(struct arrayi * const arr, const unsigned int new_arr_size);

#endif