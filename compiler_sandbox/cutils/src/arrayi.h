// Implementation of: struct array

#ifndef ARRAYI_H
#define ARRAYI_H

#include <stdio.h>
#include <stdlib.h>

#define ARRAYI_GROWTH_FACTOR 2
#define ARRAYI_SHRINK_FACTOR 3
#define ARRAYI_INITIAL_CAP 12

struct arrayi {
    unsigned int size;
    unsigned int _capacity;
    int* _arr;
};

/**
 * Creates a dynamic array. 
 * The initial capacity of the array is 10.
 */
struct arrayi *arrayi_create() {
    struct arrayi *a = (struct arrayi*)malloc(sizeof(struct arrayi));
    
    a->_capacity = ARRAYI_INITIAL_CAP;
    a->_arr = (int *)malloc(ARRAYI_INITIAL_CAP * sizeof(*(a->_arr)));

    a->size = 0;

    return a;
}

void arrayi_destroy(struct arrayi *arr) {
    free(arr->_arr);
    free(arr);
}

void arrayi_push(struct arrayi * const arr, int new_val) {
    if (arr->size == arr->_capacity) {
        realloc(arr->_arr, arr->_capacity * ARRAYI_GROWTH_FACTOR);
    }

    arr->_arr[arr->size] = new_val;
    ++(arr->size);
}

void _arrayi_realloc_shrink(struct arrayi * const arr) {
    unsigned int shrink_limit = arr->_capacity / ARRAYI_SHRINK_FACTOR;
    if (arr->size < shrink_limit) {
        realloc(arr->_arr, shrink_limit);
    }
}

int arrayi_pop(struct arrayi * const arr) {
    --(arr->size);
    int popped = arr->_arr[arr->size];

    _arrayi_realloc_shrink(arr);
    
    return popped;
}

/**
 * Returns an element from the array at a given index.
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index from where we want to retreive an element
 */
int arrayi_at(struct arrayi * const arr, unsigned int i) {
    if (i > arr->size) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, arr->size);
        exit(1);
    }

    return arr->_arr[i];
}

/**
 * Removes an element from the array at a given index.
 * Retains the order of the elements.
 * Implements a backwards removal algorithm
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index where we want to remove an element
 */
void arrayi_remove_at(struct arrayi * const arr, unsigned int j) {
    int previous = arr->_arr[arr->size - 1];
    int current;

    // going backwards in the array
    // copying every element to left by 1 element
    // until we pass the element to be removed.
    for (int i = arr->size - 2; i >= j; --i) {
        current = arr->_arr[i];
        arr->_arr[i] = previous;
        previous = current;
    }

    --arr->size; // array shrinks
    _arrayi_realloc_shrink(arr);
}

/**
 * Removes every element from the array.
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 */
void arrayi_empty(struct arrayi * const arr) {
    // emptying is very easy, we just
    arr->size = 0;
    _arrayi_realloc_shrink(arr);
}

#endif