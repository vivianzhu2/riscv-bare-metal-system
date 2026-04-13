// File: codegen.c
// ---------------
// C examples that highlight interesting properties/transformations
// when C compiler generates assembly instructions.

// TO DO: Paste functions one by one in Compiler Explorer and review
// translation to assembly. If you hover your mouse over an assembly
// instruction, Compiler Explorer will give you more explanation of it. Neat!

// Part (a): arithmetic

// negate():  `neg` is a pseudo-instruction. Hover over `neg` to see the actual instruction.
// compare():  comparators in C evaluate to 0 or 1. What assembly instruction is being used to test equality? Hover to learn more. Change to != or < and note how assembly changes
// multiply(): change constant to see how constructed

int negate(int arg) {
    return -arg;
}

int compare(int arg) {
    return arg == 0;
}

int multiply(int arg) {
    return arg * 2;
}


// Part (b): if-else

// Branch instructions make decisions for control flow.
// Paste the function below and look at generated assembly.
// Which branch instruction is used?
// Compare the generated assembly for optimization level -Og to that
// for -O2? The optimized version avoids the branch entirely, how?
int conditional(int arg) {
    if (arg != 0) { // change comparison from 0 to 13, what changes?
        return 1;
    } else {
        return 2;
    }
}

// Part (c): loops

// The function loop() has an ordinary for loop.
// Read generated assembly to see how loop is constructed in assembly.
// Which register is used for variable `i`? for variable `result`?
// Where are the instructions for loop init? loop test? loop increment?
// How is control flow routed?
// Compare the generated assembly at optimization level -Og with
// more aggressively optimized -O2.  What has changed?
int loop(int arg, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        result += arg;
    }
    return result;
}

// The function wait_until_low() is a C version of
// "wait for button press" from lab1.
// Read generated assembly and compare levels -Og and -O2.
// At -Og, the generated assembly re-loads from address each iteration
// and all is good. At -O2, it loads only once and thereafter assumes value
// will not change. Uh-oh, a later button press will not be detected!
// The proper fix to qualify a variable that can change unpredictably
// as `volatile`.  Which variable is volatile: `state` or `PB_DATA`?
// Try changing one variable at a time and observe how it changes
// the generated assembly. Which of the two needs to be qualified
// as `volatile` and why?
void wait_until_low(void) {
    unsigned int *PB_DATA  = (unsigned int *)0x02000040;
    unsigned int state;

    do {
        state = *PB_DATA;       // read current state from PB DATA register
    } while ((state & 1) != 0); // loop while button state is high
}

// Part (d): pointers

// The function below accesses memory using pointer/array
// notation. The code is divided into pairs of statements
// which perform a similar operation, but have subtle differences
// (such as adding a typecast or operating on pointer of
// different pointee type). How do those differences affect
// the generated assembly?

struct coord { int x, y, z;};

void pointers(int index, int *ptr, char *cptr, struct coord *pts) {
    *ptr = 55;      // compare to next line
    *cptr = 56;

    ptr[3] = 77;     // compare to next line
    cptr[3] = 78;

    *(cptr + 4) = 99;  // compare to next line
    *((int *)cptr + 4) = 100;

    *(ptr + index) = 11;  // compare to next line
    ptr[index] = 12;

    pts->y = 22;       // compare to next line
    pts[7].y = 33;
}

// This one is interesting to compare the generated assembly
// for -Og versus -O2.  The unoptimized loop body is 5
// instructions, the optimizer only 3. What was it able
// to remove/streamline?
void clear_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = 0;
    }
}

// Part (e): surprises
// If you compile what you believe is valid C code and yet when executing it
// something wacky happens ... get to the bottom of it by examining the
// generated assembly to learn what is _really_ going on.
// The assembly instructions are always the Ground Truth!

// This function compiles to always return 0 -- why?
int precedence(int arg) {
    return arg & 1 == 0;
}

// These last few exhibit "undefined behavior" (violates C language standard)
// Will not be rejected, but compiler allowed to generate whatever asm it wants
// no_return: missing return statement, what is returned?
// semi_initialized: what if local variable is initialized only on some paths, not all?
// never_initialized: local variable is never initialized, used with junk contents
//                  compare difference of what will happen compiled -Og versus -O2

int no_return(int arg) {
    int result = arg * 17;
}

int semi_initialized(int arg) {
    int result;
    if (arg > 0)
        result = 5;
    return result;
}

int never_initialized(int arg) {
    int result;
    for (int i = 0; i <= 6; i++) {
        result *= arg;
    }
    return result;
}
