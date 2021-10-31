#include "../include/cutils/cstring.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif // UNIT_TESTING

// struct string {
//     unsigned int size; // equals to the number of characters excluding the termination char
//     unsigned int _capacity;
//     char* _s;
// };

struct string *string_create() {
    return _string_create_allocate(0);
}

struct string *string_create_from(const char *const src) {
    // 1. n <-- run through the list to determine its length
    // 2. allocate accordingly

    // ----------------------
    // unsigned int allocate_capacity = ( (n+STRING_TERMINATION_SIZE) + STRING_INITIAL_CAPACITY ) / STRING_INITIAL_CAPACITY * STRING_INITIAL_CAPACITY;
    // struct string *dest = _string_create_allocate(allocate_capacity);
    // memcpy(dest, src, n);

    // dest->size = n;
    // dest->_s[n] = '\0';

    return NULL;
}

struct string *_string_create_allocate(const unsigned int str_size) {
    // TODO
    // instead of creating by capacity, have a size and determine what
    // is the capacity needed for this size...

    return NULL;
}

/**
 * Assumes that "str" contains the previous size!
 */
void _string_realloc(struct string * const str, const unsigned int new_str_size) {
    // growing
    if (new_str_size > str->size) {

    }
    // shirking
    else if (new_str_size < str->size) {

    }
    
    // unsigned int shrink_limit = str->_capacity / STRING_SHRINK_FACTOR;
    // void* new_s = realloc(str->_s, shrink_limit);

    // if (new_s != NULL) {
    //     str->_s = new_s;
    // } else {
    //     // ... TODO error/warning couldn't reallocate array
    // }

    // if (str->size+STRING_TERMINATION_SIZE > cap) {
    //     printf("WARNING: [string.h] Reallocating string with loss of data.");
    //     str->size = cap - 1;
    //     str->_s[str->size] = '\0';
    // }
}

void string_destroy(struct string *const str) {
    free(str->_s);
    free(str);
}

void string_copy(struct string *const dst, const char *const src) {
    // MIGHT BE SHIT
    //      |
    //      v
    //
    // int new_cap = dst->size + STRING_TERMINATION_SIZE;
    // _string_realloc_growth(dst, new_cap);
    // memcpy(dst, src, n);

    // dst->size = n;
    // dst->_s[n] = '\0';
}

void string_empty(struct string *const str) {
    _string_realloc(str, 0);
    str->size = 0;
    str->_s[str->size] = '\0';
}

char string_at(const struct string *const str, const unsigned int i) {
    if (i > str->size) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, str->size);
        exit(1);
    }
    return str->_s[i];
}

char string_pop(struct string *const str) {
    str->size -= STRING_TERMINATION_SIZE;
    char popped = str->_s[str->size];
    // ... todo move terminating character
    // and return: str->s[str->size];
    return '0';
}

void string_append_chrlst(struct string *const dst, const char *const src) {

}


void string_append_chr(struct string *const dst, const char ch) {
    
}