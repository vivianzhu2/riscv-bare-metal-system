#include "uart.h"
#include "printf.h"
#include "calc.h"

static int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

void show_expression(const char *expr) {
    printf("%s = %d\n", expr, calc_evaluate(expr));
}

void main(void) {
    uart_init();
    calc_init();

    show_expression("12+55");
    show_expression("2-9");
    show_expression("5*4");

    // uncomment after implement command dispatch
    // calc_add_operator('^', power);
    // show_expression("5^3");

    printf("All done!\n");
}
