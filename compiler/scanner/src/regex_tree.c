#include <scanner_utils/regex_tree.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif

#define _SCANNER_REGEX_TREE_INITIAL_CAP 5

struct scanner_regex_tree_node *scanner_regex_tree_create(
                                    const enum SCANNER_REGEX_TREE_NODE_TYPE type,
                                    char literal) {
    // ... 
    struct scanner_regex_tree_node *n = malloc(sizeof(struct scanner_regex_tree_node));
    n->type = type;
    n->literal = literal;
    n->children_n = 0;
    n->_children_capacity = _SCANNER_REGEX_TREE_INITIAL_CAP;
    n->children = calloc(n->_children_capacity, sizeof(struct scanner_regex_tree_node*));
    n->parent = NULL;

    return n;
}

static void _tree_recursive_destroy(struct scanner_regex_tree_node *n) {
    if (n->children_n == 0) {
        free(n->children);
        return;
    }

    // destroy all elements with recursive post-order traversal
    for (unsigned short i = 0; i < n->children_n; i++) {
        _tree_recursive_destroy(n->children[i]);
        free(n->children[i]);
    }

    free(n->children);
}

void scanner_regex_tree_destroy(struct scanner_regex_tree_node *root) {
    _tree_recursive_destroy(root);
    free(root);
}

void scanner_regex_tree_add_child(struct scanner_regex_tree_node * const parent,
                                  struct scanner_regex_tree_node * child) {
    // increase children array
    if (parent->children_n == parent->_children_capacity) {
        parent->_children_capacity *= 2;
        struct scanner_regex_tree_node **new_children = realloc(parent->children, sizeof(*(parent->children)) * parent->_children_capacity);

        if (new_children == NULL) {
            printf("REGEX_TREE_ADD_CHILD: unable to increase capacity of children array. (new capacity: %d)\n", parent->_children_capacity);
            exit(EXIT_FAILURE);
        }

        parent->children = new_children;
    }

    child->parent = parent;
    parent->children[parent->children_n] = child;
    parent->children_n++;
}
                                  
void scanner_regex_tree_minimize(struct scanner_regex_tree_node **p_root) {
    struct scanner_regex_tree_node *root = *p_root;

    if (root->children_n == 0) {
        return;
    }

    for (unsigned short i = 0; i < root->children_n; i++) {
        scanner_regex_tree_minimize(root->children + i);
    }

    if (root->children_n == 1 &&
        (root->type == SCANNER_REGEX_TREE_NODE_ALT ||
         root->type == SCANNER_REGEX_TREE_NODE_CONC)) {
            struct scanner_regex_tree_node *old_root = root;

            root = root->children[0];
            root->parent = old_root->parent;

            free(old_root->children);
            free(old_root);
    }

    *p_root = root;
}

static void _tree_recursive_print(const struct scanner_regex_tree_node * const root, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("%c\n", root->literal);
    for (unsigned short i = 0; i < root->children_n; i++) {
        _tree_recursive_print(root->children[i], depth+1);
    }
}

void scanner_regex_tree_print(const struct scanner_regex_tree_node * const root) {
    _tree_recursive_print(root, 0);
}