#include "../include/cutils/cstring.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

// struct string {
//     unsigned int size; // equals to the number of characters excluding the termination char
//     unsigned int _capacity;
//     char* _s;
// };

struct string *string_create() {
    return _string_create_allocate(STRING_INITIAL_CAPACITY);
}

struct string *string_create_from(const char *const src, const unsigned int n) {
    // init should add termination char
    // termination char should not be included in size

    unsigned int allocate_capacity = ( (n+STRING_TERMINATION_SIZE) + STRING_INITIAL_CAPACITY ) / STRING_INITIAL_CAPACITY * STRING_INITIAL_CAPACITY;
    struct string *dest = _string_create_allocate(allocate_capacity);
    memcpy(dest, src, n);

    dest->size = n;
    dest->_s[n] = '\0';

    return dest;
}

struct string *_string_create_allocate(const unsigned int cap) {
    struct string *str = malloc( sizeof(struct string) );

    str->_capacity = cap;
    str->_s = malloc(cap * sizeof(*(str->_s)));

    str->size = 0;
    str->_s[str->size] = '\0';

    return str;
}

void _string_realloc_growth(struct string * const str, const unsigned int cap) {
    unsigned int shrink_limit = str->_capacity / STRING_SHRINK_FACTOR;
    void* new_s = realloc(str->_s, shrink_limit);

    if (new_s != NULL) {
        str->_s = new_s;
    } else {
        // ... TODO error/warning couldn't reallocate array
    }

    if (str->size+STRING_TERMINATION_SIZE > cap) {
        printf("WARNING: [string.h] Reallocating string with loss of data.");
        str->size = cap - 1;
        str->_s[str->size] = '\0';
    }
}

int _string_realloc_shrink(struct string * const str) {
    unsigned int shrink_limit = str->_capacity / STRING_SHRINK_FACTOR;
    
    if (str->size+STRING_TERMINATION_SIZE < shrink_limit) {
        void* new_s = realloc(str->_s, shrink_limit);

        if (new_s != NULL) {
            str->_s = new_s;
        } else {
            // ... TODO error/warning couldn't reallocate array
        }
    }

    return 0;
}

void string_destroy(struct string *const str) {
    free(str->_s);
    free(str);
}

void string_copy(struct string *const dst, const char *const src, const unsigned int n) {
    int new_cap = dst->size + n + STRING_TERMINATION_SIZE;
    _string_realloc_growth(dst, new_cap);
    memcpy(dst, src, n);

    dst->size = n;
    dst->_s[n] = '\0';
}

void string_empty(struct string *const str) {
    str->size = 0;
    _string_realloc_shrink(str);
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