#include <stdlib.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <scanner_utils/regex_tree.h>

static void test_scanner_regex_tree_create(void **state) {
    struct scanner_regex_tree_node *n = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, 'a');

    assert_int_equal(n->type, SCANNER_REGEX_TREE_NODE_LITERAL);
    assert_int_equal(n->literal, 'a');
    assert_int_equal(n->children_n, 0);
    assert_int_equal(n->_children_capacity, 5);
    
    assert_null(n->parent);
    for (unsigned short i = 0; i < n->_children_capacity; i++) {
        assert_null(n->children[i]);
    } 

    scanner_regex_tree_destroy(n);
}

static void test_scanner_regex_tree_add_child(void **state) {
    // root
    struct scanner_regex_tree_node *root = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, 'a');

    // root -> 6 * '|'
    struct scanner_regex_tree_node *c[6];
    for (int i = 0; i < 6; i++) {
        c[i] = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ALT, '|');
        scanner_regex_tree_add_child(root, c[i]);
    }

    // root -> 3rd '|' -> 3 * '.'
    struct scanner_regex_tree_node *c2[3];
    for (int i = 0; i < 3; i++) {
        c2[i] = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.');
        scanner_regex_tree_add_child(c[2], c2[i]);
    }

    // root
    assert_int_equal(root->type, SCANNER_REGEX_TREE_NODE_LITERAL);
    assert_int_equal(root->literal, 'a');
    assert_int_equal(root->children_n, 6);
    assert_int_equal(root->_children_capacity, 10);
    assert_null(root->parent);

    for (int i = 0; i < root->children_n; i++) {
        assert_non_null(c[i]);
        assert_int_equal(c[i]->type, SCANNER_REGEX_TREE_NODE_ALT);
        assert_int_equal(c[i]->literal, '|');
        assert_int_equal(c[i]->parent, root);

        if (i == 2) {
            assert_int_equal(c[i]->children_n, 3);
            assert_int_equal(c[i]->_children_capacity, 5);

            for (int j = 0; j < c[i]->children_n; j++) {
                assert_non_null(c[i]->children[j]);
                assert_int_equal(c[i]->children[j]->type, SCANNER_REGEX_TREE_NODE_CONC);
                assert_int_equal(c[i]->children[j]->literal, '.');
                assert_int_equal(c[i]->children[j]->children_n, 0);
                assert_int_equal(c[i]->children[j]->_children_capacity, 5);
                assert_int_equal(c[i]->children[j]->parent, c[i]);
            }
        } else {
            assert_int_equal(c[i]->children_n, 0);
            assert_int_equal(c[i]->_children_capacity, 5);
        }
    }

    scanner_regex_tree_destroy(root);
}

void test_scanner_regex_tree_minimize(void **state) {
    // Before minimization:
    // |
    //   .
    //     *
    //       |
    //         .
    //           a
    //           b
    //         .
    //           b
    struct scanner_regex_tree_node *root = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ALT, '|');
    scanner_regex_tree_add_child(root, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.'));
    scanner_regex_tree_add_child(root->children[0], scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CLOS, '*'));
    scanner_regex_tree_add_child(root->children[0]->children[0], scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ALT, '|'));
    struct scanner_regex_tree_node *last_alt = root->children[0]->children[0]->children[0];
    scanner_regex_tree_add_child(last_alt, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.'));
    scanner_regex_tree_add_child(last_alt->children[0], scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, 'a'));
    scanner_regex_tree_add_child(last_alt->children[0], scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, 'b'));
    scanner_regex_tree_add_child(last_alt, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.'));
    scanner_regex_tree_add_child(last_alt->children[1], scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, 'b'));

    // Minimize regex evaluation tree
    scanner_regex_tree_minimize(&root);

    // After minimization:
    // *
    //   |
    //     .
    //       a
    //       b
    //     b
    struct scanner_regex_tree_node *current;

    // *
    assert_non_null(root);
    assert_int_equal(root->type, SCANNER_REGEX_TREE_NODE_CLOS);
    assert_int_equal(root->literal, '*');
    assert_int_equal(root->children_n, 1);

    //   |
    current = root->children[0];
    assert_non_null(current);
    assert_int_equal(current->parent, root);
    assert_int_equal(current->type, SCANNER_REGEX_TREE_NODE_ALT);
    assert_int_equal(current->literal, '|');
    assert_int_equal(current->children_n, 2);
   
    //     .
    assert_non_null(current->children[0]);
    assert_int_equal(current, current->children[0]->parent);
    current = current->children[0];
    assert_int_equal(current->type, SCANNER_REGEX_TREE_NODE_CONC);
    assert_int_equal(current->literal, '.');
    assert_int_equal(current->children_n, 2);

    //       a
    assert_int_equal(current, current->children[0]->parent);
    current = current->children[0];
    assert_int_equal(current->type, SCANNER_REGEX_TREE_NODE_LITERAL);
    assert_int_equal(current->literal, 'a');
    assert_int_equal(current->children_n, 0);

    //       b
    assert_int_equal(current->parent, current->parent->children[1]->parent);
    current = current->parent->children[1];
    assert_int_equal(current->type, SCANNER_REGEX_TREE_NODE_LITERAL);
    assert_int_equal(current->literal, 'b');
    assert_int_equal(current->children_n, 0);

    //     b
    assert_int_equal(current->parent->parent, current->parent->parent->children[1]->parent);
    current = current->parent->parent->children[1];
    assert_int_equal(current->type, SCANNER_REGEX_TREE_NODE_LITERAL);
    assert_int_equal(current->literal, 'b');
    assert_int_equal(current->children_n, 0);

    // DECONSTRUCT
    scanner_regex_tree_destroy(root);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scanner_regex_tree_create),
        cmocka_unit_test(test_scanner_regex_tree_add_child),
        cmocka_unit_test(test_scanner_regex_tree_minimize)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}