#ifndef SCANNER_REGEX_ANALYZER_H
#define SCANNER_REGEX_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <cutils/string.h>
#include <cutils/arrayi.h>

#define SCANNER_REGEX_TOKEN_LITERAL            0 // ab
#define SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN   1 // (
#define SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE  2 // )
#define SCANNER_REGEX_TOKEN_BRACKETS_OPEN      3 // [
#define SCANNER_REGEX_TOKEN_BRACKETS_CLOSE     4 // ]
#define SCANNER_REGEX_TOKEN_OP_ALTERATION      5 // |
#define SCANNER_REGEX_TOKEN_OP_CLOSURE         6 // *
#define SCANNER_REGEX_TOKEN_OP_POSCLOSURE      7 // +
#define SCANNER_REGEX_TOKEN_OP_ZERO_OR_ONE     8 // ?
#define SCANNER_REGEX_TOKEN_RANGE              9 // [0-9]
#define SCANNER_REGEX_TOKEN_WILDCARD          10 // .
#define SCANNER_REGEX_TOKEN_EMPTYSTR          11 // \e
#define SCANNER_REGEX_TOKEN_ANYWHITESPACE     12 // \s

const char* scanner_regex_get_token_name(const unsigned int token);

typedef int SCANNER_REGEX_STATUS;
SCANNER_REGEX_STATUS scanner_regex_analyze(const struct cutils_string * const rgx,
                                   struct cutils_arrayi *tokens,
                                   struct cutils_string ***lexemes,
                                   unsigned int *lexemes_capacity,
                                   unsigned int *lexemes_n);

#endif // SCANNER_REGEX_ANALYZER_H