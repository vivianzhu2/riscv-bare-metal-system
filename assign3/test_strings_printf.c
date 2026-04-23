/* File: test_strings_printf.c
 * ---------------------------
 * ***** TODO: add your file header comment here *****
 */
#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

// Prototypes copied from printf.c to allow unit testing of helper functions
// that are not announced in public header
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);


static void test_memset(void) {
    char buf[25];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x7e, bufsize);
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0x7e);

    /***** TODO: add more tests *****/
}

static void test_strcmp(void) {
    assert(strcmp("me", "me") == 0);
    assert(strcmp("me", "you") != 0);

    /***** TODO: add more tests *****/
}

static void test_strlcat(void) {
    char buf[20];
    size_t bufsize = sizeof(buf);
    // as aid for debugging, fill contents of buffer with repeat value 0x7e
    // rather than leave contents uninitialized, this makes it easier to
    // spot later if contents have not changed in the way you intend
    memset(buf, 0x7e, bufsize); // for debug, init array contents with fixed repeat value

    buf[0] = '\0'; // null at first index makes empty string
    assert(strcmp(buf, "") == 0);
    strlcat(buf, "CS", bufsize); // append CS
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", bufsize); // append 107e
    assert(strcmp(buf, "CS107e") == 0);

    /***** TODO: add more tests *****/
}

static void test_strtonum(void) {
    assert(strtonum("67", NULL) == 67);

    const char *input = "107rocks";
    const char *rest = NULL;
    long val = strtonum(input, &rest);
    assert(val == 107);
    assert(rest == &input[3]);  // rest should now point to first non-digit character

    /***** TODO: add more tests *****/
}

static void test_helpers(void) {
    char buf[32];
    size_t bufsize = sizeof(buf);
    memset(buf, 0x7e, bufsize); // for debug, init array contents with fixed repeat value

    num_to_string(45, 10, buf);
    assert(strcmp(buf, "45") == 0);
    num_to_string(45, 16, buf);
    assert(strcmp(buf, "2d") == 0);

    assert(strcmp(decimal_string(-88), "-88") == 0);
    assert(strcmp(hex_string(0x107e), "107e") == 0);

    /***** TODO: add more tests *****/
}

static void test_snprintf(void) {
    char buf[100];
    size_t bufsize = sizeof(buf);
    memset(buf, 0x7e, bufsize); // for debug, init array contents with fixed repeat value

    // No formatting codes
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // One string formatting code
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    /***** TODO: add more tests *****/
}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    // Uncomment the call to each test function below when you have implemented
    // the functions and are ready to test them

    test_memset();
    // test_strcmp();
    // test_strlcat();
    // test_strtonum();
    // test_helpers();
    // test_snprintf();

    /***** TODO: Add more and better tests!  *****/

    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}
