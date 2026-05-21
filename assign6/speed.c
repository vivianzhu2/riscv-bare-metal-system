/* File: extension.c
 * ------------
 * Simple main program with timing code to measure
 * performance of different implementations of redraw.
 */

#include "fb.h"
#include "gl.h"
#include "strings.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "console.h"
#include "gl.h"
#include "gl_mine.h"


// static struct { 
//     color_t bg_color, fg_color;
//     int line_height;
// } module; 
int cursor_x; // cursor_x and cursor_y represent the location of the upper left corner of the next character to be drawn
int cursor_y;
// -------------------------------------------------------------------
// Time Trial Helpers
// -------------------------------------------------------------------

static void _time_trial(const char *f_name, void (*fn)()) {
    unsigned long start = timer_get_ticks();
    fn();
    unsigned long elapsed = timer_get_ticks() - start;
    printf("\n%10ld ticks %s()\n", elapsed, f_name);
}

#define TIME_TRIAL(fn) _time_trial(#fn, fn)

// -------------------------------------------------------------------
// Baseline redraw0, using public gl_xxx functions
// -------------------------------------------------------------------

// static void redraw0(color_t color) {
//     for (int y = 0; y < gl_get_height(); y++) {
//         for (int x = 0; x < gl_get_width(); x++)  {
//             gl_draw_pixel(x, y, color);
//         }
//     }
// }

// -------------------------------------------------------------------
// redraw1, no gl_draw_pixel, write BGRA values directly into framebuffer
// -------------------------------------------------------------------

// static void redraw1(color_t color) {
//     uint8_t *ptr = fb_get_draw_buffer();
//     for (int i = 0; i < gl_get_width() * gl_get_height(); i++) {
//         *ptr++ = color & 0xff;           // blue
//         *ptr++ = (color >> 8) & 0xff;    // green
//         *ptr++ = (color >> 16) & 0xff;   // red
//         *ptr++ = 0xff;                   // alpha
//     }
// }

// -------------------------------------------------------------------
// Improved redrawN functions of your own go here:
// -------------------------------------------------------------------

// int console_printf0(const char *format, ...) {

//     const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this

//     char buf[MAX_OUTPUT_LEN];
//     int bufsize = MAX_OUTPUT_LEN;
//     va_list ap;
//     va_start(ap, format);
//     int result = vsnprintf(buf, bufsize, format, ap);
//     va_end(ap);
//     cursor_y =gl_get_height() - module.line_height; // start printing from the last line of the screen
//     cursor_x = 0;
//     // gl_draw_string(cursor_x, cursor_y, buf, module.fg_color);
//     int lenBuf = strlen(buf);
//     for(int i = 0; i < lenBuf; i ++){
//         if(buf[i] == '\b'){ // if the character is a backspace, move cursor backwards one position and clear the character at that position
           
//             // (move cursor backwards one position)
//             // if(cursor_x >= gl_get_width()){
            
//             cursor_x -= gl_get_char_width();
//             gl_draw_rect(cursor_x, cursor_y, gl_get_char_height(), gl_get_char_width(), module.bg_color);

//            // printf("after reached backspace %d \n", cursor_x);
            
//         }
//         else if(buf[i] == '\n'){ // if the character is a newline, move cursor down to the beginning of next line
            
//             cursor_y += module.line_height;
//             cursor_x = 0;

//             if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
//                 cursor_y -= module.line_height;
//                 for(int yp = module.line_height; yp < cursor_y; yp ++){
//                     for(int xp = 0; xp < gl_get_width() ; xp++){
//                         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
//                     }
//                 }
//                 // unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
//                 // for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
//                 //     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
                    
//                 // }
//                 // memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
//                 // cursor_y -= module.line_height;

//             }
            

            
//             // everytime new line, clean up new lien?
//         }
//         else if(buf[i] == '\f'){ // if the character is a form feed, clear all contents and move cursor to home position
//             // do we need to print the pi prompt
//             console_clear();
//         }
//         else{ // if the character is an ordinary character, draw the character and move cursor to the right by one character width
//             // printf("Pre wrap: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            
//             if (cursor_x + gl_get_char_width() >= gl_get_width()){ //   if the character to be inserted does not fit on the current row, automatically wrap the overflow to the next row
//                 // 
//                 cursor_y += module.line_height;
//                 // cursor_x = cursor_x % gl_get_width() ;
//                 cursor_x = 0;
//                 // printf("reached the horizontal limit: x: %d y: %d \n", cursor_x, cursor_y);
//             }
//             // printf("Pre scroll: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
//             if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
//                 // printf("reached the vertical limit: %d", cursor_y);
//                 // add scrolling
//                 // cursor_y = total % module.line_height;
//                 // unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
//                 // for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
//                 //     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
//                 //     // printf("SCROLLING");
//                 // }
//                 // memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
               
//                 cursor_y -= module.line_height;
//                 for(int yp = module.line_height; yp < cursor_y; yp ++){
//                     for(int xp = 0; xp < gl_get_width() ; xp++){
//                         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
//                     }
//                 }

//             }
    
    
//             gl_draw_char(cursor_x, cursor_y, buf[i], module.fg_color);

            
//             cursor_x += gl_get_char_width();

//         }
        
    
//      }
//     gl_swap_buffer();
    
//     return result;


// }



// -------------------------------------------------------------------
// Improved console_printf functions of your own go here:
// -------------------------------------------------------------------

// int console_printf1(const char *format, ...) {

//     const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this

//     char buf[MAX_OUTPUT_LEN];
//     int bufsize = MAX_OUTPUT_LEN;
//     va_list ap;
//     va_start(ap, format);
//     int result = vsnprintf(buf, bufsize, format, ap);
//     va_end(ap);
//     cursor_y =gl_get_height() - module.line_height; // start printing from the last line of the screen
//     cursor_x = 0;
//     // gl_draw_string(cursor_x, cursor_y, buf, module.fg_color);
//     int lenBuf = strlen(buf);
//     for(int i = 0; i < lenBuf; i ++){
//         if(buf[i] == '\b'){ // if the character is a backspace, move cursor backwards one position and clear the character at that position
           
//             // (move cursor backwards one position)
//             // if(cursor_x >= gl_get_width()){
            
//             cursor_x -= gl_get_char_width();
//             gl_draw_rect(cursor_x, cursor_y, gl_get_char_height(), gl_get_char_width(), module.bg_color);

//            // printf("after reached backspace %d \n", cursor_x);
            
//         }
//         else if(buf[i] == '\n'){ // if the character is a newline, move cursor down to the beginning of next line
            
//             cursor_y += module.line_height;
//             cursor_x = 0;

//             if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
//                 // cursor_y -= module.line_height;
//                 // for(int yp = module.line_height; yp < cursor_y; yp ++){
//                 //     for(int xp = 0; xp < gl_get_width() ; xp++){
//                 //         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
//                 //     }
//                 // }
//                 unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
//                 for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
//                     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
                    
//                 }
//                 memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
//                 cursor_y -= module.line_height;

//             }
            

            
//             // everytime new line, clean up new lien?
//         }
//         else if(buf[i] == '\f'){ // if the character is a form feed, clear all contents and move cursor to home position
//             // do we need to print the pi prompt
//             console_clear();
//         }
//         else{ // if the character is an ordinary character, draw the character and move cursor to the right by one character width
//             // printf("Pre wrap: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            
//             if (cursor_x + gl_get_char_width() >= gl_get_width()){ //   if the character to be inserted does not fit on the current row, automatically wrap the overflow to the next row
//                 // 
//                 cursor_y += module.line_height;
//                 // cursor_x = cursor_x % gl_get_width() ;
//                 cursor_x = 0;
//                 // printf("reached the horizontal limit: x: %d y: %d \n", cursor_x, cursor_y);
//             }
//             // printf("Pre scroll: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
//             if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
//                 // printf("reached the vertical limit: %d", cursor_y);
//                 // add scrolling
//                 // cursor_y = total % module.line_height;
//                 unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
//                 for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
//                     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
//                     // printf("SCROLLING");
//                 }
//                 memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
               
//                 cursor_y -= module.line_height;
//                 // for(int yp = module.line_height; yp < cursor_y; yp ++){
//                 //     for(int xp = 0; xp < gl_get_width() ; xp++){
//                 //         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
//                 //     }
//                 // }

//             }
    
    
//             gl_draw_char(cursor_x, cursor_y, buf[i], module.fg_color);

            
//             cursor_x += gl_get_char_width();

//         }
        
    
//      }
//     gl_swap_buffer();
    
//     return result;


// }


void scroll0() {

    // const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this

    // char buf[MAX_OUTPUT_LEN];
    // int bufsize = MAX_OUTPUT_LEN;
    // va_list ap;
    // va_start(ap, format);
    // int result = vsnprintf(buf, bufsize, format, ap);
    // va_end(ap);
    // cursor_y =gl_get_height() - module.line_height; // start printing from the last line of the screen
    // cursor_x = 0;
    // // gl_draw_string(cursor_x, cursor_y, buf, module.fg_color);
    // int lenBuf = strlen(buf);
    // for(int i = 0; i < lenBuf; i ++){
    //     if(buf[i] == '\b'){ // if the character is a backspace, move cursor backwards one position and clear the character at that position
           
    //         // (move cursor backwards one position)
    //         // if(cursor_x >= gl_get_width()){
            
    //         cursor_x -= gl_get_char_width();
    //         gl_draw_rect(cursor_x, cursor_y, gl_get_char_height(), gl_get_char_width(), module.bg_color);

    //        // printf("after reached backspace %d \n", cursor_x);
            
    //     }
    //     else if(buf[i] == '\n'){ // if the character is a newline, move cursor down to the beginning of next line
            
    //         cursor_y += module.line_height;
    //         cursor_x = 0;

    //         if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
    //             cursor_y -= module.line_height;
                for(int yp = 10; yp < 1080; yp ++){
                    for(int xp = 0; xp < 1080 ; xp++){
                        gl_draw_pixel(xp,yp - 10, gl_read_pixel(xp, yp));
                    }
                }
                // unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
                // for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
                //     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
                    
                // }
                // memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
                // cursor_y -= module.line_height;

    //         }
            

            
    //         // everytime new line, clean up new lien?
    //     }
    //     else if(buf[i] == '\f'){ // if the character is a form feed, clear all contents and move cursor to home position
    //         // do we need to print the pi prompt
    //         console_clear();
    //     }
    //     else{ // if the character is an ordinary character, draw the character and move cursor to the right by one character width
    //         // printf("Pre wrap: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            
    //         if (cursor_x + gl_get_char_width() >= gl_get_width()){ //   if the character to be inserted does not fit on the current row, automatically wrap the overflow to the next row
    //             // 
    //             cursor_y += module.line_height;
    //             // cursor_x = cursor_x % gl_get_width() ;
    //             cursor_x = 0;
    //             // printf("reached the horizontal limit: x: %d y: %d \n", cursor_x, cursor_y);
    //         }
    //         // printf("Pre scroll: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
    //         if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
    //             // printf("reached the vertical limit: %d", cursor_y);
    //             // add scrolling
    //             // cursor_y = total % module.line_height;
    //             // unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
    //             // for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
    //             //     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
    //             //     // printf("SCROLLING");
    //             // }
    //             // memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
               
    //             cursor_y -= module.line_height;
    //             for(int yp = module.line_height; yp < cursor_y; yp ++){
    //                 for(int xp = 0; xp < gl_get_width() ; xp++){
    //                     gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
    //                 }
    //             }

    //         }
    
    
    //         gl_draw_char(cursor_x, cursor_y, buf[i], module.fg_color);

            
    //         cursor_x += gl_get_char_width();

    //     }
        
    
    //  }
    // gl_swap_buffer();
    
    // return result;


}




void scroll1() {

    // const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this

    // char buf[MAX_OUTPUT_LEN];
    // int bufsize = MAX_OUTPUT_LEN;
    // va_list ap;
    // va_start(ap, format);
    // int result = vsnprintf(buf, bufsize, format, ap);
    // va_end(ap);
    // cursor_y =gl_get_height() - module.line_height; // start printing from the last line of the screen
    // cursor_x = 0;
    // // gl_draw_string(cursor_x, cursor_y, buf, module.fg_color);
    // int lenBuf = strlen(buf);
    // for(int i = 0; i < lenBuf; i ++){
    //     if(buf[i] == '\b'){ // if the character is a backspace, move cursor backwards one position and clear the character at that position
           
    //         // (move cursor backwards one position)
    //         // if(cursor_x >= gl_get_width()){
            
    //         cursor_x -= gl_get_char_width();
    //         gl_draw_rect(cursor_x, cursor_y, gl_get_char_height(), gl_get_char_width(), module.bg_color);

    //        // printf("after reached backspace %d \n", cursor_x);
            
    //     }
    //     else if(buf[i] == '\n'){ // if the character is a newline, move cursor down to the beginning of next line
            
    //         cursor_y += module.line_height;
    //         cursor_x = 0;

    //         if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
    //             cursor_y -= module.line_height;
                // for(int yp = 10; yp < 1080; yp ++){
                //     for(int xp = 0; xp < 1080 ; xp++){
                //         gl_draw_pixel(xp,yp - 10, gl_read_pixel(xp, yp));
                //     }
                // }
                unsigned int (*im)[1080] = fb_get_draw_buffer();
                for( int ypos = 10; ypos < 1080; ypos ++){
                    
                    memcpy(im[ypos - 10], im[ypos], 1080 * 4);
                    
                }
                memset(im[1080 - 10], 0, 1080 * 10* 4);
              //  cursor_y -= module.line_height;

    //         }
            

            
    //         // everytime new line, clean up new lien?
    //     }
    //     else if(buf[i] == '\f'){ // if the character is a form feed, clear all contents and move cursor to home position
    //         // do we need to print the pi prompt
    //         console_clear();
    //     }
    //     else{ // if the character is an ordinary character, draw the character and move cursor to the right by one character width
    //         // printf("Pre wrap: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            
    //         if (cursor_x + gl_get_char_width() >= gl_get_width()){ //   if the character to be inserted does not fit on the current row, automatically wrap the overflow to the next row
    //             // 
    //             cursor_y += module.line_height;
    //             // cursor_x = cursor_x % gl_get_width() ;
    //             cursor_x = 0;
    //             // printf("reached the horizontal limit: x: %d y: %d \n", cursor_x, cursor_y);
    //         }
    //         // printf("Pre scroll: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
    //         if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
    //             // printf("reached the vertical limit: %d", cursor_y);
    //             // add scrolling
    //             // cursor_y = total % module.line_height;
    //             // unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
    //             // for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
    //             //     memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
    //             //     // printf("SCROLLING");
    //             // }
    //             // memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
               
    //             cursor_y -= module.line_height;
    //             for(int yp = module.line_height; yp < cursor_y; yp ++){
    //                 for(int xp = 0; xp < gl_get_width() ; xp++){
    //                     gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
    //                 }
    //             }

    //         }
    
    
    //         gl_draw_char(cursor_x, cursor_y, buf[i], module.fg_color);

            
    //         cursor_x += gl_get_char_width();

    //     }
        
    
    //  }
    // gl_swap_buffer();
    
    // return result;


}


void main(void)  {
    timer_init();
    uart_init();
    gl_init(1280, 720, GL_SINGLEBUFFER);
    gl_clear(0xff666666);

    TIME_TRIAL(scroll0);
    TIME_TRIAL(scroll1);
    // TODO: Add more TIME_TRIAL calls here for your improved versions

    timer_delay(2);
}
