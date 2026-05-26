/* File: test_interrupts.c
 * -----------------------
 * This file contains some simple tests to confirm that the PS/2 keyboard
 * implementation is correctly generating interrupts and that the interrupt handler is correctly reading scancodes and making them available to the main program.
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"

#include "assert.h"
#include "shell.h"
#include "strings.h"
#include "mango.h"

/*
 * This function tests the behavior of the assign5 ps2
 * implementation versus the new-improved assign7 version. If using the
 * ps2 module as written for assign5, a scancode that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * ps2 module for assign7 to be interrupt-driven, those scancodes should
 * be queued up and can be read after delay finishes.
 */
static void check_read_delay(void) {
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    printf("\n%s() will read each typed key and pause for a few seconds\n", __func__);
    while (1) {
        printf("\nType a key on PS/2 keyboard (q to quit): ");
        char ch = keyboard_read_next();
        printf("\nRead: %c\n", ch);
        if (ch == 'q') break;
        printf("Pausing for 2 seconds (type ahead now on PS/2 keyboard to queue events)... ");
        timer_delay(2);
        printf("done.\n");
    }
}

static void clock_edge(void *clientData) {
    static int count = 0;
    gpio_interrupt_clear(KEYBOARD_CLOCK);
    uart_putchar('0' + count); // print digit
    if (++count == 11) {// after 11 edges = one scancode, reset counter
        count = 0;
        uart_putchar('\n');
    }
}

/*
 * This function is a simple test to confirm interrupts are being generated
 * and that the interrupt handler is being called. It configures interrupts
 * on the clock gpio using the handler function above a counter.
 */
static void check_interrupts_received(void) {
    gpio_interrupt_init();
    gpio_interrupt_config(KEYBOARD_CLOCK, GPIO_INTERRUPT_NEGATIVE_EDGE, false);
    gpio_interrupt_set_handler(KEYBOARD_CLOCK, clock_edge, NULL);
    interrupts_global_enable();

    printf("\n%s() waiting for interrupts on keyboard clock gpio\n", __func__);
    printf("Type on your PS/2 keyboard. I'll wait for 5 seconds...\n");
    timer_delay(5);
    printf("Time's up!\n");
}

//
#define ESC_SCANCODE 0x76
 // these are my assign5 scancode tests, which should still work with the new interrupt-driven ps2 implementation.
static void test_keyboard_scancodes(void) {
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        uint8_t scancode = keyboard_read_scancode();
        printf("[%2x]\n", scancode);
        if (scancode == ESC_SCANCODE) break;
    }
    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void) {
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        const char *which = action.what == KEY_PRESS ? "Press" : "Release";
        printf("%7s [%2x] %s\n", which, action.scancode, action.extended? "extended" : "");
        if (action.what == KEY_PRESS && action.scancode == ESC_SCANCODE) break;
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void) {
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c,0x%x} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.ch_shifted, evt.key.id, evt.modifiers);
        if (evt.action.what == KEY_PRESS && evt.key.id == PS2_KEY_ESC) break;
    }
    printf("Done with key events test.\n");
}

static void test_keyboard_chars(void) {
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c <= 0x7f) // if ASCII
            printf("%c", c);
        else
            printf("[%2x]", c);
        if (c == PS2_KEY_ESC) break;
    }
    printf("\nDone with key chars test.\n");
}



static void test_keyboard_assert(void) {
    char ch;
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated
 
}

// This is a more comprehensive unit test of the keyboard_read_next function that tests all the modifiers and their interactions with each other.
static void test_keyboard_assert_unit(void) {
    char ch;
    printf("\nHold down left Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated
    printf("\nHold down right Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated        
    printf("\nPress CapLock once and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');  // confirm user can follow directions and correct key char generated

    printf("\nPress CapLock again and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'g');  // confirm user can follow directions and correct key char generated

    printf("\nHold down left Ctrl and type 'h'\n");
    ch = keyboard_read_next();
    assert(ch == '\b');  // confirm user can follow directions and correct key char generated
    printf("\nHold down right Ctrl and type 'h'\n");
    ch = keyboard_read_next();
    assert(ch == '\b');  // confirm user can follow directions and correct key char generated

    printf("\nHold down left Alt and type 'z'\n");
    ch = keyboard_read_next();
    assert(ch == 'z');  // confirm user can follow directions and correct key char generated
    printf("\nHold down right Alt and type 'z'\n");
    ch = keyboard_read_next();
    assert(ch == 'z');  // confirm user can follow directions and correct key char generated

}
 //
static void test_shell_evaluate(void) {
    shell_init(keyboard_read_next, printf);

    printf("\nTest shell_evaluate on fixed commands.\n");
    int ret = shell_evaluate("echo hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);



}

//  This is a unit test function that uses the shell_evaluate function to turn on the blue LED. 
static void test_shell_evaluate_blue_led_on(void) {
    shell_init(keyboard_read_next, printf);

///
    printf("\nTest shell_evaluate on fixed poke commands to turn on and off the blue LED!.\n");
    int ret = shell_evaluate("poke 0x20000a0 0x40000");
    printf("Command result is zero if successful, is it? %d\n", ret);
    printf("Blue LED is turned ON, is it?\n");


}

//  This is a unit test function that uses the shell_evaluate function to turn off the blue LED. 
static void test_shell_evaluate_blue_led_off(void) {
    shell_init(keyboard_read_next, printf);

///
    printf("\nTest shell_evaluate on fixed poke commands to turn on and off the blue LED!.\n");
    int ret = shell_evaluate("poke 0x20000a0 0");
    printf("Command result is zero if successful, is it? %d\n", ret);
    printf("Blue LED is turned OFF, is it?\n");


}

// This is an example of a "mock" input used for testing. When asked to read
// next character, read_fixed returns char from a fixed string and advances
// its internal index
static char read_fixed(void) {
    const char *input = "echo hello, world\nhelp\n";
    static int index = 0; // note declared static

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_input(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixed, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as mock input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static void test_shell_readline_keyboard(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(keyboard_read_next, printf); // input from keybaord

    printf("\nTest shell_readline, type a line of input on ps2 keyboard.\n");
    printf("? ");
    shell_readline(buf, bufsize);
}

// This is an example of a "mock" input used for testing the shell command poke to turn on/ off the blue LED.
static char read_fixed_blue_led(void) {
    const char *input = "poke 0x20000a0 0x40000\npoke 0x20000a0 0\n";
    static int index = 0; // note declared static

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}


// This is an example of a "mock" input used for testing the shell command poke to turn on/ off the blue LED.
static void test_shell_readline_fixed_input_blue_led(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixed_blue_led, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed poke ledstring as mock input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
}

// void main(void) {
//     uart_init();
//     keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

//     printf("Testing keyboard and shell.\n");

//     // Uncomment the call to each test function below when you have implemented
//     // the functions and are ready to test them


 

//     printf("Finished executing main() in test_keyboard_shell.c\n");
// }



// The main function runs all the tests. Uncomment the calls individually to test them. 
void main(void) {
    gpio_init();
    timer_init();
    uart_init();
    printf("\nStarting main() in %s\n", __FILE__);
    interrupts_init();
    interrupts_global_enable();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    // check_interrupts_received();
    // check_read_delay();


    // test_keyboard_scancodes();
    // test_keyboard_sequences();
    // test_keyboard_events();
    // test_keyboard_chars();
    // test_keyboard_assert();
    // test_shell_evaluate();
    // test_shell_readline_fixed_input();
    // test_shell_readline_keyboard();
    // 

    test_keyboard_assert_unit();   // X
    // test_shell_readline_fixed_input_blue_led();
    // test_shell_evaluate_blue_led_on();
    // test_shell_evaluate_blue_led_off();  

    printf("\nCompleted execution of main() in %s\n", __FILE__);

}
