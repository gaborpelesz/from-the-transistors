#include <math.h>

typedef struct tree_graph {
    unsigned int n_node;
    tree_graph_node* nodes;
} tree_graph;

typedef struct tree_graph_node {
    char value;
    tree_graph_node* parent;
    tree_graph_node* child_left;
    tree_graph_node* child_right;
} tree_graph_node;

unsigned int tree_graph_level_count(tree_graph *g) {
    return ceil(log2(g->n_node));
}