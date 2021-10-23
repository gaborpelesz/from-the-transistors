#include <stdio.h>
#include <math.h>

int main(void) {
    void *ptr = "asdfg";

    printf("%c\n", *(char*)(ptr + 2 * sizeof(char)));


    return 0;
}