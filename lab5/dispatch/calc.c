#include "calc.h"
#include "printf.h"
#include "strings.h"

#define MAX_OPERATORS 10


// the declaration below is for a global array of operators
// each array element is a struct with fields for the op
// character and function pointer to perform operation.
// The array is initially empty (noperators = 0)
// calc_add_operator adds a new operator to
// the end of the array.
// The calculator can use this array as a "dispatch table".
// It searches the table to find an entry matching the
// operator character and dispatches to the associated
// function to perform the operation. Neat!
static struct {
    char op;
    binary_fn_t fn;
} operators[MAX_OPERATORS];

static int noperators = 0;


static int add(int a, int b) {
    return a + b;
}

static int subtract(int a, int b) {
    return a - b;
}

static int multiply(int a, int b) {
    return a * b;
}

static int apply_operation(char op, int a, int b) {
    if (op == '+') {
        return add(a, b);
    } else if (op == '-') {
        return subtract(a, b);
    } else if (op == '*') {
        return multiply(a, b);
    }
    // TODO 1: add code for divide operator

    // TODO 2: change apply_operator to instead use operators array as a
    // dispatch table, no need for explicit cases any more!
    printf("calc_evaluate: no such operator '%c'\n", op);
    return 0;
}

void calc_init(void) {
    calc_add_operator('+', add);
    calc_add_operator('-', subtract);
    calc_add_operator('*', multiply);
    // TODO 3: add divide operator to dispatch table
}

void calc_add_operator(char ch, binary_fn_t fn) {
    if (noperators < MAX_OPERATORS) { // if room in table
        operators[noperators].op = ch;
        operators[noperators].fn = fn;
        noperators++;
    }
}

int calc_evaluate(const char *expr) {
    // string expected to be in form <num><op><num>
    // use strtonum to parse, those modules you wrote are so handy!
    int operand1 = strtonum(expr, &expr);
    char op = *expr++;
    int operand2 = strtonum(expr, NULL);
    return apply_operation(op, operand1, operand2);
}
