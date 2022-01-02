#ifndef SCANNER_REGEX_TREE_H_
#define SCANNER_REGEX_TREE_H_

enum SCANNER_REGEX_TREE_NODE_TYPE {
    SCANNER_REGEX_TREE_NODE_ALT,      // `|`: alteration
    SCANNER_REGEX_TREE_NODE_CONC,     // `ab`: concatenation
    SCANNER_REGEX_TREE_NODE_LITERAL,  // `a`: any literal
    SCANNER_REGEX_TREE_NODE_CLOS,     // `*`: closure
                                      // `+`: positive closure
                                      // `?`: zero or one occurence
    SCANNER_REGEX_TREE_NODE_RANGE,    // `[0-9]`: range
    SCANNER_REGEX_TREE_NODE_WILDCARD, // `.`: any character
    SCANNER_REGEX_TREE_NODE_EMPTYLIT, // `\e`: empty literal
    SCANNER_REGEX_TREE_NODE_ANYWHITE  // `\s`: any whitespace character
};

struct scanner_regex_tree_node {
    enum SCANNER_REGEX_TREE_NODE_TYPE type;
    char literal;
    struct scanner_regex_tree_node *parent;
    struct scanner_regex_tree_node **children; // TODO it would be nice to have a void* dynamic array
};

/**
 * 
 */
struct scanner_regex_tree_node *scanner_regex_tree_create(
                                    const enum SCANNER_REGEX_TREE_NODE_TYPE type,
                                    char literal);

/**
 * 
 */
void scanner_regex_tree_destroy(struct scanner_regex_tree_node *root);

/**
 * 
 */
void scanner_regex_tree_minimize(struct scanner_regex_tree_node *node);

/**
 * 
 */
void scanner_regex_tree_add_child(struct scanner_regex_tree_node * const root,
                                  struct scanner_regex_tree_node * child);

#endif // SCANNER_REGEX_TREE_H_