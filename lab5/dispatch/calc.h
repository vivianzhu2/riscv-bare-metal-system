#ifndef CALC_H
#define CALC_H

// This typedef declares the new type `binary_fn_t`
// as a pointer to a function that takes two int
// parameters and returns an int
typedef int (*binary_fn_t)(int, int);

// initialize the calculator module
void calc_init(void);

// add a new operator to calculator module
void calc_add_operator(char ch, binary_fn_t fn);

// evaluate string expression and return result.
// string is expected to be single binary operator
// expressed in exactly the form <num><op><num>,
// no spaces or other characters.
int calc_evaluate(const char *expr);

#endif
