#include <stdio.h>
#include <stdlib.h>

typedef char bool;

struct FiniteAutomaton {
    char *alphabet;
    unsigned int alphabetN;

    // array containing accepting state id's
    unsigned int *acceptingStates;
    unsigned int acceptingStatesN;

};

struct FiniteAutomaton* create_finite_automaton(char *alphabet, unsigned int alphabetN, int *acceptingStates, unsigned int acceptingStatesN) {
    // use calloc instead of malloc
    return NULL;
}

bool scan_with_fa(struct FiniteAutomaton *fa, char *text, unsigned int textN) {
    return 0;
}


int find_character_index(char *alphabet, int alphabetN, char text) {
    for (int i = 0; i < alphabetN; ++i) {
        if (alphabet[i] == text) {
            return i; // found, returning index
        }
    }

    return -1; // not found
}

int main(void) {

    int statesN = 11 + 1;
    char alphabetN = 8;
    char *alphabet = calloc(alphabetN, sizeof(char));

    alphabet[0] = 'e';
    alphabet[1] = 'h';
    alphabet[2] = 'i';
    alphabet[3] = 'l';
    alphabet[4] = 'n';
    alphabet[5] = 'o';
    alphabet[6] = 't';
    alphabet[7] = 'w';

    // initialize `state_map`
    int **state_map = calloc(statesN, sizeof(int*));
    for (int i = 0; i < statesN; ++i) {
        state_map[i] = calloc(alphabetN, sizeof(int));

        for (int j = 0; j < alphabetN; ++j) {
            state_map[i][j] = statesN - 1;
        }
    }

    //              0,1,2,3,4,5,6,7
    //              e,h,i,l,n,o,t,w
    state_map[0][4] = 1;
    state_map[0][7] = 6;

    state_map[1][0] = 2;
    state_map[1][5] = 4;

    state_map[2][7] = 3;

    state_map[4][6] = 5;

    state_map[6][1] = 7;

    state_map[7][2] = 8;

    state_map[8][3] = 9;

    state_map[9][0] = 10;

    
    int currentState = 0;
    int acceptingStatesN = 3;
    int* acceptingStates = calloc(acceptingStatesN, sizeof(int));
    acceptingStates[0] = 3;
    acceptingStates[1] = 5;
    acceptingStates[2] = 10;

    char text0[3] = {'n', 'e', 'w'};
    char text1[3] = {'n', 'o', 't'};
    char text2[5] = {'w', 'h', 'i', 'l', 'e'};

    // process text
    for (int i = 0; i < 5; ++i) {
        int char_index = find_character_index(alphabet, alphabetN, text2[i]);

        currentState = state_map[currentState][char_index];
        
        if (currentState == statesN - 1) {
            break; // we are in the error state, aborting...
        }
    }

    // find out if we are in an accepting state
    bool text_allowed = 0;
    for (int i = 0; i < acceptingStatesN; ++i) {
        if (currentState == acceptingStates[i]) {
            text_allowed = 1;
        }
    }

    if (text_allowed == 1) {
        printf("Message is allowed!\n");
    } else {
        printf("Message not allowed!\n");
    }

    // free `alphabet` and `state_map`
    free(alphabet);
    for (int i = 0; i < statesN; ++i) {
        free(state_map[i]);
    }
    free(state_map);

    return 0;
}