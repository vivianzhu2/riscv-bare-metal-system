/* File: keyboard.c
 * -----------------
 *  implement a PS/2 keyboard driver

 * The keyboard module layers on that to implement the logic to process scancodes into key actions, 
 * events, and eventually typed characters.
 *  Within the keyboard module, the functionality is arranged hierarchically, each routine building 
 * on the next. The bottom level routine reads a raw scancode (by delegating to the ps2 module), 
 * the next level gathers a sequence of scancodes into one logical key action, the higher level 
 * routines translate actions into key events and typed characters.
 */
#include "keyboard.h"
#include "ps2.h"
#include "printf.h"
#include "ps2_keys.h"
#include "timer.h"

static ps2_device_t *dev;

// Initializes the keyboard driver, creating a new PS2 device connected to the given clock and data pins.
void keyboard_init(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    dev = ps2_new(clock_gpio, data_gpio);
}

// The keyboard_read_scancode function reads one scancode byte from the PS2 device.
uint8_t keyboard_read_scancode(void) {
    return ps2_read(dev);
}

/*
 * `keyboard_read_sequence`: Low level keyboard interface.
 *
 * Reads a sequence of scancode bytes corresponding to the press or
 * release of a single key. Returns a `key_action_t` struct that
 * represents the key action for the sequence read. Reads 1, 2, or 3
 * scancodes:
 *    1 byte:  ordinary key press
 *    2 bytes: ordinary key release or extended key press
 *    3 bytes: extended key release
 *
 * The `keycode` field of the returned key_action_t stores the last byte of
 * the sequence. This identifies the scancode of the PS2 key that was acted upon.
 * The `extended` field is true if this was an extended scancode, false otherwise.
 *
 * This function calls `keyboard_read_scancode` to read each scancode.
 *
 * @return      key_action_t struct containing key action for sequence
 */
key_action_t keyboard_read_sequence(void) {

    key_action_t action = { 0 };
    uint8_t next = keyboard_read_scancode();

    if(next == 0xe0){  // extended sequence, read one more byte
        // printf("got byte: %x", next);
        action.extended = 1; 
        next = keyboard_read_scancode();

    }

    if(next == 0xF0){ // release sequence, read one more byte for keycode
        action.what = KEY_RELEASE;
        next =keyboard_read_scancode();
    }
    else{
        action.what = KEY_PRESS;
    }


    action.scancode = next;
 
    return action;

}



//The mid level routine keyboard_read_event processes key actions into key events.
//It calls keyboard_read_sequence to get a key_action_t and packages the action into 
//a key_event_t struct which includes the state of keyboard modifiers and the PS/2 key 
//that was acted upon.
key_event_t keyboard_read_event(void) {

    key_event_t event = { 0 };
  
    key_action_t sequence = keyboard_read_sequence();
    
    //The modifiers field of a key_event_t reports which modifier keys are in effect. 
    static keyboard_modifiers_t mods = 0;


    ps2_key_t keyChar = ps2_key_for_scancode(sequence.scancode, sequence.extended);

    uint8_t id = keyChar.id;




    static int isCapsLock = 0;





    if(id == PS2_KEY_SHIFT && sequence.what == KEY_PRESS){ // shift is pressed down
        mods |= KEYBOARD_MOD_SHIFT; 
        // printf("next %x \n",nextId);
    }
    else{
        if(sequence.what == KEY_RELEASE && id == PS2_KEY_SHIFT){ // shift is released
            mods &= ~KEYBOARD_MOD_SHIFT;
            
        }

    }
    if(id == PS2_KEY_ALT && sequence.what == KEY_PRESS){ // alt is pressed down
        mods |= KEYBOARD_MOD_ALT; 
        // printf("next %x \n",nextId);
    }
    else{
        if(sequence.what == KEY_RELEASE && id == PS2_KEY_ALT){ // alt is released
            mods &= ~KEYBOARD_MOD_ALT;
            
        }

    }
    if(id == PS2_KEY_CTRL && sequence.what == KEY_PRESS){ // ctrl is pressed down
        mods |= KEYBOARD_MOD_CTRL; 
        // printf("next %x \n",nextId);
    }
    else{
        if(sequence.what == KEY_RELEASE && id == PS2_KEY_CTRL){ // ctrl is released
            mods &= ~KEYBOARD_MOD_CTRL;
            
        }

    }
    if(id == PS2_KEY_CAPS_LOCK && sequence.what == KEY_PRESS && isCapsLock == 0){ // caps lock is pressed down
        mods |= KEYBOARD_MOD_CAPS_LOCK; 
        // printf("next %x \n",nextId);
        isCapsLock = 1;
    }
    else if(id == PS2_KEY_CAPS_LOCK && isCapsLock == 1 && sequence.what == KEY_PRESS){ // caps lock is pressed down again, turn it off
        isCapsLock = 0;
        mods &= ~KEYBOARD_MOD_CAPS_LOCK;
    }
    else{
        if(sequence.what == KEY_RELEASE && id == PS2_KEY_CAPS_LOCK){ // caps lock is released, do nothing

            
            
        }

    }
    
    if( id ==0 || id>93){ // if the key is not a modifier, return the event with modifiers updated
        event.action = sequence;
        event.key = keyChar;
        event.modifiers = mods;
        return event;
    }
    while(sequence.what == KEY_RELEASE && (keyChar.id != 0 && keyChar.id<=93)){ // if the key is a modifier and the sequence is a release, keep reading until we get a press for a non-modifier key
        sequence =keyboard_read_sequence();
        keyChar = ps2_key_for_scancode(sequence.scancode, sequence.extended);
        id = keyChar.id;
        if(id == PS2_KEY_SHIFT && sequence.what == KEY_PRESS){
            mods |= KEYBOARD_MOD_SHIFT; 
            // printf("next %x \n",nextId);
        }
        else{
            if(sequence.what == KEY_RELEASE && id == PS2_KEY_SHIFT){ // shift is released
                mods &= ~KEYBOARD_MOD_SHIFT;
                
            }
    
        }
        if(id == PS2_KEY_ALT && sequence.what == KEY_PRESS){
            mods |= KEYBOARD_MOD_ALT; 
            // printf("next %x \n",nextId);
        }
        else{
            if(sequence.what == KEY_RELEASE && id == PS2_KEY_ALT){ // alt is released
                mods &= ~KEYBOARD_MOD_ALT;
                
            }
    
        }
        if(id == PS2_KEY_CTRL && sequence.what == KEY_PRESS){
            mods |= KEYBOARD_MOD_CTRL; 
            // printf("next %x \n",nextId);
        }
        else{
            if(sequence.what == KEY_RELEASE && id == PS2_KEY_CTRL){ // ctrl is released
                mods &= ~KEYBOARD_MOD_CTRL;
                
            }
    
        }
    }

    key_action_t next =keyboard_read_sequence(); // read one more sequence to get the key that was acted upon, this will be a press for a non-modifier key
    ps2_key_t nextChar = ps2_key_for_scancode(next.scancode, next.extended);
    

  
    event.modifiers = mods;
    event.action = next;
    event.key = nextChar;


    return event;
}


// top-level routine keyboard_read_next that translates a key press event into a typed character.
char keyboard_read_next(void) {

    key_event_t event = keyboard_read_event();
    keyboard_modifiers_t modify =  event.modifiers;
    ps2_key_t charkey = event.key;
    
    
    while(event.action.what == KEY_RELEASE){ // if the event is a release, keep reading until we get a press for a  key
        event = keyboard_read_event();
        // printf("reached %c", event.key.ch);
        modify =  event.modifiers;
        charkey = event.key;

    }
    // printf("next: %x  modify: %d  what: %d \n", charkey.ch, modify, event.action.what);
    if((modify & KEYBOARD_MOD_SHIFT) != 0 && event.action.what == KEY_PRESS){ // if shift is pressed, return the shifted character if it exists, otherwise return the unshifted character or id
       
        if(charkey.id == 0x0){ // if the key is not a special key, return the shifted character
            return charkey.ch_shifted;
        }
        else{ // if the key is a special key, return the id
            return charkey.id;
        }
        
    }
    else if((modify & KEYBOARD_MOD_ALT) != 0 && event.action.what == KEY_PRESS){ // if alt is pressed, return the unshifted character if it exists, otherwise return the id
        if(charkey.id == 0x0){ // if the key is not a special key, return the unshifted character
            return charkey.ch;
        }
        else{
            return charkey.id; // if the key is a special key, return the id
        }
    }

    else if((modify & KEYBOARD_MOD_CTRL) != 0 && event.action.what == KEY_PRESS){ // if ctrl is pressed, return the control character if it exists, otherwise return the unshifted character or id
        if(charkey.id == 0x0 ){
            if(charkey.ch >= 97 && charkey.ch <= 122){ // if the character is a letter, return the corresponding control character
                
                return charkey.ch - 96;
            }
            else{ // if the character is not a letter, return the unshifted character
                return charkey.ch; 
            }
        }
        
        else{ // if the key is a special key, return the id
            return charkey.id;
        }
    }
    else if((modify & KEYBOARD_MOD_CAPS_LOCK) != 0 && event.action.what == KEY_PRESS){ // if caps lock is on, return the shifted character for letters, otherwise return the unshifted character or id
        if(charkey.id == 0x0 ){ // if the key is not a special key, return the shifted character for letters, otherwise return the unshifted character
            if(charkey.ch >= 97 && charkey.ch <= 122){ // if the character is a letter, return the shifted character
                
                return charkey.ch_shifted; // if caps lock is on, letters are shifted
            }
            else{
                return charkey.ch;  // if the character is not a letter, caps lock does not affect it, return the unshifted character
            }
        }
        
        else{ // if the key is a special key, return the id
            return charkey.id;
        }
    }
    else{ // if no modifiers are pressed, return the unshifted character if it exists, otherwise return the id
        if(charkey.id == 0x0){  // if the key is not a special key, return the unshifted character
            return charkey.ch;
        }
        else{ // if the key is a special key, return the id
            return charkey.id;
        }
    }

    
}
