/* File: console.c
 * ---------------
 * A console provides a command-line text interface for entering commands and seeing output. 
 * This file implements a simple console that supports printing text, backspace, newline, form feed, 
 * horizontal wrapping, and vertical scrolling. Implement the console_printf function to support these features. 
 * 
 */
#include "console.h"
#include "gl.h"
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"


static struct { 
    color_t bg_color, fg_color;
    int line_height;
} module; 
int cursor_x; // cursor_x and cursor_y represent the location of the upper left corner of the next character to be drawn
int cursor_y;


/*
 * `console_init`: required initialization for console
 *
 * Initialize the console. The console text begins empty and
 * the cursor is in the home position (upper left corner).
 *
 * @param nrows       requested number of rows (in characters)
 * @param ncols       requested number of columns (in characters)
 * @param foreground  foreground color used for text
 * @param background  background color
 *
 * A subsequent call to console_init after the first does a re-initialization.
 */
void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    // Please add this amount of vertical space between console rows
    const static int LINE_SPACING = 5;

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.fg_color = foreground;
    module.bg_color = background;
    


    // initialize text contents to empty
    int width = ncols * gl_get_char_width();
    int height = nrows *module.line_height;// gl_get_char_height();

    //printf("WIDTH: %d HEIGHT: %d", width, height);

    gl_init(width, height, GL_DOUBLEBUFFER); 

    gl_clear(background);
    // gl_swap_buffer();
    cursor_x = 0;
    cursor_y = 0;

    // cursor is in the upper left corner
    

    // wrap stops after a certain amount of characters?? 
    // scrolling repeats the last line and there is a spaceing issue 
    // does not let continue


}

/*
 * `console_clear`
 *
 * Clear all console text and move the cursor to home position.
 */
void console_clear(void) {
    gl_clear(module.bg_color);
    cursor_x = 0;
    cursor_y = 0;
}

/*
 * `console_printf`
 *
 * Print the formatted string to the console starting at current cursor
 * position. The arguments to this function are the same as `printf`.
 * When processing characters, interpret the following special characters:
 *
 * '\n' :  newline (move cursor down to the beginning of next line)
 * '\b' :  backspace (move cursor backwards one position)
 * '\f' :  form feed (clear all contents and move cursor to home position)
 *
 * @param format    format for output string. May contain ordinary characters
 *                  and format conversions
 * @param ...       variable arguments to be converted
 * @return          count of characters written to the console
 */
//Supporting adding text, managing cursor, wrap, scrolling, backspace
//\b : backspace (move cursor backwards one position)
// \n : newline (move cursor down to first column of next row)
// \f : form feed (clear contents and move cursor to home position in upper left)

int console_printf(const char *format, ...) {

    const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this

    char buf[MAX_OUTPUT_LEN];
    int bufsize = MAX_OUTPUT_LEN;
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);

    // gl_draw_string(cursor_x, cursor_y, buf, module.fg_color);
    int lenBuf = strlen(buf);
    for(int i = 0; i < lenBuf; i ++){
        if(buf[i] == '\b'){ // if the character is a backspace, move cursor backwards one position and clear the character at that position
           
            // (move cursor backwards one position)
            // if(cursor_x >= gl_get_width()){
            
            cursor_x -= gl_get_char_width();
            gl_draw_rect(cursor_x, cursor_y, gl_get_char_height(), gl_get_char_width(), module.bg_color);

           // printf("after reached backspace %d \n", cursor_x);
            
        }
        else if(buf[i] == '\n'){ // if the character is a newline, move cursor down to the beginning of next line
            
            cursor_y += module.line_height;
            cursor_x = 0;

            if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
                // cursor_y -= module.line_height;
                // for(int yp = module.line_height; yp < cursor_y; yp ++){
                //     for(int xp = 0; xp < gl_get_width() ; xp++){
                //         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
                //     }
                // }
                unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
                for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
                    memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
                    
                }
                memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
                cursor_y -= module.line_height;

            }
            

            
            // everytime new line, clean up new lien?
        }
        else if(buf[i] == '\f'){ // if the character is a form feed, clear all contents and move cursor to home position
            // do we need to print the pi prompt
            console_clear();
        }
        else{ // if the character is an ordinary character, draw the character and move cursor to the right by one character width
            // printf("Pre wrap: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            
            if (cursor_x + gl_get_char_width() >= gl_get_width()){ //   if the character to be inserted does not fit on the current row, automatically wrap the overflow to the next row
                // 
                cursor_y += module.line_height;
                // cursor_x = cursor_x % gl_get_width() ;
                cursor_x = 0;
                // printf("reached the horizontal limit: x: %d y: %d \n", cursor_x, cursor_y);
            }
            // printf("Pre scroll: buf[i]: %c cursor x: %d \n cursor y: %d \n",buf[i], cursor_x, cursor_y);
            if(cursor_y + module.line_height > gl_get_height()){ // if the cursor is at the vertical limit, add scrolling
                // printf("reached the vertical limit: %d", cursor_y);
                // add scrolling
                // cursor_y = total % module.line_height;
                unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
                for( int ypos = module.line_height; ypos < gl_get_height(); ypos ++){
                    
                    memcpy(im[ypos - module.line_height], im[ypos], gl_get_width() * 4);
                    // printf("SCROLLING");
                }
                memset(im[gl_get_height() - module.line_height], 0, gl_get_width() * module.line_height * 4);
               
                cursor_y -= module.line_height;
                // for(int yp = module.line_height; yp < cursor_y; yp ++){
                //     for(int xp = 0; xp < gl_get_width() ; xp++){
                //         gl_draw_pixel(xp,yp - module.line_height, gl_read_pixel(xp, yp));
                //     }
                // }

            }
    
    
            gl_draw_char(cursor_x, cursor_y, buf[i], module.fg_color);

            
            cursor_x += gl_get_char_width();

        }
        
    
     }
    gl_swap_buffer();
    
    return result;


}

