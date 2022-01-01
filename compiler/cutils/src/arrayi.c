#include "../include/cutils/arrayi.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif // UNIT_TESTING

struct cutils_arrayi *cutils_arrayi_create() {
    return _cutils_arrayi_create_allocate(0);
}

void cutils_arrayi_destroy(struct cutils_arrayi *arr) {
    if (arr->_arr != NULL) {
        free(arr->_arr);
        arr->_arr = NULL;
    }

    if (arr != NULL) {
        free(arr);
        arr = NULL;
    }
}

void cutils_arrayi_push(struct cutils_arrayi *const arr, int val) {
    unsigned int new_size = arr->size + 1;

    _CUTILS_REALLOC_ERROR ret = _cutils_arrayi_realloc(arr, new_size);

    if (ret != _REALLOC_ERROR) {
        arr->_arr[arr->size] = val;
        arr->size = new_size;
    }
}

void cutils_arrayi_empty(struct cutils_arrayi *const arr) {
    _cutils_arrayi_realloc(arr, 0);
    arr->size = 0;
}

int cutils_arrayi_pop(struct cutils_arrayi *const arr) {
    int popped = arr->_arr[arr->size-1];

    _cutils_arrayi_realloc(arr, arr->size-1);
    arr->size -= 1;

    return popped;
}

int cutils_arrayi_at(const struct cutils_arrayi *const arr, const unsigned int i) {
    if (i > arr->size) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, arr->size);
    }
    return arr->_arr[i];
}

void cutils_arrayi_remove_at(struct cutils_arrayi *const arr, unsigned int j) {
    if (arr->size <= j) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", j, arr->size);
    } 
    // removing last element
    else if (arr->size - 1 == j) {
        cutils_arrayi_pop(arr);
        return;
    }
    // 'size' <= 2 && 'j' is valid && not last element => must be first element
    else if (arr->size <= 2) {
        arr->_arr[0] = 0;
    }
    // backwards copying all element until 'j'
    else {
        int previous = arr->_arr[arr->size - 1];
        int current;

        // going backwards in the array
        // copying every element to left by 1 element
        // until we pass the element to be removed.
        for (int i = arr->size - 2; i >= (int)j; --i) {
            current = arr->_arr[i];
            arr->_arr[i] = previous;
            previous = current;
        }
    }


    _cutils_arrayi_realloc(arr, arr->size-1);
    arr->size -= 1;
}

struct cutils_arrayi *_cutils_arrayi_create_allocate(const unsigned int arr_size) {
    struct cutils_arrayi *arr = malloc(sizeof(struct cutils_arrayi));

    if (arr != NULL) {
        arr->_capacity = _cutils_arrayi_calc_capacity(arr_size);
        arr->_arr = malloc(arr->_capacity * sizeof(*(arr->_arr)));
        
        if (arr->_arr != NULL) {
            arr->size = 0;
        } else {
            printf("[cutils/arrayi.c -> _cutils_arrayi_create_allocate()] MALLOC ERROR: Couldn't allocate internal array.\n");
            free(arr);
            arr = NULL;
        }
    }

    return arr;
}

unsigned int _cutils_arrayi_calc_capacity(unsigned int size) {
    if (size < CUTILS_ARRAYI_INITIAL_CAPACITY) {
        return CUTILS_ARRAYI_INITIAL_CAPACITY;
    }

    unsigned int growth_factor_pow = (unsigned int)floorf(logf((size)/12)/logf(CUTILS_ARRAYI_GROWTH_FACTOR)) + 1;

    return CUTILS_ARRAYI_INITIAL_CAPACITY * (int)(powf(CUTILS_ARRAYI_GROWTH_FACTOR, growth_factor_pow) + 0.5);
}

/**
 * Growing/shrinking the internal array if necessary
 */
_CUTILS_REALLOC_ERROR _cutils_arrayi_realloc(struct cutils_arrayi * const arr, const unsigned int new_arr_size) {
    if (new_arr_size == arr->size) {
        return _REALLOC_NO_CHANGE;
    }

    unsigned int new_capacity = _cutils_arrayi_calc_capacity(new_arr_size);

    if (new_capacity == arr->_capacity) {
        return _REALLOC_NO_CHANGE;
    }

    // shrink rule
    if (new_arr_size < arr->size && new_arr_size > (int)(new_capacity * CUTILS_ARRAYI_SHRINK_FACTOR)) {
        return _REALLOC_NO_CHANGE;
    }

    void* new_arr = realloc(arr->_arr, new_capacity * sizeof(*(arr->_arr)));

    if (new_arr != NULL) {
        arr->_arr = new_arr;
        arr->_capacity = new_capacity;

        return _REALLOC_CHANGED;
    } else {
        printf("[cutils_arrayi.c -> _cutils_arrayi_realloc()] Reallocation error\n");
        return _REALLOC_ERROR;
    }
}