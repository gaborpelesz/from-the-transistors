#ifndef CSTRING_H
#define CSTRING_H

#define STRING_INITIAL_CAPACITY 12
#define STRING_GROWTH_FACTOR 2
#define STRING_SHRINK_FACTOR 2.f/3.f

typedef enum _STRING_ERROR {
    _REALLOC_ERROR = -1,
    _REALLOC_CHANGED = 0,
    _REALLOC_NO_CHANGE = 1
} _STRING_ERROR;

/**
 * Implementation decisions:
 *  - does size include termination character?
 *      - No.
 *  - when passing a const char*, do we need to provide the number of chars in the array?
 *      - No.
 */
struct string {
    unsigned int size; // equals to the number of characters excluding the termination char
    unsigned int _capacity;
    char* _s;
};

/**
 * Creates a pointer to a string object
 */
struct string *string_create();

/**
 * Creates a pointer to a string from a pointer to a char list by COPY.
 */
struct string *string_create_from(const char *const src);

void string_destroy(struct string *str);

struct string *_string_create_allocate(const unsigned int str_size);

unsigned int _string_calc_capacity(unsigned int size);

/**
 * Growing/shrinking the internal array if necessary
 */
_STRING_ERROR _string_realloc(struct string * const str, const unsigned int new_str_size);

void string_copy(struct string *const dst, const char *const src);

// deletes the elements of this string
void string_empty(struct string *const str);

void string_append_chrlst(struct string *const dst, const char *const src);

// append ch at the end of dst
void string_append_chr(struct string *const dst, const char ch);

char string_at(const struct string *const str, const unsigned int i);

char string_pop(struct string *const str);

#endif