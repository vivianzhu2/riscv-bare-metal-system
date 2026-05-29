/* File: fb.c
 * ----------
 * The base layer of graphics support is the framebuffer, which is a contiguous block of memory 
 * that represents the pixels on the screen.
 * The fb module allocates and manages the framebuffer memory and coordinates with the lower-level
 *  de and hdmi modules to display the framebuffer on an HDMI monitor.

 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"
#include "printf.h"
// module-level variables, you may add/change this struct as you see fit
static struct{
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
    void *framebuffer;     // address of framebuffer memor
    
    void *buffers[2];
    int current;

    fb_mode_t mode;

} module;


/*
 * `fb_init` : Required initialization for framebuffer
 *
 * Initialize the framebuffer.
 *
 * @param width     requested framebuffer width (in pixels)
 * @param height    requested framebuffer height (in pixels)
 * @param mode      whether framebuffer should be
 *                      single buffered (FB_SINGLEBUFFER)
 *                      or double buffered (FB_DOUBLEBUFFER)
 *
 * The depth of the framebuffer is always 4 bytes; each pixel is
 * 32-bit value in format BGRA.
 *
 * If the requested size can be successfully accommodated by the display
 * hardware peripherals, the function returns normally. Otherwise an assert
 * is raised within de/hdmi to report the incompatibility.
 *
 * A subsequent call to fb_init after the first does a re-initialization.
 * All previous framebuffer memory is deallocated and the framebuffer is
 * reset for the requested configuration.
 *
 * The contents of a newly created framebuffer are unspecified. Although an
 * implementation might choose to fill contents as a debugging aid, it
 * is not required to do so.
 */

//fb_init allocates the framebuffer memory for the requested width and height and fixed 32-bit depth 
// that supports a 4-byte BGRA color for each pixel. Add support framebuffer in double-buffered mode. 
//In this mode, two separate framebuffers are allocated. The active buffer is the one currently displayed 
//on-screen; the other buffer that is off-screen is used as the 'draw' buffer. In double-buffered mode, 
//all drawing is done to the off-screen draw buffer. When drawing is complete, the client calls fb_swap_buffer 
//to bring the draw buffer on-onscreen. A single swap of which buffer is active exchanges the two buffers.

//A second or subsequent call to fb_init should perform a re-initialization. Any previous memory is deallocated 
//and the framebuffer is reset for the requested configuration.

void fb_init(int width, int height, fb_mode_t mode) {


    module.width = width;
    module.height = height;
    module.depth = 4;
    int nbytes = module.width * module.height * module.depth;
    
    // struct display second;
    module.mode = mode;
    module.current = 0;
    if(mode == FB_DOUBLEBUFFER){ // allocate two buffers
 
        if(module.buffers[0] != NULL) // check if buffers already allocated
        {
            free(module.buffers[0]); // deallocate previous memory
        }
        
        if(module.buffers[1] != NULL){ // check if buffers already allocated
            free(module.buffers[1]); // deallocate previous memory
            // wpuld you only nned to free the draw buffer? 
        
        }
        if(module.framebuffer != NULL){ // check if framebuffer already allocated
            free(module.framebuffer); // deallocate previous memory
        }
        module.buffers[0] = malloc(nbytes);
        memset(module.buffers[0], 0x0, nbytes);
        module.buffers[1] = malloc(nbytes);
        memset(module.buffers[1], 0x0, nbytes);

        // printf("init  %p buffer[0]: %x", module.buffers[0], *(char*)module.buffers[0]);
        // printf("init  %p buffer[1]: %x", module.buffers[1], *(char*)module.buffers[1]);
    }
    else{ // allocate single buffer
        if(module.framebuffer != NULL){ // check if framebuffer already allocated
            free(module.framebuffer); // deallocate previous memory
        }

        if(module.buffers[0] != NULL) // check if buffers already allocated
        {
            free(module.buffers[0]); // deallocate previous memory
        }
        
        if(module.buffers[1] != NULL){ // check if buffers already allocated
            free(module.buffers[1]); // deallocate previous memory
            // wpuld you only nned to free the draw buffer? 
        
        }
        module.framebuffer = malloc(nbytes);
        memset(module.framebuffer, 0x0, nbytes);
    }
   
   


    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
    if(mode == FB_SINGLEBUFFER){ // set active framebuffer to single buffer
        de_set_active_framebuffer(module.framebuffer);
    }
    else{ // set active framebuffer to first buffer in double buffer mode
        de_set_active_framebuffer(module.buffers[0]);
    }

    

     
}



/*
 * `fb_get_width`
 *
 * Get the current width in pixels of the framebuffer.
 *
 * @return    the width in pixels
 */
int fb_get_width(void) {

    return module.width;
}

/*
 * `fb_get_height`
 *
 * Get the current height in pixels of the framebuffer.
 *
 * @return    the height in pixels
 */
int fb_get_height(void) {
    return module.height;
}

/*
 * `fb_get_depth`
 *
 * Get the current depth in bytes of a single pixel.
 * In our case, depth is always 4.
 *
 * @return    the depth in bytes
 */
int fb_get_depth(void) {
    return module.depth;
}

/*
 * `fb_get_draw_buffer`
 *
 * Get the start address of the framebuffer memory into which the
 * client can draw pixels. The address returned is the start of an
 * array of bytes of capacity width*height*depth.
 *
 * If in single buffering mode, there is only one buffer in use, so the
 * returned address does not change. That buffer is on-screen at all
 * times. The client draws to that one buffer, and all updates are
 * immediately displayed.
 *
 * In double buffering mode, there are two buffers: the one
 * currently on-screen and a second buffer that is off-screen. The
 * address returned by `fb_get_draw_buffer` corresponds to the buffer
 * that is currently off-screen. The off-screen buffer is sometimes
 * called the draw buffer. The client will do all drawing to the off-screen
 * buffer and when ready, calls `fb_swap_buffer` to exchange the
 * on-screen and off-screen buffers. The swap brings the updated
 * drawing on-screen in one smooth update.
 *
 * Note the address is returned as `void*`. Client should store into
 * a properly typed pointer so as to access the pixel data according
 * to their desired scheme (1-d, 2-d, etc.)
 *
 * @return    the address of the current draw (off-screen) buffer
 */
void* fb_get_draw_buffer(void){

    if(module.mode ==FB_DOUBLEBUFFER ){ // return off-screen buffer in double buffer mode
        // printf("module buff 1  %p \n", module.buffers[1]);
        // printf("module buff 0 %p \n", module.buffers[0]);
        return module.buffers[1];
        

    }
    else{ // return single buffer in single buffer mode
        return module.framebuffer;
    }


}

/*
 * `fb_swap_buffer`
 *
 * Fast exchange of front (on-screen) and back (off-screen) buffers.
 * The buffer that was previously off-screen is now displayed on-screen
 * and buffer that was on-screen moves off-screen.
 *
 * If not in double buffering mode, there is only one buffer and this
 * function has no effect.
 */
void fb_swap_buffer(void) {

    // fast exchange of front and back (off-screen) buffers
    // buffer offscreen is now on screen
    
    void* temp = module.buffers[0];
    module.buffers[0] = module.buffers[1]; 
    module.buffers[1] = temp;
    if(module.current == 0){ // swap buffers and set current to 1
        module.current = 1;
    }
    else{ // swap buffers and set current to 0
        module.current = 0;
    }
    // printf("current: %d", module.current);

}
