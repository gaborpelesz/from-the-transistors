#ifndef STRING_H
#define STRING_H

struct string {
    unsigned int size;
    char* s;
};

struct string *string_create() {
    // init should add termination char
    // termination char should not be included in size
    // ...
    return 0;
}

void string_destroy(struct string *const str) {
    // ...
}

void string_copy(struct string *const dst, const char *const src) {
    // ...
}

// deletes the elements of this string
void string_empty() {

}

void string_add_string(struct string *const dst, const char *const src) {
    // append src at the end of dst
}

void string_add_char(struct string *const dst, const char ch) {
    // append ch at the end of dst
}

char string_at(const struct string *const str, const unsigned int i) {
    if (i < str->size) {
        return str->s[i];
    }
}

char string_pop(struct string *const str) {
    // ... todo move terminating character
    // and return: str->s[str->size];
    return '0';
}

#endif