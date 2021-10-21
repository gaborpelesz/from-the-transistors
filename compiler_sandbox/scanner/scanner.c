/**
 * developer: gaborpelesz@gmail.com
 * date: 21/10/2021
 * 
 * Development approach:
 *  Create a wireframe of the scanner.
 *  This is only how I would prefer to implement it
 *  and maybe not the most efficient or optimized way to do it.
 *  Later on we can make any changes and the scanner
 *  won't be that big of a component either.
 * 
 */

// UTIL DEFINITIONS
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

// -------- struct array ---------
struct array {}; // 'void' array so it is type-agnostic???
// -------- struct array ---------

// -------- struct stack ---------
struct stack_short {
    // we differentiate allocated size and the stack's size
    // this way when we empty the stack, we don't necessarily
    // need to reallocate the memory, we just reset the stack_size
    //
    // allocated size can also be very dynamic, it could smartly preallocate
    // more at once so it doesn't need to do this memory operation on
    // every push and pop
    unsigned int stack_size;
    unsigned int _allocated_size;
    short* _values;
};

void stack_short_init(struct stack_short *const stack) {
    // stack->values = malloc();
    // _allocated_size = ~10?
    // stack_size = 0;
}

void stack_short_destroy(struct stack_short *const stack) {
    // delete stack->values;
}

void stack_short_push(struct stack_short *const stack, short val) {
    // push value at the end of the stack
}

short stack_short_pop(struct stack_short *const stack) {
    // remove last element from stack and return it
    return 0;
}

void stack_short_empty(struct stack_short *const stack) {
    // empty the stack
    // maybe don't allocate and deallocate, 
    // just delete the values and reset the size
}
// -------- struct stack ---------

// -------- struct string ---------
struct string {
    unsigned int size;
    char* s;
};

void string_init(struct string *const str) {
    // init should add termination char
    // termination char should not be included in size
    // ...
}

void string_destroy(struct string *const str) {
    // ...
}

void string_copy(struct string *const dst, const char *const src) {
    // ...
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

// -------- struct string ---------

// TODO better order of parameter table
void scanner_skeleton_original(const struct string *const text, BOOL *accepting_states, short **transition_table) {
    // FA states:
    #define FA_STATE_INITIAL 0
    #define FA_STATE_ERROR -1
    #define FA_STATE_BAD -2 // this gets pushed to the stack at first

    int i = 0;

    // Skeleton scanner FA table-driven simulation
    // original algorithm, can be optimized a lot
    while (i < text->size) {
        short current_state = 0;

        // for lexeme we could make this dynamic, so if it
        // somehow reaches 255, then it would allocate
        // another 255 to it. 
        //   - Also 255 could be changed to a lower number in
        //     this case so we don't necessarily store too much.
        //   - Allocation can also be exponential or something
        //       - first allocate 20 -> 50 -> 100
        struct string lexeme;
        string_init(&lexeme);

        struct stack_short state_stack;
        stack_short_init(&state_stack);
        stack_short_push(&state_stack, -1); // push "bad"

        // forward pass until either we do not reach
        //  - a state from where we can't go further, or
        //  - end of text
        while (current_state != FA_STATE_ERROR || i < text->size) {
            string_add_char(&lexeme, string_at(&text, i));

            if (accepting_states[current_state]) {
                stack_short_empty(&state_stack);
            }

            stack_short_push(&state_stack, current_state);

            // very simple dummy usage on transition table, might be changed later...
            current_state = transition_table[current_state][string_at(&text, i)];

            i = i+1; // next char
        }

        // rollback loop -> trying to find longest prefix that is accepting
        while (current_state != FA_STATE_BAD && !accepting_states[current_state]) {
            current_state = stack_short_pop(&state_stack);
            string_pop(&lexeme); // we don't need to save output
            i = i-1; // previous char
        }

        if (accepting_states[current_state]) {
            // TODO create a pair class
            // TODO create an array class with 'void'
            //      so it will maybe able to capture any type?!
            //      is this how they do "templating" in C???

            // implement in this if:
            // ---------------------
            // tuple = classify_lexeme(current_state), lexeme
            // tokens.append(tuple)
        }
    }

    /**
     * 
     * inputs: 
     *  - text -> text to analyze
     *  - transition_table (not column minimized)
     *  - accepting_states -> Boolean array (True -> accepting; False -> not accepting)
     *  - classify_lexeme -> word classification and token assignment based on accepting state
     * 
     * ORIGINAL ALGORITHM - Python pseudo
     * -------------------------------------------------------------
     *  NextWord() #obviously should be switched with a loop
     *  current_state = 0
     *  current_lexeme = ""
     *  stack = [-1] # -1 means "bad"
     * 
     *  i = 0
     * 
     *  while current_state != error_state:
     *      lexeme = lexeme + text[i]
     * 
     *      if accepting_states[current_state]:
     *          stack = []
     * 
     *      stack.append(current_state)
     * 
     *      current_state = transition_table(current_state, text[i])
     * 
     *      i = i+1
     * 
     *  while current_state != -1 and not accepting_states[current_state]:
     *      current_state = stack.pop() #removes and returns last element
     *      lexeme = lexeme[:-1] #removing last element
     * 
     *      i = i-1
     * 
     *  if accepting_states[current_state]:
     *      tokens.append([
     *          classify_lexeme(current_state), lexeme
     *      ])
     *  else:
     *      raise Exception(f"Error happened when analyzing '{lexeme}'")
     * 
     *  -------------------------------------------------------------
     */
}

void scanner_skeleton_custom() {
    /**
     * Similar but improved algorithm... WORK IN PROGRESS
     * Improvements:
     *  - precise line and column tracking of lexemes
     *  - better design for rollback (don't need to iterate backwards)
     * 
     * SLIGHTLY IMRPOVED - Python pseudo
     * ----------------------------------
     *  current_state = 0 #starting state
     *  current_lexeme = "" #init lexeme with empty string
     
     *  line_counter = 0 #number of lines
     *  line_counter_index = 0 #saves the current index of the '\n' character, thus
     *                         #when rollback happens, we doesn't count lines 2 times
     * 
     *  for i in range( length( text ) ):
     *      
     * 
     *      current_state = transition_table( text[i], current_state )
     *      current_lexeme = current_lexeme + text[i]  #string concat
     *  
     *      if accepting_states[current_state]:
     *          saved_accepting_state = current_state
     *          saved_accepting_lexeme = current_lexeme
     *          saved_last_char_index = i
     * 
     *      elif current_state == error_state: #ROLLBACK
     *          tokens.append([
     *                  classify_lexeme(saved_accepting_lexeme),
     *                  saved_line_counter,
     *                  saved_lexeme_column_start, #first char index in the column
     *                  saved_accepting_lexeme
     *              ])
     * 
     *          i = saved_last_char_index + 1 #rollback
     *          saved_lexeme_column_start = saved_last_char_index + 1
     *          saved_line_counter = line_counter
     * -----------------------------------
     * 
     * -
     * |
     * v
     * this is a character sequence
     *
     *
     */
}