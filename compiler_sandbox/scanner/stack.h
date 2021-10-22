#ifndef STACK_H
#define STACK_H

struct stack_short {
    // we differentiate allocated size and the stack's size
    // this way when we empty the stack, we don't necessarily
    // need to reallocate the memory, we just reset the stack_size
    //
    // allocated size can also be very dynamic, it could smartly preallocate
    // more at once so it doesn't need to do this memory operation on
    // every push and pop
    unsigned int stack_size;
    unsigned int _allocated_size; // a.k.a. capacity
    short* _values;
};

struct stack_short *stack_short_create() {
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

#endif