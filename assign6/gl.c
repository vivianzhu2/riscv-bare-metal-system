/* File: gl.c
 * ----------
 * The graphics library module layers on the framebuffer 
 * and provides higher-level drawing primitives to set and 
 * get the color of a pixel, draw filled rectangles, and display text.
 */
#include "gl.h"
#include "gl_mine.h"
#include "strings.h"
#include "font.h"

/*
 * `gl_init` : Required initialization for graphics library
 *
 * Initialize the graphic library. This function will call `fb_init` in turn
 * to initialize the framebuffer.
 *
 * @param width     requested framebuffer width (in pixels)
 * @param height    requested framebuffer height (in pixels)
 * @param mode      whether framebuffer should be
 *                  single buffered (GL_SINGLEBUFFER)
 *                  or double buffered (GL_DOUBLEBUFFER)
 *
 * A subsequent call to gl_init after the first does a re-initialization.
 */
void gl_init(int width, int height, gl_mode_t mode) {
    fb_init(width, height, mode);
}

/*
 * `gl_get_width`
 *
 * Get the current width of the framebuffer.
 *
 * @return    the width (in pixels)
 */
int gl_get_width(void) {
  
    return fb_get_width();
}


/*
 * `gl_get_height`
 *
 * Get the current height of the framebuffer.
 *
 * @return    the height (in pixels)
 */
int gl_get_height(void) {
    return fb_get_height();
}

/*
 * `gl_color`
*
 * Returns a color composed of the specified red, green, and
 * blue components. The alpha component of the color will be
 * set to 0xff (fully opaque).
 *
 * @param r     red component of color
 * @param g     green component of color
 * @param b     blue component of color
 *
 * @return   the color as a single value of type color_t
 */
color_t gl_color(uint8_t r, uint8_t g, uint8_t b) {

    int opaque = 0xff;
    int color = 0xff000000;
    int maskr =  0x00ff0000;
    int shiftr = r << 16;
    int maskg = 0x0000ff00;
    int shiftg = g << 8;
    int maskb = 0x000000ff;
    int shiftb = b << 0;

    color &= ~maskr;
    color |= shiftr;
    color &= ~maskg;
    color |= shiftg;
    color &= ~maskb;
    color |= shiftb;
    return color;


}

/*
 * `gl_swap_buffer`
 *
 * If in double-buffered mode, all gl drawing takes place in the
 * off-screen buffer and updated drawing is not brought on-screen until
 * a call is made to `gl_swap_buffer` to exchange the on-screen
 * and off-screen buffers.
 *
 * If not in double-buffer mode, all drawing takes place on-screen and
 * the `gl_swap_buffer` function has no effect.
 */
void gl_swap_buffer(void) {
    /***** TODO: Your code goes here *****/
    fb_swap_buffer();
}

/*
 * `gl_clear`
 *
 * Clear all the pixels in the framebuffer to the given color.
 *
 * @param c     color to set all pixels
 */
void gl_clear(color_t c) {
    for(int i = 0; i < gl_get_width(); i++){
        for(int j = 0; j < gl_get_height(); j++){
           gl_draw_pixel(i, j, c);
        }
    }
}

/*
 * `gl_draw_pixel`
 *
 * Draw a single pixel at location x,y in color c.
 * If the location is outside the bounds of framebuffer, it is not drawn.
 *
 * @param x     x location of pixel
 * @param y     y location of pixel
 * @param c     color of pixel
 */
void gl_draw_pixel(int x, int y, color_t c) {
    if(x < 0 || x >= gl_get_width() || y < 0 || y >= gl_get_height()){ // if location is outside bounds of framebuffer, do not draw the pixel
        return;
    }
    else{ // if location is within bounds of framebuffer, draw the pixel in color c
        unsigned int per_row = fb_get_width();
        unsigned int (*im)[per_row] = fb_get_draw_buffer();
        im[y][x] = c;
    }

    

}

/*
 * `gl_read_pixel`
 *
 * Return the color of the pixel at location x,y. Returns 0 if the
 * location is outside the bounds of the framebuffer.
 *
 * @param x     x location of pixel
 * @param y     y location of pixel
 *
 * @return   the color at that location
 */
color_t gl_read_pixel(int x, int y) {

    if(x < 0 || x >= gl_get_width() || y < 0 || y >= gl_get_height()){ // if location is outside bounds of framebuffer, return 0
        return 0;
    }
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    return im[y][x];
}

/*
 * `gl_draw_rect`
 *
 * Draw a filled rectangle at location x,y with size w,h filled with color c.
 * All pixels in the rectangle that lie within the bounds of the
 * framebuffer are drawn. Any pixel that lies outside is clipped (i.e. not drawn).
 *
 * @param x     x location of upper left corner of the rectangle
 * @param y     y location of upper left corner of the rectangle
 * @param w     width of rectangle
 * @param h     height of rectangle
 * @param c     color of rectangle
 */
void gl_draw_rect(int x, int y, int w, int h, color_t c) {

    if( (x+w) >= gl_get_width() || (y+h) >= gl_get_height() || (x+w) < 0 || (y+h) < 0){ // if rectangle is outside bounds of framebuffer, only draw the part that is within bounds
        for(int i = x; i < gl_get_width(); i ++){
            for(int j = y; j < gl_get_height(); j++){
                gl_draw_pixel(i,j,c);
            }
        }
    }
    else{ // if rectangle is within bounds of framebuffer, draw the whole rectangle
        for(int k = x; k < x + w; k ++){
            for(int m = y; m < y + h; m++){
                gl_draw_pixel(k,m,c);
            }
        }
   
    }
}

/*
 * `gl_draw_char`
 *
 * Draw a single character at location x,y in color c.
 * Only those pixels of the character that lie within the bounds
 * of the framebuffer are drawn. Any pixel that lies outside is
 * clipped (i.e. not drawn). Only the "on" pixels of the character
 * are drawn, all "off" pixels are left as-is.
 *
 * @param x     x location of upper left corner of character glyph
 * @param y     y location of upper left corner of character glyph
 * @param ch    character to be drawn, e.g. 'a'. If this character has no glyph
 *              in the current font, nothing is drawn (refer to font_get_glyph())
 * @param c     color of the character
 */
void gl_draw_char(int x, int y, char ch, color_t c) {

    int nbytes = gl_get_char_height() * gl_get_char_width();
    uint8_t buffer[nbytes];
    font_get_glyph(ch, buffer, nbytes);
    for(int i  = 0; i < gl_get_char_width(); i++){
        for(int j = 0; j < gl_get_char_height(); j ++){
            if (buffer[j * gl_get_char_width() + i] == 0xFF) { // if the pixel is "on", draw the pixel in color c
                gl_draw_pixel(x + i, y + j,c);
            }
        }
    }
}

/*
 * `gl_draw_string`
 *
 * Draw a string at location x,y in color c. The characters are drawn
 * left to right in a single line. Only the pixels of the characters
 * that lie within the bounds of the framebuffer are drawn. Any pixel
 * that lies outside is clipped (i.e. not drawn). Only the "on" pixels of
 * the characters are drawn, all "off" pixels are left as-is.
 *
 * @param x     x location of upper left corner of the first char of string
 * @param y     y location of upper left corner of the first char of string
 * @param str   null-terminated string to be drawn
 * @param c     color of the string
 */
void gl_draw_string(int x, int y, const char* str, color_t c) {
    int gap = 0; 
    for(int i = 0; i < strlen(str); i ++){ // for each character in the string, draw the character and add gap between characters
        if(str[i] == ' '){ // if character is a space, add gap between characters but do not draw anything
            gap += gl_get_char_width();
        }
        gl_draw_char(x + gap, y, str[i], c);
        gap += gl_get_char_width();
        //add gap between characters

    }
}



/*
 * `gl_get_char_height`
 *
 * Get the height in pixels of a single character glyph.
 *
 * @return  the height of character glyph in pixels
 */
int gl_get_char_height(void) {
    
    return font_get_glyph_height();
}

/*
 * `gl_get_char_width`
 *
 * Get the width in pixels of a single character glyph.
 *
 * @return  the width of character glyph in pixels
 */
int gl_get_char_width(void) {
    return font_get_glyph_width();
}
