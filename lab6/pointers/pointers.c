#include "printf.h"
#include "uart.h"

void main(void) {
    uart_init();

    // Pointer review: What is the difference between arithmetic on a `char
    // *` versus an `int *`?
    printf("\nPointer review:\n");
    printf("===============\n");
    char *cptr = "Hello";
    int iarray[] = {0, 1, 2};
    int *iptr = iarray;

    printf("cptr   = %p\n", cptr);
    printf("cptr+1 = %p\n", cptr + 1);

    printf("iptr   = %p\n", iptr);
    printf("iptr+1 = %p\n", iptr + 1);

    printf("\nSingle-dimensional arrays and pointers\n");
    printf("======================================\n");
    int f[] = { 0, 1 };
    int g[] = { 2, 3, 4 };
    printf("f = %p\n", f);
    printf("g = %p\n", g);

    int *p[2] = { f, g };
    printf("p = %p\n", p);
    printf("p[0] = %p\n", p[0]);
    printf("p[1] = %p\n", p[1]);

    printf("\nPointers to pointers example\n");
    printf("==============================\n");
    const char *tokens[2] = { "Hello", "World" };
    printf("tokens   = %p \n", tokens);
    printf("tokens+1 = %p \n", tokens + 1);
    printf("tokens[0] = %p \"%s\"\n", tokens[0], tokens[0]);
    printf("tokens[1] = %p \"%s\"\n", tokens[1], tokens[1]);
    printf("*tokens[0] = '%c'\n", *tokens[0]);
    printf("*tokens[1] = '%c'\n", *tokens[1]);
    printf("**tokens = '%c'\n", **tokens);
    printf("**(tokens + 1) = '%c'\n", **(tokens + 1));

    printf("\nMulti-dimensional arrays and pointers\n");
    printf("=====================================\n");
    // how does the 2-d array `a` compare to layout of array of pointers `p` above?
    int a[3][4] = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    // pointers b,c,d hold same address as a, but different interpretations of pointee
    int *b = &a[0][0]; // pointee = int
    int (*c)[4] = a;   // pointee = int[4]
    int (*d)[6] = (int (*)[6])a; // pointee = int[6]

    printf("a = %p\n", a );
    printf("&a[0][0] = %p  %d\n", &a[0][0], a[0][0]);
    printf("&a[0][1] = %p  %d\n", &a[0][1], a[0][1]);
    printf("&a[1][0] = %p  %d\n", &a[1][0], a[1][0]);

    printf("&a[0] = %p\n", &a[0]);
    printf("&a[1] = %p\n", &a[1]);
    printf("a   = %p\n", a);
    printf("a+1 = %p  %d\n", a + 1, **(a+1));
    printf("b   = %p\n", b);
    printf("b+1 = %p  %d\n", b + 1, *(b+1));
    printf("c   = %p\n", c);
    printf("c+1 = %p  %d\n", c + 1, **(c+1));
    printf("d   = %p\n", d);
    printf("d+1 = %p  %d\n", d + 1, **(d+1));
}
