#include <stdlib.h>
#include <memory.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/arrayi.h>
#include <cutils/string.h>
#include <scanner_utils/regex_analyzer.h>

static void test_scanner_regex_analyze_1(void **state) {
    // input
    const char *test_rgx = "(ab|b)*";
    struct cutils_string *rgx = cutils_string_create_from(test_rgx);

    // output
    struct cutils_arrayi *tokens = cutils_arrayi_create();
    struct cutils_string **lexemes;
        lexemes = malloc(sizeof(struct cutils_string*) * 10);
        unsigned int lexemes_capacity = 10;
        unsigned int lexemes_size = 0;

    // running regex analyzer
    int status = scanner_regex_analyze(rgx, tokens, &lexemes, &lexemes_capacity, &lexemes_size);

    // assertions
    {
        assert_int_equal(tokens->size, 6);
        assert_int_equal(lexemes_size, 6);
        
        assert_string_equal(lexemes[0]->_s, "(");
        assert_string_equal(lexemes[1]->_s, "ab");
        assert_string_equal(lexemes[2]->_s, "|");
        assert_string_equal(lexemes[3]->_s, "b");
        assert_string_equal(lexemes[4]->_s, ")");
        assert_string_equal(lexemes[5]->_s, "*");
        
        assert_int_equal(cutils_arrayi_at(tokens, 0), SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN);
        assert_int_equal(cutils_arrayi_at(tokens, 1), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 2), SCANNER_REGEX_TOKEN_OP_ALTERATION);
        assert_int_equal(cutils_arrayi_at(tokens, 3), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 4), SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE);
        assert_int_equal(cutils_arrayi_at(tokens, 5), SCANNER_REGEX_TOKEN_OP_CLOSURE);

        assert_int_equal(status, 0);
    }

    // clean-up
    {
        cutils_string_destroy(rgx);
        cutils_arrayi_destroy(tokens);

        for (unsigned int i = 0; i < lexemes_size; i++) {
            cutils_string_destroy(lexemes[i]);
        }
        free(lexemes);
    }
}

static void test_scanner_regex_analyze_2(void **state) {
    // input
    const char *test_rgx = "(a\"ab\\\"|cb\"|(asdf*b?))[0-9]\\s\\t\n end+";
    struct cutils_string *rgx = cutils_string_create_from(test_rgx);

    // output
    struct cutils_arrayi *tokens = cutils_arrayi_create();
    struct cutils_string **lexemes;
        lexemes = malloc(sizeof(struct cutils_string*) * 10);
        unsigned int lexemes_capacity = 10;
        unsigned int lexemes_size = 0;

    // running regex analyzer
    int status = scanner_regex_analyze(rgx, tokens, &lexemes, &lexemes_capacity, &lexemes_size);

    // assertions
    {
        assert_int_equal(tokens->size, 14);
        assert_int_equal(lexemes_size, 14);
        
        assert_string_equal(lexemes[0]->_s, "(");
        assert_string_equal(lexemes[1]->_s, "aab\"|cb");
        assert_string_equal(lexemes[2]->_s, "|");
        assert_string_equal(lexemes[3]->_s, "(");
        assert_string_equal(lexemes[4]->_s, "asdf");
        assert_string_equal(lexemes[5]->_s, "*");
        assert_string_equal(lexemes[6]->_s, "b");
        assert_string_equal(lexemes[7]->_s, "?");
        assert_string_equal(lexemes[8]->_s, ")");
        assert_string_equal(lexemes[9]->_s, ")");
        assert_string_equal(lexemes[10]->_s, "[0-9]");
        assert_string_equal(lexemes[11]->_s, "\\s");
        assert_string_equal(lexemes[12]->_s, "\t\n end");
        assert_string_equal(lexemes[13]->_s, "+");
        
        assert_int_equal(cutils_arrayi_at(tokens, 0), SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN);
        assert_int_equal(cutils_arrayi_at(tokens, 1), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 2), SCANNER_REGEX_TOKEN_OP_ALTERATION);
        assert_int_equal(cutils_arrayi_at(tokens, 3), SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN);
        assert_int_equal(cutils_arrayi_at(tokens, 4), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 5), SCANNER_REGEX_TOKEN_OP_CLOSURE);
        assert_int_equal(cutils_arrayi_at(tokens, 6), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 7), SCANNER_REGEX_TOKEN_OP_ZERO_OR_ONE);
        assert_int_equal(cutils_arrayi_at(tokens, 8), SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE);
        assert_int_equal(cutils_arrayi_at(tokens, 9), SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE);
        assert_int_equal(cutils_arrayi_at(tokens, 10), SCANNER_REGEX_TOKEN_RANGE);
        assert_int_equal(cutils_arrayi_at(tokens, 11), SCANNER_REGEX_TOKEN_ANYWHITESPACE);
        assert_int_equal(cutils_arrayi_at(tokens, 12), SCANNER_REGEX_TOKEN_LITERAL);
        assert_int_equal(cutils_arrayi_at(tokens, 13), SCANNER_REGEX_TOKEN_OP_POSCLOSURE);

        assert_int_equal(status, 0);
    }

    // clean-up
    {
        cutils_string_destroy(rgx);
        cutils_arrayi_destroy(tokens);

        for (unsigned int i = 0; i < lexemes_size; i++) {
            cutils_string_destroy(lexemes[i]);
        }
        free(lexemes);
    }
}


static void test_scanner_regex_analyze_X(void **state) {
    // input
    const char *test_rgx = "";
    struct cutils_string *rgx = cutils_string_create_from(test_rgx);

    // output
    struct cutils_arrayi *tokens = cutils_arrayi_create();
    struct cutils_string **lexemes;
        lexemes = malloc(sizeof(struct cutils_string*) * 10);
        unsigned int lexemes_capacity = 10;
        unsigned int lexemes_size = 0;

    // running regex analyzer
    int status = scanner_regex_analyze(rgx, tokens, &lexemes, &lexemes_capacity, &lexemes_size);

    // assertions
    {
        assert_int_equal(tokens->size, 0);
        assert_int_equal(lexemes_size, 0);
        
        assert_string_equal(lexemes[0]->_s, "");
        
        assert_int_equal(cutils_arrayi_at(tokens, 0), SCANNER_REGEX_TOKEN_LITERAL);

        assert_int_equal(status, 0);
    }

    // clean-up
    {
        cutils_string_destroy(rgx);
        cutils_arrayi_destroy(tokens);

        for (unsigned int i = 0; i < lexemes_size; i++) {
            cutils_string_destroy(lexemes[i]);
        }
        free(lexemes);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scanner_regex_analyze_1),
        cmocka_unit_test(test_scanner_regex_analyze_2),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}