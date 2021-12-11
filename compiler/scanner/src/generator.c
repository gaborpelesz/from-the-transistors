#include <scanner_utils/regex_scanner.h>

// for testing
int main(void) {
    // REGEX TO BE TESTED
    //struct cutils_string *rgx = cutils_string_create_from("(ab|b)*");
    struct cutils_string *rgx = cutils_string_create_from("(a(b\\\"|b+\\\"))?a*[0-9].v\\n");

    struct cutils_arrayi *tokens = cutils_arrayi_create();

    struct cutils_string **lexemes;
    lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int lexemes_size = 10;
    unsigned int lexemes_n = 0;

    int status = scanner_regex_analyze(rgx, tokens, &lexemes, &lexemes_size, &lexemes_n);
    printf("status: %d\n", status);

    printf("Tokens: %d, Lexemes: %d\n", tokens->size, lexemes_n);

    // print lexemes with tokens
    for (int i = 0; i < tokens->size; i++) {
        int ti = cutils_arrayi_at(tokens, i);
        printf("('%s' -> '%s')\n", lexemes[i]->_s, scanner_regex_get_token_name(ti));
    }

    // deconstruct
    cutils_string_destroy(rgx);

    for (unsigned int i = 0; i < lexemes_n; i++) {
        cutils_string_destroy(lexemes[i]);
    }
    free(lexemes);
    // ~ deconstruct

    return 0;
}