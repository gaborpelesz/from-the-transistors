#ifndef CUTILS_STRING_H
#define CUTILS_STRING_H

#define CUTILS_STRING_INITIAL_CAPACITY 12
#define CUTILS_STRING_GROWTH_FACTOR 2
#define CUTILS_STRING_SHRINK_FACTOR 2.f/3.f

#include "../include/cutils/common.h"

/**
 * Implementation decisions:
 *  - does size include termination character?
 *      - No.
 *  - when passing a const char*, do we need to provide the number of chars in the array?
 *      - No.
 */
struct cutils_string {
    unsigned int size; // equals to the number of characters excluding the termination char
    unsigned int _capacity;
    char* _s;
};

/**
 * Creates a pointer to a cutils_string object
 */
struct cutils_string *cutils_string_create();

/**
 * Creates a pointer to a cutils_string from a pointer to a char list by COPY.
 */
struct cutils_string *cutils_string_create_from(const char *const src);

void cutils_string_destroy(struct cutils_string *str);

struct cutils_string *_cutils_string_create_allocate(const unsigned int str_size);

unsigned int _cutils_string_calc_capacity(unsigned int size);

/**
 * Growing/shrinking the internal array if necessary
 */
_CUTILS_REALLOC_ERROR _cutils_string_realloc(struct cutils_string * const str, const unsigned int new_str_size);

void cutils_string_copy(struct cutils_string *const dst, const char *const src);

// deletes the elements of this cutils_string
void cutils_string_empty(struct cutils_string *const str);

void cutils_string_append_chrlst(struct cutils_string *const dst, const char *const src);

// append ch at the end of dst
void cutils_string_append_chr(struct cutils_string *const dst, const char ch);

char cutils_string_at(const struct cutils_string *const str, const unsigned int i);

char cutils_string_pop(struct cutils_string *const str);

/* MISC FUNCTION */

int cutils_string_is_alphanum_c(char c);

/* --------------- */

#endif // CUTILS_STRING_H