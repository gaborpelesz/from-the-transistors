#include <stdio.h>
#include <stdlib.h>
#include <cutils/string.h>
#include <cutils/arrayi.h>

// What do we need to generate?
//  - FA states: #define FA_STATE_INITIAL 0; etc...
//    - list of accepting states: BOOL is_accepting_state[NUM_OF_STATES];
//  - transition table: short transition_table[NUM_OF_STATES][256];
//    - with the columns for the ASCII characters
//  - array to match token name indexes with states
//    - unsigned int classify_lexeme[NUM_OF_STATES] -> returns index for the token names array;
//  - tokens list with names
//    - this is the easiest I guess

#define SCANNER_REGEX_TOKEN_LITERAL            0 // ab
#define SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN   1 // (
#define SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE  2 // )
#define SCANNER_REGEX_TOKEN_BRACKETS_OPEN      3 // [
#define SCANNER_REGEX_TOKEN_BRACKETS_CLOSE     4 // ]
#define SCANNER_REGEX_TOKEN_OP_ALTERATION      5 // |
#define SCANNER_REGEX_TOKEN_OP_CLOSURE         6 // *
#define SCANNER_REGEX_TOKEN_OP_POSCLOSURE      7 // +
#define SCANNER_REGEX_TOKEN_OP_QUESTION_MARK   8 // ?
#define SCANNER_REGEX_TOKEN_RANGE              9 // [0-9]
#define SCANNER_REGEX_TOKEN_WILDCARD          10 // .
#define SCANNER_REGEX_TOKEN_EMPTYSTR          11 // \e
#define SCANNER_REGEX_TOKEN_ANYWHITESPACE     12 // \s

static const struct cutils_string* regex_token_map[13];

void append_to_string_array(struct cutils_string ***strarr, // pointer to an array of cutils_string
                            unsigned int *capacity,
                            unsigned int *size,
                            char *newstr) {
    // handle growing array's reallocation
    if (*size + 1 >= *capacity) {
        *capacity *= 2;
        (*strarr) = realloc((*strarr), sizeof(**strarr) * *capacity);
    }

    (*strarr)[*size] = cutils_string_create_from(newstr);

    *size += 1;
}

void append_literal_ifany(struct cutils_string *literal,
                          struct cutils_arrayi *tokens,
                          struct cutils_string ***lexemes,
                          unsigned int *lexemes_capacity,
                          unsigned int *lexemes_size) {
    if (literal->size > 0) {
        cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_LITERAL);
        append_to_string_array(lexemes, lexemes_capacity, lexemes_size, literal->_s);
        cutils_string_empty(literal);   
    }
}

typedef int REGEX_SCANNER_STATUS;
REGEX_SCANNER_STATUS regex_scanner(const struct cutils_string * const rgx,
                                   struct cutils_arrayi *tokens,
                                   struct cutils_string ***lexemes,
                                   unsigned int *lexemes_capacity,
                                   unsigned int *lexemes_n) {

    printf("trying regex: %s\n", rgx->_s);

    int retval = 0;

    int parenthesis_cnt = 0;
    int b_is_bracket_open = 0;  
    int b_is_quote_literal = 0; 

    struct cutils_string *literal = cutils_string_create();

    for (unsigned int i = 0; i < rgx->size; i++) {
        char current_char = cutils_string_at(rgx, i);

        // dealing with possible special characters
        if (current_char == '\\') {
            // check if peak is possible
            if (i+1 == rgx->size) {
                retval = 1;
                break;
            }

            // peak is possible,
            char peaked = cutils_string_at(rgx, i+1);

            // now check if we are dealing with quotes
            if (peaked == '"') {
                // escape quote detected -> considering quote as a literal
                cutils_string_append_chr(literal, '"');
                i = i+1; // we've already dealt with the next char
            }
            // backslash is always a special char even inside quoted literals
            // that means to use backslash as a literal, you have to escape it
            else if (peaked == '\\') {
                cutils_string_append_chr(literal, '\\'); 
                i = i+1; // we've already dealt with the next char
            }
            // only consider other special characters
            // if they aren't inside quotes (i.e., aren't taken as literals)
            else if (!b_is_quote_literal) {
                // handling a special character outside of a literal

                // any whitespace
                if (peaked == 's') {
                    append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                    cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_ANYWHITESPACE);
                    append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "\\s");
                }
                // tab is just a literal
                else if (peaked == 't') {
                    cutils_string_append_chr(literal, '\t');

                }
                // carriage return is just a literal
                else if (peaked == 'r') {
                    cutils_string_append_chr(literal, '\r');

                }
                // newline is just a literal
                else if (peaked == 'n') {
                    cutils_string_append_chr(literal, '\n');
                }
                // empty string
                else if (peaked == 'e') {
                    append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                    cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_EMPTYSTR);
                    append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "\\e");
                }
            }
            // else taking backslash as a literal without considering peaked
            else {
                cutils_string_append_chr(literal, '\\');
            }
        }
        else if (current_char == '"') {
            b_is_quote_literal = !b_is_quote_literal;

            //                         current_char __
            //                                        | 
            //                                        v
            // if a literal ended: "this was a literal"
            // 
            // it still doesn't need to be handled specially
            // it will be implicitly handled because we didn't include
            // the opening and closing quotes inside the literal
            // so we can just continue appending to it until we
            // stumble upon a special character
        }
        else if (!b_is_quote_literal) {
            // this also handles ']'
            if (current_char == '[') {
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);


                // ensure that there are enough characters left for a range definition
                // a range is always 5 characters: [0-9] -> [<from>-<to>]
                if (i+4 >= rgx->size) {
                    printf("Unfinished range definition in %s -> '%s'\n", rgx->_s, rgx->_s + (rgx->size - i));
                    retval = 1;
                    break;
                }
                // checking if the range definition is correct
                else if (cutils_string_at(rgx, i+2) != '-' || cutils_string_at(rgx, i+4) != ']') {
                    printf("Incorrect range definition in %s -> '%s'\n", rgx->_s, rgx->_s + 4);
                    retval = 1;
                    break;
                }

                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_RANGE);

                char range[] = {'[', cutils_string_at(rgx, i+1), '-', cutils_string_at(rgx, i+3), ']', '\0'};
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, range);

                i += 4;
            }
            // already handled by the opening bracket
            // so it is unexpected to stumble upon a closing bracket
            else if (current_char == ']') {
                printf("Unexpected closing bracket in %s at %d. char", rgx->_s, i);
                retval = 1;
                break;
            }

            else if (current_char == '(') {
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                parenthesis_cnt += 1;
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "(");
            }
            else if (current_char == ')') {
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                parenthesis_cnt -= 1;
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, ")");
            }
            
            else if (current_char == '|') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_ALTERATION);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "|");
            }

            else if (current_char == '*') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_CLOSURE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "*");
            }

            else if (current_char == '+') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_POSCLOSURE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "+");
            }

            else if (current_char == '?') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_QUESTION_MARK);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "?");
            }

            else if (current_char == '.') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_WILDCARD);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, ".");
            }

            // no special character, append to the literal
            else {
                cutils_string_append_chr(literal, current_char);
            }
        }
        else { // current char is inside quotes -> handling it as literal
            cutils_string_append_chr(literal, current_char);
        }
    }

    // scanning...
    // ... add characters to a lexeme until we find a special character. The token for the lexeme: concatenated_chars
    // ... every string that's inside two quotes should be a concatenated_chars immediately
    // ... if the string inside two quotes contains a quote character it should be escaped with a backslash "\"this\"" matches -> "this"

    if (retval == 0) {
        // if there was no error, we might have some literals left to append
        append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);

        if (parenthesis_cnt != 0) {
            if (parenthesis_cnt < 0) {
                printf("Unmatched parenthesis inside regex: too much closing ')'\n");
            } else { // (parenthesis_cnt < 0)
                printf("Unmatched parenthesis inside regex: too much opening '('\n");
            }
            retval = 1;
        }
        if (b_is_bracket_open != 0) {
            printf("Unmatched brackets inside regex: %s\n", rgx->_s);
            retval = 1;
        }
        if (b_is_quote_literal != 0) {
            printf("Unmatched quotes inside regex: %s\n", rgx->_s);
            retval = 1;
        }
    }

    cutils_string_destroy(literal);

    return retval;
}

// for testing
int main(void) {
    regex_token_map[0] = cutils_string_create_from("LITERAL");
    regex_token_map[1] = cutils_string_create_from("PARENTHESIS_OPEN");
    regex_token_map[2] = cutils_string_create_from("PARENTHESIS_CLOSE");
    regex_token_map[3] = cutils_string_create_from("BRACKET_OPEN");
    regex_token_map[4] = cutils_string_create_from("BRACKET_CLOSE");
    regex_token_map[5] = cutils_string_create_from("ALTERATION");
    regex_token_map[6] = cutils_string_create_from("CLOSURE");
    regex_token_map[7] = cutils_string_create_from("POS_CLOSURE");
    regex_token_map[8] = cutils_string_create_from("OP_ONEORMORE");
    regex_token_map[9] = cutils_string_create_from("RANGE");
    regex_token_map[10] = cutils_string_create_from("WILDCARD");
    regex_token_map[11] = cutils_string_create_from("EMPTYSTR");
    regex_token_map[12] = cutils_string_create_from("ANYWHITESPACE");

    struct cutils_string *rgx = cutils_string_create_from("(ab|b)*");
    struct cutils_arrayi *tokens = cutils_arrayi_create();

    struct cutils_string **lexemes;
    lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int lexemes_size = 10;
    unsigned int lexemes_n = 0;

    int status = regex_scanner(rgx, tokens, &lexemes, &lexemes_size, &lexemes_n);
    printf("status: %d\n", status);

    printf("Tokens: %d, Lexemes: %d\n", tokens->size, lexemes_n);

    // print lexemes with tokens
    for (int i = 0; i < tokens->size; i++) {
        int ti = cutils_arrayi_at(tokens, i);
        printf("('%s' -> '%s')\n", lexemes[i]->_s, regex_token_map[ti]->_s);
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