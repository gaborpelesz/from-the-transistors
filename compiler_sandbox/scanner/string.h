#ifndef STRING_H
#define STRING_H

#define STRING_STRING_INITIAL_CAPACITY 12
#define STRING_GROWTH_FACTOR 2
#define STRING_SHRINK_FACTOR 3


struct string {
    unsigned int size;
    unsigned int _capacity;
    char* _s;
};

struct string *string_create() {
    return _string_create_allocate(STRING_INITIAL_CAPACITY);
}

/**
 * Creates a pointer to a string from a pointer to a char list by COPY.
 * WARNING: This function assumes that "n" does NOT include the termination character
 * 
 * Input:
 *  - src: char list
 *  -   n: number of elements in the string, excluding the termination character
 */
struct string *string_create_from(const char *const src, const unsigned int n) {
    // init should add termination char
    // termination char should not be included in size

    unsigned int allocate_capacity = (n + STRING_INITIAL_CAPACITY) / STRING_INITIAL_CAPACITY * STRING_INITIAL_CAPACITY;
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

void _string_realloc_shrink(struct string * const str) {
    unsigned int shrink_limit = str->_capacity / STRING_SHRINK_FACTOR;
    if (str->size < shrink_limit) {
        realloc(str->_s, shrink_limit);
    }
}

void string_destroy(struct string *const str) {
    free(str->_s);
    free(str);
}

void string_copy(struct string *const dst, const char *const src) {
    
}

// deletes the elements of this string
void string_empty(struct string *const str) {
    str->size = 0;
    _string_realloc_shrink(str);
}

void string_append_chrlst(struct string *const dst, const char *const src) {
    // append src at the end of dst
}

void string_append_chr(struct string *const dst, const char ch) {
    // append ch at the end of dst
}

char string_at(const struct string *const str, const unsigned int i) {
    if (i > str->size) {
        printf("Overindexing error: trying to retreive element at index '%d', while the array has only %d elements.\n", i, str->size);
        exit(1);
    }
    return str->_s[i];
}

char string_pop(struct string *const str) {
    // ... todo move terminating character
    // and return: str->s[str->size];
    return '0';
}

#endif