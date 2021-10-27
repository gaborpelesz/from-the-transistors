#ifndef CSTRING_H
#define CSTRING_H

#define STRING_INITIAL_CAPACITY 12
#define STRING_GROWTH_FACTOR 2
#define STRING_SHRINK_FACTOR 2

#define STRING_TERMINATION_SIZE 1

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

void string_destroy(struct string *const str);

struct string *_string_create_allocate(const unsigned int cap);

/**
 * Growing the internal array if necessary
 */
void _string_realloc_growth(struct string * const str, const unsigned int cap);

/**
 * Only reallocates the internal array if it reached the shrink limit.
 * 
 * Returns:
 *  - int: if reallocation occured
 */
int _string_realloc_shrink(struct string * const str);


void string_copy(struct string *const dst, const char *const src);

// deletes the elements of this string
void string_empty(struct string *const str);

void string_append_chrlst(struct string *const dst, const char *const src);

// append ch at the end of dst
void string_append_chr(struct string *const dst, const char ch);

char string_at(const struct string *const str, const unsigned int i);

char string_pop(struct string *const str);

#endif