#include "../include/cutils/string.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif // CUTILS_UNIT_TESTING

static inline unsigned int _cutils_string_chrlst_len(const char *const chrlst) {
    unsigned int size = 0;
    for(; chrlst[size] != '\0'; size++);
    return size;
}

struct cutils_string *cutils_string_create() {
    return _cutils_string_create_allocate(0);
}

struct cutils_string *cutils_string_create_from(const char *const src) {
    struct cutils_string* str = cutils_string_create();
    cutils_string_copy(str, src);
    return str;
}

struct cutils_string *_cutils_string_create_allocate(const unsigned int str_size) {
    struct cutils_string *str = malloc(sizeof(struct cutils_string));

    if (str != NULL) {
        str->_capacity = _cutils_string_calc_capacity(str_size);
        str->_s = malloc(str->_capacity * sizeof(char));
        
        if (str->_s != NULL) {
            str->size = 0;
            str->_s[0] = '\0';
        } else {
            printf("[cutils/string.c -> _cutils_string_create_allocate()] MALLOC ERROR: Couldn't allocate internal array.\n");
            free(str);
            str = NULL;
        }
    }

    return str;
}

unsigned int _cutils_string_calc_capacity(unsigned int size) {
    // +1 because of termination character
    unsigned int size_with_t = size + 1;

    if ((size_with_t) < CUTILS_STRING_INITIAL_CAPACITY) {
        return CUTILS_STRING_INITIAL_CAPACITY;
    }

    // size + 1 because of termination character
    unsigned int growth_factor_pow = (unsigned int)floorf(logf((size_with_t)/12)/logf(CUTILS_STRING_GROWTH_FACTOR)) + 1;

    return CUTILS_STRING_INITIAL_CAPACITY * (int)(powf(CUTILS_STRING_GROWTH_FACTOR, growth_factor_pow) + 0.5);
}

/**
 * Assumes that "str" contains the previous size and not the new!
 * 
 * Return:
 *  
 */
_CUTILS_REALLOC_ERROR _cutils_string_realloc(struct cutils_string * const str, const unsigned int new_str_size) {
    if (new_str_size == str->size) {
        return _REALLOC_NO_CHANGE;
    }

    unsigned int new_capacity = _cutils_string_calc_capacity(new_str_size);

    if (new_capacity == str->_capacity) {
        return _REALLOC_NO_CHANGE;
    }

    // shrink rule
    if (new_str_size < str->size && new_str_size > (int)(new_capacity * CUTILS_STRING_SHRINK_FACTOR)) {
        return _REALLOC_NO_CHANGE;
    }

    void* new_s = realloc(str->_s, new_capacity * sizeof(char));

    if (new_s != NULL) {
        str->_s = new_s;
        str->_capacity = new_capacity;

        return _REALLOC_CHANGED;
    } else {
        printf("[cutils/string.c -> _cutils_string_realloc()] Reallocation error\n");
        return _REALLOC_ERROR;
    }
}

void cutils_string_destroy(struct cutils_string *str) {
    if (str->_s != NULL) {
        free(str->_s);
        str->_s = NULL;
    }

    if (str != NULL) {
        free(str);
        str = NULL;
    }
}

void cutils_string_copy(struct cutils_string *const dst, const char *const src) {
    unsigned int size = _cutils_string_chrlst_len(src);

    _CUTILS_REALLOC_ERROR retr = _cutils_string_realloc(dst, size);

    if (retr != _REALLOC_ERROR) {
        memcpy(dst->_s, src, (size+1) * sizeof(char));
        dst->size = size;
    } else {

    }
}

void cutils_string_empty(struct cutils_string *const str) {
    _cutils_string_realloc(str, 0);
    str->size = 0;
    str->_s[str->size] = '\0';
}

char cutils_string_at(const struct cutils_string *const str, const unsigned int i) {
    if (i > str->size) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, str->size);
    }
    return str->_s[i];
}

char cutils_string_pop(struct cutils_string *const str) {
    char popped = str->_s[str->size-1];
    
    _cutils_string_realloc(str, str->size-1);

    str->size -= 1;
    str->_s[str->size] = '\0';
    
    return popped;
}

void cutils_string_append_chrlst(struct cutils_string *const dst, const char *const src) {
    unsigned int src_size = _cutils_string_chrlst_len(src);
    unsigned int new_size = dst->size + src_size;

    _CUTILS_REALLOC_ERROR ret = _cutils_string_realloc(dst, new_size);

    if (ret != _REALLOC_ERROR) {
        memcpy(dst->_s + dst->size * sizeof(char), src, (src_size+1) * sizeof(char));
        dst->size = new_size;
    }
}


void cutils_string_append_chr(struct cutils_string *const dst, const char ch) {
    unsigned int new_size = dst->size + 1;

    _CUTILS_REALLOC_ERROR ret = _cutils_string_realloc(dst, new_size);

    if (ret != _REALLOC_ERROR) {
        dst->_s[dst->size] = ch;
        dst->size = new_size;
        dst->_s[new_size] = '\0';
    }
}

int cutils_string_is_alphanum_c(char c) {
    // a-z: 97-122   | most common
    // 0-9: 48-57    |
    // A-Z: 65-90    v least common
    return 97 <= c && c <= 122 ||
           48 <= c && c <= 57 ||
           65 <= c && c <= 90;
}