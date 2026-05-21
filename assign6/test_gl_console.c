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
    // assert(hdmi_best_match(WIDTH, HEIGHT) != HDMI_INVALID);
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
    gl_clear(GL_BLUE);
    gl_draw_string(150, 50, "TEST", GL_WHITE);
    gl_swap_buffer();

    //ssert-based unit tests that make a gl_draw_xxx call followed 
    // by calls to gl_read_pixel to confirm the pixel color at a location.
    //asserts for edge cases or specific details that are not easily to visually confirm
    gl_draw_pixel(WIDTH-1, 0, GL_GREEN);
    assert(gl_read_pixel(WIDTH-1, 0) == GL_GREEN);

    gl_draw_pixel(0, HEIGHT-1, GL_BLUE);
    assert(gl_read_pixel(0, HEIGHT-1) == GL_BLUE);

    gl_draw_pixel(WIDTH-1, HEIGHT-1, GL_WHITE);
    assert(gl_read_pixel(WIDTH-1, HEIGHT-1) == GL_WHITE);
    pause("Added boundary pixel cases");

    
    gl_draw_string(0, 0, "X", GL_AMBER);
    gl_swap_buffer();

    /* previous text position should now be background color */
    assert(gl_read_pixel(150, 50) == GL_BLUE);
    /* top-left should contain the new character (at least its top-left pixel) */
    pause("Letter case");
   
   
    gl_draw_pixel(-1, 0, GL_RED);
    gl_draw_pixel(0, -1, GL_RED);
    gl_draw_pixel(WIDTH, 0, GL_RED);
    gl_draw_pixel(0, HEIGHT, GL_RED); 
    
    gl_swap_buffer();
    assert(gl_read_pixel(-1, 0) == 0);
    assert(gl_read_pixel(0, -1) == 0);
    assert(gl_read_pixel(WIDTH, 0) == 0);
    assert(gl_read_pixel(0, HEIGHT) == 0);

    pause("Added not in boundary pixel cases");


    gl_clear(GL_BLACK);
    gl_draw_rect(10, 10, 50, 30, GL_CYAN);
    gl_swap_buffer();
    // corners of rect should be cyan
    assert(gl_read_pixel(10, 10)   == GL_CYAN);
    assert(gl_read_pixel(59, 10)   == GL_CYAN);  // x+w-1
    assert(gl_read_pixel(10, 39)   == GL_CYAN);  // y+h-1
    assert(gl_read_pixel(59, 39)   == GL_CYAN);
    // just outside rect should still be black
    assert(gl_read_pixel(9, 10)    == GL_BLACK);
    assert(gl_read_pixel(60, 10)   == GL_BLACK);
    assert(gl_read_pixel(10, 9)    == GL_BLACK);
    assert(gl_read_pixel(10, 40)   == GL_BLACK);
    pause("Added rectangle pixel cases");


    gl_clear(GL_BLACK);
    gl_draw_rect(WIDTH - 20, HEIGHT - 20, 100, 100, GL_YELLOW); // hangs off two edges
    gl_swap_buffer();
    assert(gl_read_pixel(WIDTH - 20, HEIGHT - 20) == GL_YELLOW); // ****
    assert(gl_read_pixel(WIDTH - 1,  HEIGHT - 1)  == GL_YELLOW);

    pause("Added boundary rectangle cases");

    int cw = gl_get_char_width();
    int ch_h = gl_get_char_height();
    assert(cw > 0);
    assert(ch_h > 0);
    

    gl_swap_buffer();
    gl_clear(GL_BLACK);
    gl_draw_char(0, 0, 'Z', GL_WHITE);          // top-left corner
    gl_draw_char(WIDTH - 1, 0, 'Z', GL_WHITE);  // partially clipped right
    gl_draw_char(0, HEIGHT - 1, 'Z', GL_WHITE);
    pause("Added font width checks");

    gl_clear(GL_BLACK);
    gl_draw_string(0, 0, "CS107E", GL_AMBER);
    assert(gl_read_pixel(6 * cw + 5, 0) == GL_BLACK);
    // gl_draw_char(160, 50, 'I', GL_AMBER);
    // gl_swap_buffer();
    pause("Added font checks");
    
    gl_clear(GL_BLACK);
    int flag_x = WIDTH/2 - 150;
    int flag_y = HEIGHT/2 - 100;
    int stripe_w = 100;
    int flag_h = 200;
    
    gl_draw_rect(flag_x,             flag_y, stripe_w, flag_h, GL_GREEN);
    gl_draw_rect(flag_x + stripe_w,  flag_y, stripe_w, flag_h, GL_WHITE);
    gl_draw_rect(flag_x + stripe_w*2,flag_y, stripe_w, flag_h, GL_RED);
    
    // verify one pixel from each stripe
    assert(gl_read_pixel(flag_x + 5,             flag_y + 5) == GL_GREEN);
    assert(gl_read_pixel(flag_x + stripe_w + 5,  flag_y + 5) == GL_WHITE);
    assert(gl_read_pixel(flag_x + stripe_w*2 + 5,flag_y + 5) == GL_RED);
    
    gl_swap_buffer();
    pause("Italian flag: green, white, red vertical stripes");
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

    // test cases that confirm that character drawing is correctly clipped.
  
     // use gl_draw_string to draw a string that is longer than the width of the screen to confirm that wrapping and clipping are working correctly for character drawing.
    gl_clear(BG_COLOR);
    const char *long_string = "This is a very long string that should clip at the screen boundaries";
    gl_draw_string(0, 0, long_string, GL_WHITE);
    gl_swap_buffer();
    /* Verify text is drawn at top-left */

    printf("0,0 color: %x", gl_read_pixel(0, 0));
    assert(gl_read_pixel(cw/2, ch_h/2) == GL_WHITE);

    pause("Long string test: string wraps within screen boundaries and is clipped correctly");

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
    for (int i = 0; i < 40; i++) {
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