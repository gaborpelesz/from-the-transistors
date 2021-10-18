/**
 * I'm really lacking a good tree-graph datastructure. Build it!
 * Then it will be easier to do the actual dataflow graph building and manipulation.
 * 
 * After that, finish the algorithm... probably need to think a bit differently!
 */


// basic syntax:
//  - concatenation: ab
//  - alteration: a|b
//  - closure (0 or any no.): a*
//  - parentheses: (a|b)(a|c)*
//  - positive closure: a+ == aa*
//  - empty string special character: \e
//  - zero or one occurance: a? == (\e | a)
//  - wildcard (matches any char in alphabet): a.b == a(a|b|c|...)b
//  - shorthands:
//      - [0...9] == (0|1|2|3|4|5|6|7|8|9)
//      - [a...z] == (a|b|...|z)
//      - [A...Z] == (A|B|...|Z)
//      - [a...Z] == [a...z]|[A...Z] == (a|b|...|z|A|B|...|Z)
//      - arbitrary range: [c...L]
//                         [4...8]
//  - match any whitespace character (\t, \r, \n, spaces): a\sb -> \s special character
//
// arithmetic: (a|b)* == a*b*
// precedence: parentheses ( (a|b)* ), closure (a*), concatenation (ab), alteration (a|b)
//
// special characters: \, |, (, ), *, +, ?, ., [, ], \s, 
//  - to escape any special character put a backslash before it: \(asd\) will match "(asd)"
//      - to match a single backslash: \\ 
//
// ----------------------------
// additional, only implementing if needed:
//  - finite closure: a^3 == (a|aa|aaa)

/**
 * 
 */
typedef struct dataflow_graph {
    dataflow_node* nodes; // stored level_N -> level_1, right -> left, so it can be easily traversed backwards
                          // first node is the parent node
                          // second node is right child
                          // third node is left child
                          // fourth node is the right child of the parent's right child
                          // etc...
} dataflow_graph;

typedef struct dataflow_node {
    char value;
    dataflow_node* parent;
    dataflow_node* child_left;
    dataflow_node* child_right;
} dataflow_node;


void regex_to_nfa(char *text) {
    // steps:
    //    1. build dataflow graph
    //    2. execute graph and build NFA
    if (!(text && text[0])) {
        // TODO ... throw error
    }

    for (unsigned int i = 0; text[i] != '\0'; i++) {
        // Special character escape
        if (text[i] == '\\') {
            char nextChar = text[i+1];
            // special character look-ahead
            if (nextChar == 's') {
                // ... handle special character: \s
                i++; // next character was handled as well by this look-ahead
            } else if (nextChar == 'e') {
                i++;
            } else if (nextChar == '\\' ||
                       nextChar == '|' ||
                       nextChar == '(' ||
                       nextChar == ')' ||
                       nextChar == '*' ||
                       nextChar == '+' ||
                       nextChar == '?' ||
                       nextChar == '.' ||
                       nextChar == '[' ||
                       nextChar == ']') {
                // ... handle escape special character
                i++; // next character was handled as well
            } else {
                // TODO ... report error: unexpected use of special character '\'
            }
        }
    }
}

/**
 * This function can only handle a simple regex, i.e., the most basic one that the theory can provide.
 * Also it can't handle special characters.
 * Capabilities: |, *, ()
 * 
 * Algorithm:
 *  1., observe next item
 *  2., if item is -> normal char
 *      2.a, create dataflow node for it
 *      2.b, peak next
 *          2.b.a, if * then create node for * and connect char as its child, also increase iterator
 *      2.c, if saved node is NULL -> this was the first character
 *      2.d, else if saved node is normal char
 *          2.d.a, create node for concat and join them
 *          2.d.b, assign concat node as saved node
 *  3., else if item is -> |
 *      3.a, connect saved node as its left child
 *      3.b, peak until next | or end of string
 *      3.c, 
 */
void regex_simple_to_nfa(char *text) {
    if (!(text && text[0])) {
        // TODO ... throw error
    }



    for (unsigned int i = 0; text[i] != '\0'; i++) {

    }

}