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
    char buf1[1];
    size_t bufsize1 = sizeof(buf1);
    memset(buf1, 0, bufsize1);
    for (int i = 0; i < bufsize1; i++)
        assert(buf1[i] == 0);

    // char buf2[2];
    // size_t bufsize2 = sizeof(buf2);
    // memset(buf2, , bufsize2);
    // for (int i = 0; i < bufsize2; i++)
    //     assert(buf2[i] == -1);
    char buf2[1];
    // size_t bufsize2 = 0;
    memset(buf2, 5, 0);
    for (int i = 0; i < sizeof(buf2); i++)
        assert(buf2[i] != 5);

///////
    char buf3[25];
    size_t bufsize3 = 10;

    memset(buf3, 0x7e, bufsize3);
    for (int i = 0; i < sizeof(buf3); i++)
        if (i<bufsize3) assert(buf3[i] == 0x7e)
        else    assert(buf3[i] != 0x7e);     
}

static void test_strcmp(void) {
    assert(strcmp("me", "me") == 0);
    assert(strcmp("me", "you") != 0);

    /***** TODO: add more tests *****/
    assert(strcmp("abc", "abd") < 0); // 'c' is greater than 'd'
    assert(strcmp("abd", "abc") > 0); // 'd' is less than 'c'
    assert(strcmp("", "") == 0); // two empty strings are equal
    assert(strcmp("bread", "breadboard") < 0); // "bread" is more than "breadboard" because it ends first
    assert(strcmp("breadboard", "bread") > 0); // "breadboard" is less than "bread" because "breadboard" has extra characters after "bread"
///////
    assert(strcmp("bread", "breacboard") > 0); // "bread" is more than "breadboard" because it ends first
    assert(strcmp("breacboard", "bread") < 0); // "breadboard" is less than "bread" because "breadboard" has extra characters after "bread"
   
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
    char buf2[20];
    size_t bufsize2 = sizeof(buf2)+5; 
    memset(buf2, 0x7e, bufsize2); // for debug, init array contents with fixed repeat value
     
    buf2[0]='\0';
    assert(strlcat(buf2, "hello", bufsize2)==5); // for debug, init array contents with fixed repeat value
    assert(strlcat(buf2, "world",bufsize2)== 10);
    // strlcat(buf2, "world", bufsize2); // append world
    assert(strlcat(buf2, "!lotsof!!!!!!!!!!!!!!!!!!!!!",bufsize2)== 24);
    // char teststr[19] = "Today is a good day";
    // assert(strlcat(buf2, teststr, bufsize2) == 25);
///////
    char buf3[20];
    size_t bufsize3 = sizeof(buf3)+5; 
    memset(buf3, 0x7e, bufsize3); // for debug, init array contents with fixed repeat value
     
    buf3[0]='\0';
    assert(strlcat(buf3, "", 0)==0); // for debug, init array contents with fixed repeat value
    assert(strlcat(buf3, "",1)== 0);
    //assert(strlcat(buf3, "",10)== 0);   // strlcat(buf2, "world", bufsize2); // append world
    buf3[0]='\0';
    assert(strlcat(buf3, "v", 0)==0); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';   
    assert(strlcat(buf3, "v", 1)==0); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';
    assert(strlcat(buf3, "v", 10)==1); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",0)== 0);
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",1)== 0);
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",7)== 6);
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",10)== 7);
    // //  // char teststr[19] = "Today is a good day";
    // // // assert(strlcat(buf2, teststr, bufsize2) == 25);
    char buf4[20];
    size_t bufsize4 = sizeof(buf4); 
    memset(buf4, 'e', bufsize4); // for debug, init array contents with fixed repeat value 
    buf4[19]='\0';
    char buf5[20];
    size_t bufsize5 = sizeof(buf5); 
    memset(buf5, 'e', bufsize5); // for debug, init array contents with fixed repeat value 
    buf5[19]='\0';

    assert(strlcat(buf4, "", 0)==0); // for debug, init array contents with fixed repeat value
    assert(strcmp(buf4,buf5)==0);
    assert(strlcat(buf4, "",1)== 1);
    assert(strcmp(buf4,buf5)==0);
    assert(strlcat(buf4, "",10)== 10);   // strlcat(buf2, "world", bufsize2); // append world
    assert(strcmp(buf4,buf5)==0);

    assert(strlcat(buf4, "v", 0)==1); // for debug, init array contents with fixed repeat value
    assert(strcmp(buf4,buf5)==0);


    assert(strlcat(buf4, "v", 1)==2); // for debug, init array contents with fixed repeat value
    assert(strcmp(buf4,buf5)==0);

    assert(strlcat(buf4, "v", 10)==11); // for debug, init array contents with fixed repeat value
    assert(strcmp(buf4,buf5)==0);
    assert(strlcat(buf4, "vvvvvvv",0)== 7);
    assert(strcmp(buf4,buf5)==0);

    assert(strlcat(buf4, "vvvvvvv",1)== 8);
    assert(strcmp(buf4,buf5)==0);

    assert(strlcat(buf4, "vvvvvvv",7)== 14);
    assert(strcmp(buf4,buf5)==0);    
    assert(strlcat(buf4, "vvvvvvv",10)== 17);
    assert(strcmp(buf4,buf5)==0);     


}

static void test_strtonum(void) {
    assert(strtonum("67", NULL) == 67);

    const char *input = "107rocks";
    const char *rest = NULL;
    long val = strtonum(input, &rest);
    assert(val == 107);
    assert(rest == &input[3]);  // rest should now point to first non-digit character

    /***** TODO: add more tests *****/

    const char* input2 = "0xA2";
    const char* rest2 = NULL;
    long val2 = strtonum(input2, &rest2);
    assert(val2 == 0xA2);


    const char* input3 = "0x123abc";
    const char* rest3 = NULL;
    long val3 = strtonum(input3, &rest3);
    assert(val3 == 0x123abc);

    const char* input4 = "0";
    const char* rest4 = NULL;
    long val4 = strtonum(input4, &rest4);
    assert(val4 == 0);

    const char* input5 = "0abcd";
    const char* rest5;
    long val5 = strtonum(input5, &rest5);
    assert(val5 == 0);
    assert(*rest5 == 'a');

//////
    val=    strtonum("0x0", NULL);
    assert (val==0);
    val=    strtonum("0x0", &rest);
    assert (val==0);
    assert(*rest == '\0'); 
    
    val=    strtonum("0X0Aa0BfQ", NULL);
    assert (val==0xAa0BF);
    val=    strtonum("0xAa0BfQ", &rest);
    assert (val==0xAa0BF);
    assert(*rest == 'Q');       

    val=    strtonum("+0X0Aa0BfQ", NULL);
    assert (val==0);
    val=    strtonum(" 0xAa0BfQ", &rest);
    assert (val==0);
    assert(*rest == ' ');     

    val=    strtonum("00X0Aa0BfQ", NULL);
    assert (val==0);
    val=    strtonum("00xAa0BfQ", &rest);
    assert (val==0);
    assert(*rest == 'x');  
//////
     val=    strtonum("0", NULL);
    assert (val==0);
    val=    strtonum("0", &rest);
    assert (val==0);
    assert(*rest == '\0'); 
    
    val=    strtonum("07Aa0BfQ", NULL);
    assert (val==7);
    val=    strtonum("7Aa0BfQ", &rest);
    assert (val==7);
    assert(*rest == 'A');       

    val=    strtonum("+0Aa0BfQ", NULL);
    assert (val==0);
    val=    strtonum(" Aa0BfQ", &rest);
    assert (val==0);
    assert(*rest == ' ');     

    val=    strtonum("00Aa0BfQ", NULL);
    assert (val==0);
    val=    strtonum("0Aa0BfQ", &rest);
    assert (val==0);
    assert(*rest == 'A');  
    
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
 
    assert(strcmp(decimal_string(-0), "0") == 0);
    assert(strcmp(hex_string(-0), "0") == 0);
    assert(strcmp(decimal_string(0), "0") == 0);
    //assert(strcmp(hex_string(-0x107e), "-107e") == 0);   
}

static void test_snprintf(void) {
    char buf[100];
    size_t bufsize = sizeof(buf);
    memset(buf, 0x7e, bufsize); // for debug, init array contents with fixed repeat value

    // No formatting codes
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // // One string formatting code
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    snprintf(buf, bufsize, "hello%c", 'v');
    assert(strcmp(buf, "hellov") == 0);

    snprintf(buf, bufsize, "hi%%abc");
    assert(strcmp(buf, "hi%abc") == 0);

    assert(snprintf(buf, bufsize, "hi%%abc") == 6);

    size_t bufsize2 = 3;
    snprintf(buf, bufsize2, "%s", "binky");
    assert(strcmp(buf, "bi") == 0);
    assert(snprintf(buf, bufsize2, "binky") == 2);

    snprintf(buf, bufsize, "%d", 76);
    assert(strcmp(buf, "76") == 0);
    assert(snprintf(buf, bufsize, "%d", 76) == 2);

    snprintf(buf, bufsize, "%x", 0xa5);
    assert(strcmp(buf, "a5") == 0);
    assert(snprintf(buf, bufsize, "%x", 0xa5) == 2);

    snprintf(buf, bufsize, "%ld", 12345678901);
    assert(strcmp(buf, "12345678901") == 0);
    assert(snprintf(buf, bufsize, "%ld", 12345678901) == 11);

    snprintf(buf, bufsize, "%lx", 0x123456789abcdef);
    assert(strcmp(buf, "123456789abcdef") == 0);
    assert(snprintf(buf, bufsize, "%lx", 0x123456789abcdef) == 15);
    /***** TODO: add more tests *****/

    snprintf(buf, bufsize, "hello%5c", 'v');
    assert(strcmp(buf, "hello    v") == 0);
    snprintf(buf, bufsize, "print%3c", 'f');
    assert(strcmp(buf, "print  f") == 0);
    assert(snprintf(buf, bufsize, "print%3c", 'f') == 8);

    snprintf(buf, bufsize, "owal%2c", 'a');
    assert(strcmp(buf, "owal a") == 0);
    assert(snprintf(buf, bufsize, "owal%2c", 'a') == 6);

    snprintf(buf, bufsize, "pumpkin%5s", "cake");
    assert(strcmp(buf, "pumpkin cake") == 0);
    assert(snprintf(buf, bufsize, "pumpkin%5s", "cake") == 12);

    snprintf(buf, bufsize, "gates%2s", "basement");
    assert(strcmp(buf, "gatesbasement") == 0);
    assert(snprintf(buf, bufsize, "gates%2s", "basement") == 13);

    snprintf(buf, bufsize, "mango%7s", "pi");
    assert(strcmp(buf, "mango     pi") == 0);
    assert(snprintf(buf, bufsize, "mango%7s", "pi") == 12);

    snprintf(buf, bufsize, "hi%5d", 42);
    assert(strcmp(buf, "hi00042") == 0);
    assert(snprintf(buf, bufsize, "hi%5d", 42) == 7);

    snprintf(buf, bufsize, "lucky%2d", 32);
    assert(strcmp(buf, "lucky32") == 0);
    assert(snprintf(buf, bufsize, "lucky%2d", 32) == 7);

    snprintf(buf, bufsize, "0123%6x", 0xa67);
    assert(strcmp(buf, "0123000a67") == 0);
    assert(snprintf(buf, bufsize, "0123%6x", 0xa67) == 10);
    // add edge cased
    // NEGATIVE CASES?
    // snprintf(buf, bufsize, "hi%5d", -42);
    // assert(strcmp(buf, "hi-0042") == 0);
    // assert(snprintf(buf, bufsize, "hi%5d", -42) == 7);

    //short/ long?
    snprintf(buf, bufsize, "count%10ld", (long)123456789);
    assert(strcmp(buf, "count0123456789") == 0);
    assert(snprintf(buf, bufsize, "count%10ld", (long)123456789) == 15);

    snprintf(buf, bufsize, "count%ld", (long)123456789);
    assert(strcmp(buf, "count123456789") == 0);
    assert(snprintf(buf, bufsize, "count%ld", (long)123456789) == 14);

    snprintf(buf, bufsize, "a%1lx", (unsigned long)0x123456789abcdef);
    assert(strcmp(buf, "a123456789abcdef") == 0);
    assert(snprintf(buf, bufsize, "a%1lx", (unsigned long)0x123456789abcdef) == 16);

    snprintf(buf, bufsize, "d%p", (void*)0x1234);
    assert(strcmp(buf, "d00001234") == 0);
    assert(snprintf(buf, bufsize, "d%p", (void*)0x1234) == 9);



}
//negative numebrs and short.long
void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    // Uncomment the call to each test function below when you have implemented
    // the functions and are ready to test them

    test_memset();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_helpers();
    test_snprintf();

    /***** TODO: Add more and better tests!  *****/

    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}
