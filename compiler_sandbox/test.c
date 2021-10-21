#include <stdio.h>
#include <math.h>

int main(void) {
    int const j = 15;
    int i = 10;

    int * const ptr = &j;

    ptr = &i;
    *ptr = 15;

    printf("%d\n", *ptr);


    return 0;
}