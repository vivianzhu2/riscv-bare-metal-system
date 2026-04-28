/* File: extension.c
 * -----------------
 * ***** TODO: add your file header comment here *****
 */
#include <stdint.h>
#include "strings.h"
#include "printf.h"
#include "uart.h"
// include whatever additional headers you need

void main(void) {
    /***** TODO: Use this program as test/application/demo of extended features *****/
 }


/*
***** TODO remove below if not relevant

This test code might be helpful if doing disassemble

static int absval(int arg) {
    return arg < 0 ? -arg : arg;
}

static void test_disassemble(void) {
    const uint32_t add  = 0x00f706b3;
    const uint32_t sd   = 0x02113423;
    printf("Encoded instruction %8x disassembles to %pI\n", add, &add);
    printf("Encoded instruction %8x disassembles to %pI\n", sd, &sd);
    uint32_t *fn = (uint32_t *)absval; // disassemble instructions from absval function
    printf("absval:\n");
    for (int i = 0; i < 4; i++) {
        printf("%p:  %8x  %pI\n", &fn[i], fn[i], &fn[i]);
    }
}
*/