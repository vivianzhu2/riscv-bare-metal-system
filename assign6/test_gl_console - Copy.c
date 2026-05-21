/* File: test_gl_console.c
 * -----------------------
 * Unit tests for fb, gl and console modules. 
 * 
 */
#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"

static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in tio to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

static void test_fb(void) {
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);

    void *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    pause("Now displaying 500 x 500 screen of light gray pixels");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    cptr = fb_get_draw_buffer();
    nbytes =  fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    assert(cptr != NULL);


    fb_swap_buffer();
    assert(cptr != NULL);
    printf("cptr: %p, %x", cptr, *(char*)cptr);
    // assert(cptr != NULL);
    pause("Now displaying 1280 x 720 white pixels");

    cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    assert(cptr != NULL);
    fb_swap_buffer();
    assert(cptr != NULL);
    pause("Now displaying 1280 x 720 dark gray pixels");

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer();
        assert(cptr != NULL);
        timer_delay_ms(250);
    }
}

static void test_gl(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55)); // create purple color

    // Draw green pixel in lower right
    gl_draw_pixel(WIDTH-10, HEIGHT-10, GL_GREEN);
    assert(gl_read_pixel(WIDTH-10, HEIGHT-10) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH/2 - 100, HEIGHT/2 - 50, 200, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");
    gl_draw_string(150, 50, "VIVIAN ZHU", GL_WHITE);
    
    gl_swap_buffer();

    // gl_draw_char(170, 50, 'V', GL_WHITE);
    // gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");
    /* Test: form feed in gl_draw_string should clear the screen and move cursor to top-left.
     * Draw some text at an offset, then issue a string containing a form feed followed by
     * a single character. After swapping, the previous text location should be cleared
     * (background color) and the character should appear at the top-left pixel(s).
     */
    const uint32_t BG_COLOR = gl_color(0x55, 0, 0x55); // same purple bg used above
    /* ensure background is set and visible */
    gl_clear(BG_COLOR);
    gl_draw_string(150, 50, "FORMFEED_TEST", GL_WHITE);
    gl_swap_buffer();

    /* Now draw a string that begins with a form feed which should clear and move cursor */
    gl_draw_string(0, 0, "\fX", GL_AMBER);
    gl_swap_buffer();

    /* previous text position should now be background color */
    assert(gl_read_pixel(150, 50) == BG_COLOR);
    /* top-left should contain the new character (at least its top-left pixel) */
    assert(gl_read_pixel(0, 0) == GL_AMBER);
    pause("Form feed test: screen cleared and cursor moved to top-left (should show 'X' at top-left)");
    // gl_draw_char(160, 50, 'I', GL_AMBER);
    // gl_swap_buffer();

    /***** TODO: add more tests *****/

    /*draw rectangle larger than the screen and confirm clipping.
     *  After drawing and swapping,
     * pixels at the four corners of the screen should be the rectangle color.
     */
    const uint32_t OVERSIZE_COLOR = gl_color(0xAB, 0x12, 0x56);
    gl_draw_rect(-100, -100, WIDTH + 200, HEIGHT + 200, OVERSIZE_COLOR);
    gl_swap_buffer();
    /* Check the four corners of the framebuffer are painted with OVERSIZE_COLOR */
    assert(gl_read_pixel(0, 0) == OVERSIZE_COLOR);
    assert(gl_read_pixel(WIDTH-1, 0) == OVERSIZE_COLOR);
    assert(gl_read_pixel(0, HEIGHT-1) == OVERSIZE_COLOR);
    assert(gl_read_pixel(WIDTH-1, HEIGHT-1) == OVERSIZE_COLOR);
    pause("Now displaying oversize rectangle clipped to screen (should fill entire screen)");

    /* draw single pixel in each corner and verify out-of-bounds writes do nothing */
    const uint32_t CORNER_COLOR = gl_color(0xff, 0x00, 0x00); /* red */
    /* Reset background to known color */
    gl_clear(BG_COLOR);
    /* Draw pixels in the four corners */
    gl_draw_pixel(0, 0, CORNER_COLOR);
    gl_draw_pixel(WIDTH-1, 0, CORNER_COLOR);
    gl_draw_pixel(0, HEIGHT-1, CORNER_COLOR);
    gl_draw_pixel(WIDTH-1, HEIGHT-1, CORNER_COLOR);
    /* Attempt to draw just outside each corner (should be ignored/clipped) */
    gl_draw_pixel(-1, 0, GL_GREEN);
    gl_draw_pixel(0, -1, GL_GREEN);
    gl_draw_pixel(WIDTH, 0, GL_GREEN);
    gl_draw_pixel(0, HEIGHT, GL_GREEN);
    gl_swap_buffer();
    /* Verify corner pixels are the corner color */
    assert(gl_read_pixel(0, 0) == CORNER_COLOR);
    assert(gl_read_pixel(WIDTH-1, 0) == CORNER_COLOR);
    assert(gl_read_pixel(0, HEIGHT-1) == CORNER_COLOR);
    assert(gl_read_pixel(WIDTH-1, HEIGHT-1) == CORNER_COLOR);
    /* Verify out-of-bounds attempts did not write inside framebuffer */
    /* check adjacent pixels remain background color */
    assert(gl_read_pixel(1, 0) == BG_COLOR || gl_read_pixel(1, 0) == CORNER_COLOR);
    assert(gl_read_pixel(0, 1) == BG_COLOR || gl_read_pixel(0, 1) == CORNER_COLOR);
    assert(gl_read_pixel(WIDTH-2, 0) == BG_COLOR || gl_read_pixel(WIDTH-2, 0) == CORNER_COLOR);
    assert(gl_read_pixel(0, HEIGHT-2) == BG_COLOR || gl_read_pixel(0, HEIGHT-2) == CORNER_COLOR);
    pause("Corner pixel test: four red pixels at corners; out-of-bounds writes should be ignored.");

    
    /// One possibility is to rig up assert-based unit tests that make a gl_draw_xxx call followed by calls to gl_read_pixel to confirm the pixel color at a location. This approach doesn't scale much beyond simple cases. We do recommend asserts for edge cases or specific details that are not easily to visually confirm (i.e. a rectangle of width 10 spans exactly 10 pixels). Most of yorur testing will be by visually confirming the result that is drawn on the display – that's the real deal! Make sure pixels that should have been clipped are not being drawn and that you never write outside the bounds of the framebuffer memory.

   
    // use gl_draw_rect to draw rectangles that are partially off-screen to confirm that clipping is working correctly.
    // use gl_draw_string to draw a string that is partially off-screen to confirm that clipping is working correctly for character drawing.
    // use gl_draw_string to draw a string that is longer than the width of the screen to confirm that wrapping and clipping are working correctly for character drawing.
    gl_clear(BG_COLOR);
    const char *long_string = "This is a very long string that should wrap and clip at the screen boundaries";
    gl_draw_string(0, 0, long_string, GL_WHITE);
    gl_swap_buffer();
    /* Verify text is drawn at top-left */
    assert(gl_read_pixel(0, 0) == GL_WHITE);
    pause("Long string test: string wraps within screen boundaries and is clipped correctly");

    // use gl_draw_string to draw enough lines of text to cause scrolling and confirm that the scrolling is working correctly and that the correct lines are being scrolled off the top of the screen and that new lines are appearing at the bottom of the screen.
    // use gl_draw_string to draw enough lines of text to cause multiple scrolls and confirm
    // use gl_draw_string to draw a string that contains backspace characters and confirm that the backspaces are working correctly, including when they cause wrapping to the previous line.
    // use gl_draw_string to draw a string that contains form feed characters and confirm that the form feeds are working correctly, including that they clear the screen and move the cursor to the top left

    const uint32_t TEST_BG = gl_color(0x20, 0x20, 0x40);
    gl_clear(TEST_BG);
    gl_draw_string(10, 10, "TEST BEFORE FF", GL_WHITE);
    gl_draw_string(0, 0, "\fZ", GL_AMBER);
    gl_swap_buffer();
    assert(gl_read_pixel(0, 0) == GL_AMBER);
    assert(gl_read_pixel(10, 10) == TEST_BG);
    pause("Form feed string test: screen cleared and 'Z' placed at top-left");

    // use gl_draw_rect to draw a rectangle that is larger than the screen and confirm that it is correctly clipped to the screen dimensions and that no out-of-bounds memory is accessed.


   
    
    ///Be sure to have test cases that confirm that character drawing is correctly clipped.

}

static void test_console(void) {
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");

    // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction
    pause("Console printfs");

    // Clear all lines
    console_printf("\f");

    // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
    pause("Console clear");

    /***** TODO: add more tests *****/

    /* Backspace test: type a few chars, erase some, correct, then newline */
    console_printf("BS test: ABC");
    console_printf("\b\bZ\n"); /* should end up as "BS test: AZ" */
    pause("Backspace test: should see 'BS test: AZ'");

    /* Form feed test: clear screen and print at top-left */
    console_printf("This will be cleared\n");
    console_printf("\f");
    console_printf("FF_TOP\n");
    pause("Form feed test: only 'FF_TOP' should be visible at top-left");

    /* Line wrap test: print a long string longer than 50 columns */
    const char *long_line = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    console_printf("Long line wrap test:\n");
    console_printf(long_line);
    console_printf("\n");
    pause("Line wrap test: long line should wrap across columns");

    /* Scrolling test: print more than the number of rows to force scroll */
    console_printf("Scroll test: will print 30 numbered lines\n");
    for (int i = 0; i < 30; i++) {
        char buf[32];
        /* simple itoa-ish formatting */
        int n = i + 1;
        int len = 0;
        int tmp = n;
        if (tmp == 0) buf[len++] = '0';
        else {
            char rev[16]; int r = 0;
            while (tmp > 0) { rev[r++] = '0' + (tmp % 10); tmp /= 10; }
            while (r-- > 0) buf[len++] = rev[r];
        }
        buf[len++] = '\0';
        console_printf("Line ");
        console_printf(buf);
        console_printf("\n");
    }
    pause("Scrolling test: top lines should have scrolled off; verify visually");

    /* Newline handling test: multiple consecutive newlines */
    console_printf("Newline test start\n\n\nEnd of newline test\n");
    pause("Newline test: there should be blank lines between start and end");

    ///It is a nice touch for backspace to work correctly on a wrapped row, but we won't test this specific case in grading.

    ///add test cases for backspace, form feed, line wrap and scrolling.

    /// For unit tests, we recommend you sequence together calls to console_printf and confirm the visual results on screen. Start with simple outputs and work your way up to correct handling of special characters, wrapping, and scrolling.
}

void main(void) {
    timer_init();
    uart_init();
    printf("Executing main() in test_gl_console.c\n");

    // Uncomment the call to each test function below when you have implemented
    // the functions and are ready to test them

    // test_fb();
    test_gl();
    // test_console();

    /***** TODO: Add more and better tests!  *****/
    printf("Completed main() in test_gl_console.c\n");
}
