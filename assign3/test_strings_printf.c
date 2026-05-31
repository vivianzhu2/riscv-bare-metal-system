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


static void test_memset(void) { // memset tests for setting memory to a specific value, including edge cases with different buffer sizes and values
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

static void test_strcmp(void) { // strcmp tests for comparing strings, including edge cases with different lengths and characters
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

static void test_strlcat(void) { // strlcat tests for appending strings with various buffer sizes, including edge cases where the buffer is too small to hold the entire result
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
    strlcat(buf2, "world", bufsize2); // append world
    assert(strlcat(buf2, "!lotsof!!!!!!!!!!!!!!!!!!!!!",bufsize2)== 43);
    // char teststr[19] = "Today is a good day";
    // assert(strlcat(buf2, teststr, bufsize2) == 25);
///////
    char buf3[20];
    size_t bufsize3 = sizeof(buf3)+5; 
    memset(buf3, 0x7e, bufsize3); // for debug, init array contents with fixed repeat value
     
    buf3[0]='\0';
    assert(strlcat(buf3, "", 0)==0); // for debug, init array contents with fixed repeat value
    assert(strlcat(buf3, "",1)== 0);
    assert(strlcat(buf3, "",10)== 0);   // strlcat(buf2, "world", bufsize2); // append world
    buf3[0]='\0';
    assert(strlcat(buf3, "v", 0)==1); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';   
    assert(strlcat(buf3, "v", 1)==1); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';
    assert(strlcat(buf3, "v", 10)==1); // for debug, init array contents with fixed repeat value
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",0)== 7);
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",1)== 7);
    buf3[0]='\0';
    assert(strlcat(buf3, "vvvvvvv",7)== 7);
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

static void test_strtonum(void) { // strtonum tests for converting strings to numbers, including edge cases and error handling
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

static void test_helpers(void) { // tests for helper functions used by printf, such as num_to_string, decimal_string, and hex_string
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

static void test_snprintf(void) { // snprintf tests are in test_printf.c
    char buf[100];
    size_t bufsize = sizeof(buf);
    memset(buf, 0x7e, bufsize); // for debug, init array contents with fixed repeat value

    // // No formatting codes
    printf("Hello, world!");
    snprintf(buf, 10, "%x", 0x9abcdef0);  //ag_assertion failed: dst contents should be |9abcdef0|
    assert(strcmp(buf, "9abcdef0") == 0);
    snprintf(buf, 20, "%x", 0xffffffff);  //ag_assertion failed: dst contents should be |ffffffff|
    assert(strcmp(buf, "ffffffff") == 0);

    snprintf(buf, 20, "%12s", "stanford");//  ag_assertion failed: dst contents should be |    stanford|
    assert(strcmp(buf, "    stanford") == 0);
    snprintf(buf, 20, "%4s", "stanford");  // ag_assertion failed: dst contents should be |stanford|
    assert(strcmp(buf, "stanford") == 0);
    snprintf(buf, 20, "%4c", 'Z'); //  ag_assertion failed: dst contents should be |   Z|
    assert(strcmp(buf, "   Z") == 0);
    snprintf(buf, 20, "%1c", 'Z');  // ag_assertion failed: dst contents should be |Z|
    assert(strcmp(buf, "Z") == 0);
    snprintf(buf, 20, "%2d", 987);  // ag_assertion failed: dst contents should be |987|
    assert(strcmp(buf, "987") == 0);
    snprintf(buf, 20, "%5d", 365);  // ag_assertion failed: dst contents should be |  365|
    assert(strcmp(buf, "  365") == 0);
    snprintf(buf, 20, "%9ld", -100L);  // ag_assertion failed: dst contents should be |     -100|
    assert(strcmp(buf, "     -100") == 0);
    snprintf(buf, 20, "%2x", 0x107e);  // ag_assertion failed: dst contents should be |107e|
    assert(strcmp(buf, "107e") == 0);
    snprintf(buf, 20, "%8x", 0xa01c); // ag_assertion failed: dst contents should be |0000a01c|
    assert(strcmp(buf, "0000a01c") == 0);




    snprintf(buf, 20, "%p", (void *)0x1badf00d);  //ag_assertion failed: dst contents should be |0x1badf00d|
    assert(strcmp(buf, "0x1badf00d") == 0);
    snprintf(buf, 20, "%p", (void *)0x100200300400L); // ag_assertion failed: dst contents should be |0x100200300400|
    assert(strcmp(buf, "0x100200300400") == 0);
    snprintf(buf, 20, "%6p", (void *)0x72f95);  //ag_assertion failed: dst contents should be |0x072f95|
    assert(strcmp(buf, "0x072f95") == 0);
    

    snprintf(buf, 50, "%p: %2x %2x %2x %2x", (void *)0x40000000, 0, 10, 37, 48); // ag_assertion failed: dst contents should be |0x40000000: 00 0a 25 30|
    assert(strcmp(buf, "0x40000000: 00 0a 25 30") == 0);
    snprintf(buf, 50, "Yo %s%de%2c", "cs", 107, '!'); //  ag_assertion failed: dst contents should be |Yo cs107e !|
    assert(strcmp(buf, "Yo cs107e !") == 0);

    snprintf(buf, 5, "%6d", 2525); // ag_assertion failed: dst contents should be |  25|
    assert(strcmp(buf, "  25") == 0);
    snprintf(buf, 5, "%6d", -3636); // ag_assertion failed: dst contents should be | -36|
    assert(strcmp(buf, " -36") == 0);

    snprintf(buf, 0, "No room at the inn");
    printf("buf:%s", buf);
    // printf("\n");
    assert(strcmp(buf, "") == 0); // dst should not be modified when bufsize is 0
    assert(buf[0] == '\0');
    assert(strlen(buf)==0);
    assert(snprintf(buf, 0, "No room at the inn") == 18); // return value should be length of full output even when bufsize is 0

    int ret_val = 0;
    ret_val = snprintf(buf, 20, "%8s", "Hello"); // ag_assertion failed: ret_val should be 8, was 84
    assert(ret_val == 8);
    ret_val = snprintf(buf, 20, "%2d", 987);  //ag_assertion failed: ret_val should be 3, was 20
    assert(ret_val == 3);
    ret_val = snprintf(buf, 20, "%5d", -365); // ag_assertion failed: ret_val should be 5, was 50
    assert(ret_val == 5);
    ret_val = snprintf(buf, 20, "%2x", 0x107e); // ag_assertion failed: ret_val should be 4, was 20
    assert(ret_val == 4);
    ret_val = snprintf(buf, 20, "%8x", 0xa01c); // ag_assertion failed: ret_val should be 8, was 80
    assert(ret_val == 8);
    ret_val = snprintf(buf, 20, "%8c", 'Z'); // ag_assertion failed: ret_val should be 8, was 80
    assert(ret_val == 8);
    
    ret_val = snprintf(buf, 20, "Yo %s%de%2c", "cs", 107, '!'); // ag_assertion failed: ret_val should be 11, was 29
    assert(ret_val == 11);

    ret_val = snprintf(buf, 3, "%8s", "Hello"); // ag_assertion failed: ret_val should be 8, was 84
    assert(ret_val == 8);
    ret_val = snprintf(buf, 3, "%2d", 987);  // ag_assertion failed: ret_val should be 3, was 20
    assert(ret_val == 3);
    ret_val = snprintf(buf, 3, "%5d", -365); // ag_assertion failed: ret_val should be 5, was 50
    assert(ret_val == 5);
    ret_val = snprintf(buf, 3, "%2x", 0x107e); // ag_assertion failed: ret_val should be 4, was 20
    assert(ret_val == 4);
    ret_val = snprintf(buf, 3, "%8x", 0xa01c); // ag_assertion failed: ret_val should be 8, was 80
    assert(ret_val == 8);
    ret_val = snprintf(buf, 3, "%8c", 'Z'); // ag_assertion failed: ret_val should be 8, was 80
    assert(ret_val == 8);

    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // // // One string formatting code
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    snprintf(buf, bufsize, "hello%c", 'v');
    assert(strcmp(buf, "hellov") == 0);

    snprintf(buf, bufsize, "hi%%abc");
    assert(strcmp(buf, "hi%abc") == 0);

    assert(snprintf(buf, bufsize, "hi%%abc") == 6);

    // size_t bufsize2 = 3;
    // snprintf(buf, bufsize2, "%s", "binky");
    // assert(strcmp(buf, "bi") == 0);
    // assert(snprintf(buf, bufsize2, "binky") == 2);

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
    // /***** TODO: add more tests *****/

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
    assert(strcmp(buf, "hi   42") == 0);
    assert(snprintf(buf, bufsize, "hi%5d", 42) == 7);

    snprintf(buf, bufsize, "lucky%2d", 32);
    assert(strcmp(buf, "lucky32") == 0);
    assert(snprintf(buf, bufsize, "lucky%2d", 32) == 7);

    snprintf(buf, bufsize, "0123%6x", 0xa67);
    assert(strcmp(buf, "0123000a67") == 0);
    assert(snprintf(buf, bufsize, "0123%6x", 0xa67) == 10);
    // // add edge cased
    // // NEGATIVE CASES?
    snprintf(buf, bufsize, "hi%5d", -42);
    assert(strcmp(buf, "hi  -42") == 0);
    assert(snprintf(buf, bufsize, "hi%5d", -42) == 7);

    // //short/ long?
    // snprintf(buf, bufsize, "count%10ld", (long)123456789);
    // assert(strcmp(buf, "count 123456789") == 0);
    // assert(snprintf(buf, bufsize, "count%10ld", (long)123456789) == 15);

    snprintf(buf, bufsize, "count%ld", (long)123456789);
    assert(strcmp(buf, "count123456789") == 0);
    assert(snprintf(buf, bufsize, "count%ld", (long)123456789) == 14);

    snprintf(buf, bufsize, "a%1lx", (unsigned long)0x123456789abcdef);
    assert(strcmp(buf, "a123456789abcdef") == 0);
    assert(snprintf(buf, bufsize, "a%1lx", (unsigned long)0x123456789abcdef) == 16);

    snprintf(buf, bufsize, "d%p", (void*)0x1234);
    assert(strcmp(buf, "d0x00001234") == 0);
    assert(snprintf(buf, bufsize, "d%p", (void*)0x1234) == 11);

    buf[0]='\0';
    assert(snprintf(buf, 0, "vvvvvv")==6);
    assert(strcmp(buf,"")==0);
    buf[0]='\0';
    assert(snprintf(buf, 6, "vvvvv")==5);
    assert(strcmp(buf,"vvvvv")==0);
    buf[0]='\0';
    assert(snprintf(buf, 5, "vvvvv")==5);
    assert(strcmp(buf,"vvvv")==0);
    buf[0]='\0';
    assert(snprintf(buf, 4, "vvvvv")==5);
    assert(strcmp(buf,"vvv")==0);
    buf[0]='\0';
    assert(snprintf(buf, 1, "vvvvv")==5);
    assert(strcmp(buf,"")==0);
    buf[0]='\0';
    assert(snprintf(buf, 0, "vvvvv")==5);
    assert(strcmp(buf,"")==0);
    
    // buf[0]='\0';
    // assert(snprintf(buf, 2, "A%c", '\0')==2);
    // assert(strcmp(buf,"A")==0);
    buf[0]='\0';
    assert(snprintf(buf, 2, "A%s", "")==1);
    assert(strcmp(buf,"A")==0);
    buf[0]='\0';
    assert(snprintf(buf, 10, "A%7s", "vvvvv")==8);
    assert(strcmp(buf,"A  vvvvv")==0);
    buf[0]='\0';
    assert(snprintf(buf, 20, "A%2s", "vvvvv")==6);
    assert(strcmp(buf,"Avvvvv")==0);
    
    buf[0]='\0';
    assert(snprintf(buf, 20, "A%d", -2000000000)==12);
    assert(strcmp(buf,"A-2000000000")==0);
    buf[0]='\0';
    assert(snprintf(buf, 20, "A%d", 0)==2);
    assert(strcmp(buf,"A0")==0);
    buf[0]='\0';
    assert(snprintf(buf, 30, "A%ld", -900000000000000000L)== 20);
    assert(strcmp(buf,"A-900000000000000000")==0);
    buf[0]='\0';
    assert(snprintf(buf, 20, "A %lx", 0x1234567890ABCDEF)==18);
    // assert(strcmp(buf,"A 1234567890ABCDEF")==0);
    buf[0]='\0';
    snprintf(buf, 20, "A %p", (void*)0x890AB);
    assert(snprintf(buf, 20, "A %p", (void*)0x890AB)==12);
    assert(strcmp(buf,"A 0x000890ab")==0);
    buf[0]='\0';
    assert(snprintf(buf, 20, "A %9p", (void*)0x890AB)==13);
    assert(strcmp(buf,"A 0x0000890ab")==0);
    buf[0]='\0';
    assert(snprintf(buf, 20, "A %p", NULL)==12);
    assert(strcmp(buf,"A 0x00000000")==0);
    buf[0]='\0';
    assert(snprintf(buf, 40, "A %p %s %x", (void*)0x890AB,"(void*)0x890AB",0x890AB)==33);
    assert(strcmp(buf,"A 0x000890ab (void*)0x890AB 890ab")==0);
    buf[0]='\0';
    // snprintf(buf, 20, "A %", 3);
    // buf[0]='\0';
    // snprintf(buf, 20, "A %b", 0xABCD);
    
    assert(snprintf(buf, 6, "A%d", 0)==2);
    assert(strcmp(buf,"A0")==0);
    buf[0]='\0';
    assert(snprintf(buf, 6, "A%ld", -900000000000000000L)== 20);
    assert(strcmp(buf,"A-900")==0);
    buf[0]='\0';
    assert(snprintf(buf, 6, "A %lx", 0x1234567890ABCDEF)==18);
    assert(strcmp(buf,"A 123")==0);
    buf[0]='\0';
    snprintf(buf, 6, "A %p", (void*)0x890AB);
    assert(snprintf(buf, 6, "A %p", (void*)0x890AB)==12);
    assert(strcmp(buf,"A 0x0")==0);
    buf[0]='\0';
    assert(snprintf(buf, 6, "A %9p", (void*)0x890AB)==13);
    assert(strcmp(buf,"A 0x0")==0);
    buf[0]='\0';
    assert(snprintf(buf, 6, "A %p", NULL)==12);
    assert(strcmp(buf,"A 0x0")==0);
    buf[0]='\0';
    assert(snprintf(buf, 7, "A %p %s %x", (void*)0x890AB,"void*)0x890AB",0x890AB)==32);
    assert(strcmp(buf,"A 0x00")==0);
    // buf[0]='\0';

    buf[0]='\0';
    assert(snprintf(buf, 6, "abc%sefg%s", "vvvvv", "wwwww")==16);
    assert(strcmp(buf,"abcvv")==0);
    
// extention tests for negtive width and %*s
    // snprintf(buf, bufsize, "hello%-5c", 'v');
    // assert(strcmp(buf, "hellov    ") == 0);
    // snprintf(buf, bufsize, "print%-3c", 'f');
    // assert(strcmp(buf, "printf  ") == 0);
    // assert(snprintf(buf, bufsize, "print%3c", 'f') == 8);

    // snprintf(buf, bufsize, "owal%-2c", 'a');
    // assert(strcmp(buf, "owala ") == 0);
    // assert(snprintf(buf, bufsize, "owal%2c", 'a') == 6);


    // snprintf(buf, bufsize, "pumpkin%-5s", "cake");
    // assert(strcmp(buf, "pumpkincake ") == 0);
    // assert(snprintf(buf, bufsize, "pumpkin%-5s", "cake") == 12);

    // snprintf(buf, bufsize, "gates%-2s", "basement");
    // assert(strcmp(buf, "gatesbasement") == 0);
    // assert(snprintf(buf, bufsize, "gates%-2s", "basement") == 13);

    // snprintf(buf, bufsize, "mango%-7s", "pi");
    // assert(strcmp(buf, "mangopi     ") == 0);
    // assert(snprintf(buf, bufsize, "mango%-7s", "pi") == 12);

    // snprintf(buf, bufsize, "hi%-5d", 42);
    // assert(strcmp(buf, "hi42   ") == 0);
    // assert(snprintf(buf, bufsize, "hi%-5d", 42) == 7);

    // snprintf(buf, bufsize, "lucky%-2d", 32);
    // assert(strcmp(buf, "lucky32") == 0);
    // assert(snprintf(buf, bufsize, "lucky%-2d", 32) == 7);

    // snprintf(buf, bufsize, "0123%-6x", 0xa67);
    // assert(strcmp(buf, "0123a67000") == 0);
    // assert(snprintf(buf, bufsize, "0123%-6x", 0xa67) == 10);
    // // // add edge cased
    // // // NEGATIVE CASES?
    // snprintf(buf, bufsize, "hi%-5d", -42);
    // assert(strcmp(buf, "hi-42  ") == 0);
    // assert(snprintf(buf, bufsize, "hi%-5d", -42) == 7);

    // buf[0]='\0';
    // assert(snprintf(buf, 6, "A %-9p", (void*)0x890AB)==13);
    // assert(strcmp(buf,"A 0x8")==0);

    // buf[0]='\0';
    // assert(snprintf(buf, 20, "A %-9p", (void*)0x890AB)==13);
    // assert(strcmp(buf,"A 0x890ab0000")==0);



    // snprintf(buf, bufsize, "hello%*c", -5, 'v');
    // assert(strcmp(buf, "hellov    ") == 0);
    // snprintf(buf, bufsize, "print%*c", -3, 'f');
    // assert(strcmp(buf, "printf  ") == 0);
    // assert(snprintf(buf, bufsize, "print%3c", 'f') == 8);

    // snprintf(buf, bufsize, "owal%*c",2, 'a');
    // assert(strcmp(buf, "owal a") == 0);
    // assert(snprintf(buf, bufsize, "owal%*c",2, 'a') == 6);


    // snprintf(buf, bufsize, "pumpkin%*s", 5, "cake");
    // assert(strcmp(buf, "pumpkin cake") == 0);
    // assert(snprintf(buf, bufsize, "pumpkin%*s", 5, "cake") == 12);

    // snprintf(buf, bufsize, "gates%*s", -2, "basement");
    // assert(strcmp(buf, "gatesbasement") == 0);
    // assert(snprintf(buf, bufsize, "gates%*s",-2, "basement") == 13);

    // snprintf(buf, bufsize, "mango%*s", -7, "pi");
    // assert(strcmp(buf, "mangopi     ") == 0);
    // assert(snprintf(buf, bufsize, "mango%*s",-7, "pi") == 12);

    // snprintf(buf, bufsize, "hi%*d", -5, 42);
    // assert(strcmp(buf, "hi42   ") == 0);
    // assert(snprintf(buf, bufsize, "hi%*d",-5, 42) == 7);

    // snprintf(buf, bufsize, "lucky%*d", 2, 32);
    // assert(strcmp(buf, "lucky32") == 0);
    // assert(snprintf(buf, bufsize, "lucky%*d",-2, 32) == 7);

    // snprintf(buf, bufsize, "0123%*x", -6, 0xa67);
    // assert(strcmp(buf, "0123a67000") == 0);
    // assert(snprintf(buf, bufsize, "0123%*x", -6, 0xa67) == 10);
    // // // add edge cased
    // // // NEGATIVE CASES?
    // snprintf(buf, bufsize, "hi%*d", -5, -42);
    // assert(strcmp(buf, "hi-42  ") == 0);
    // assert(snprintf(buf, bufsize, "hi%*d",-5, -42) == 7);

    // buf[0]='\0';
    // assert(snprintf(buf, 6, "A %*p",9, (void*)0x890AB)==13);
    // assert(strcmp(buf,"A 0x0")==0);

    // buf[0]='\0';
    // assert(snprintf(buf, 20, "A %*p", -9, (void*)0x890AB)==13);
    // assert(strcmp(buf,"A 0x890ab0000")==0);
}


static void test_printf(void) { // printf tests for various format strings and arguments, including edge cases and error handling

    printf("hi%5d", -42);
    assert(printf("hi%5d", -42) == 7);

    printf( "hi%%abc");


    assert(printf( "hi%%abc") == 6);
   


   printf( "%d", 76);

    assert(printf( "%d", 76) == 2);

   printf( "%x", 0xa5);

    assert(printf( "%x", 0xa5) == 2);

   printf( "%ld", 12345678901);

   assert(printf( "%ld", 12345678901) == 11);

   printf( "%lx", 0x123456789abcdef);
    assert(printf( "%lx", 0x123456789abcdef) == 15);
    /***** TODO: add more tests *****/

   printf( "hello%5c", 'v');

   printf( "print%3c", 'f');

    assert(printf( "print%3c", 'f') == 8);

   printf( "owal%2c", 'a');
 
    assert(printf( "owal%2c", 'a') == 6);

   printf( "pumpkin%5s", "cake");

    assert(printf( "pumpkin%5s", "cake") == 12);

   printf( "gates%2s", "basement");

    assert(printf( "gates%2s", "basement") == 13);

   printf( "mango%7s", "pi");

    assert(printf( "mango%7s", "pi") == 12);

   printf( "hi%5d", 42);

    assert(printf( "hi%5d", 42) == 7);

   printf( "lucky%2d", 32);

    assert(printf( "lucky%2d", 32) == 7);

   printf( "0123%6x", 0xa67);
   
    assert(printf( "0123%6x", 0xa67) == 10);



   printf( "count%10ld", (long)123456789);

    assert(printf( "count%10ld", (long)123456789) == 15);

   printf( "count%ld", (long)123456789);

    assert(printf( "count%ld", (long)123456789) == 14);

   printf( "a%1lx", (unsigned long)0x123456789abcdef);
    assert(printf( "a%1lx", (unsigned long)0x123456789abcdef) == 16);

   printf( "d%p", (void*)0x1234);
   assert(printf( "d%p", (void*)0x1234) == 11);



}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    // Uncomment the call to each test function below when you have implemented
    // the functions and are ready to test them

    test_memset(); //   memset tests for setting memory to a specific value, including edge cases with different buffer sizes and values   
    test_strcmp(); // strcmp tests for comparing strings, including edge cases with different lengths and characters
    test_strlcat(); // strlcat tests for appending strings with various buffer sizes, including edge cases where the buffer is too small to hold the entire result
    test_strtonum(); // strtonum tests for converting strings to numbers, including edge cases and error handling
    test_helpers(); // tests for helper functions used by printf, such as num_to_string, decimal_string, and hex_string
    test_snprintf(); // snprintf tests for various format strings and arguments, including edge cases and error handling
    test_printf(); // printf tests for various format strings and arguments, including edge cases and error handling

    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}