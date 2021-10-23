// Implementation of: struct array

#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>

struct array {
    unsigned int _size;
    unsigned int _capacity;
    unsigned char _size_t; // size of the type we are storing
    void* _arr;
};

/**
 * Creates a dynamic array. 
 * The initial capacity of the array is 10.
 */
struct array *array_create(unsigned char sizeof_element) {
    // arr->_capacity = 10;
    // arr->_el_size = sizeof_element;
    // arr->_arr = malloc( arr->_capacity * (arr->_el_size) );
}

void array_push(struct array * const arr, void *el) {
    // TODO
}

void *array_pop(struct array * const arr) {
    
}

/**
 * Returns an element from the array at a given index.
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index from where we want to retreive an element
 */
void *array_at(struct array * const arr, unsigned int i) {
    // if (i > arr->_size) {
    //     printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, arr->_size);
    //     return;
    // }

    // return arr->_arr + i*arr->_el_size;
}

/**
 * Removes an element from the array at a given index.
 * Retains the order of the elements.
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 *  -   i: index where we want to remove an element
 */
void array_remove_at(struct array * const arr, unsigned int j) {
    // backwards removal
    
    void *i = arr->_arr + arr->_size * (arr->_size_t - 1);

    void *last = ; // pointer to the last element
    char *saved = last;
    --last; // so "last" points to the element before the last element

    // going backwards in the array with pointer "last"
    for (; last != arr->_arr; last -= arr->_size_t) {
        saved = actual;
        actual = last;
        last = saved;
        memcpy(position, value, size);
    }

    --arr->_size; // array shrinks
}

/**
 * Removes every element from the array.
 * 
 * Inputs:
 *  - arr: pointer, pointing to the struct array object
 */
void array_empty(struct array * const arr) {
    // emptying is very easy, we just 
}

#endif