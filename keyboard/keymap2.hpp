/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2011
   Author(s): Christophe Grosjean, Dominique Lafages
   Based on xrdp Copyright (C) Jay Sorg 2004-2010

   header file. Keymap2 object, used to manage key stroke events
*/

#if !defined(__KEYMAP__)
#define __KEYMAP__

#include <stdio.h>
#include "constants.hpp"
#include <string.h>
#include <string>

#include "keylayouts.hpp"

using namespace std;

static const Keylayout * keylayouts[] = { &keylayout_x00000405, &keylayout_x00000406, &keylayout_x00000407
                                        , &keylayout_x00000408, &keylayout_x00000409, &keylayout_x0000040a
                                        , &keylayout_x0000040b, &keylayout_x0000040c, &keylayout_x0000040f
                                        , &keylayout_x00000410, &keylayout_x00000413, &keylayout_x00000414
                                        , &keylayout_x00000415, &keylayout_x00000416, &keylayout_x00000418
                                        , &keylayout_x00000419, &keylayout_x0000041a, &keylayout_x0000041b
                                        , &keylayout_x0000041d, &keylayout_x0000041f, &keylayout_x00000422
                                        , &keylayout_x00000424, &keylayout_x00000425, &keylayout_x00000426
                                        , &keylayout_x00000427, &keylayout_x0000042f, &keylayout_x00000438
                                        , &keylayout_x0000043a, &keylayout_x0000043b, &keylayout_x0000043f
                                        , &keylayout_x00000440, &keylayout_x00000444, &keylayout_x00000450
                                        , &keylayout_x00000452, &keylayout_x0000046e, &keylayout_x00000481
                                        , &keylayout_x00000807, &keylayout_x00000809, &keylayout_x0000080a
                                        , &keylayout_x0000080c, &keylayout_x00000813, &keylayout_x00000816
                                        , &keylayout_x0000081a, &keylayout_x0000083b, &keylayout_x00000843
                                        , &keylayout_x0000085d, &keylayout_x00000c0c, &keylayout_x00000c1a
                                        , &keylayout_x00001009, &keylayout_x0000100c, &keylayout_x0000201a
                                        , &keylayout_x00010402, &keylayout_x00010405, &keylayout_x00001809
                                        , &keylayout_x00010407, &keylayout_x00010408, &keylayout_x0001040a
                                        , &keylayout_x0001040e, &keylayout_x00010409, &keylayout_x00010410
                                        , &keylayout_x00010415, &keylayout_x00010416, &keylayout_x00010419
                                        , &keylayout_x0001041b, &keylayout_x0001041f, &keylayout_x00010426
                                        , &keylayout_x00010427, &keylayout_x0001043a, &keylayout_x0001043b
                                        , &keylayout_x0001080c, &keylayout_x0001083b, &keylayout_x00011009
                                        , &keylayout_x00011809, &keylayout_x00020405, &keylayout_x00020408
                                        , &keylayout_x00020409, &keylayout_x0002083b, &keylayout_x00030402
                                        , &keylayout_x00030408, &keylayout_x00030409, &keylayout_x00040408
                                        , &keylayout_x00040409, &keylayout_x00050408, &keylayout_x00060408
                                        };


//##############################################################################
struct Keymap2
//##############################################################################
{
    enum {
           KBDFLAGS_EXTENDED = 0x0100
         , KBDFLAGS_DOWN     = 0x4000
         , KBDFLAGS_RELEASE  = 0x8000
         };

    enum {
           SCROLLLOCK  = 0x01
         , NUMLOCK     = 0x02
         , CAPSLOCK    = 0x04
         , FLG_SHIFT   = 0x08
         , FLG_CTRL    = 0x10
         , FLG_ALT     = 0x20
         , FLG_WINDOWS = 0x40
         , FLG_ALTGR   = 0x80
    };

    enum {
           LEFT_SHIFT  = 0x36
         , RIGHT_SHIFT = 0x2A
         , LEFT_CTRL   = 0x1D
         , RIGHT_CTRL  = 0x9D
         , LEFT_ALT    = 0x38
         , RIGHT_ALT   = 0xB8
    };

    // keyboard info
    int keys_down[256];  // key states 0 up 1 down (0..127 plain keys, 128..255 extended keys)

    int key_flags; // scroll_lock = 1, num_lock = 2, caps_lock = 4,
                   // shift = 8, ctrl = 16, Alt = 32,
                   // Windows = 64, AltGr = 128

    enum {
        SIZE_KEYBUF = 20
    };

    enum {
        SIZE_KEYBUF_KEVENT = 20
    };

    enum {
          KEVENT_KEY = 0x01
        , KEVENT_TAB = 0x02
        , KEVENT_BACKTAB = 0x3
        , KEVENT_ENTER = 0x04
        , KEVENT_ESC = 0x05
        , KEVENT_DELETE = 0x06
        , KEVENT_BACKSPACE = 0x07
        , KEVENT_LEFT_ARROW = 0x08
        , KEVENT_RIGHT_ARROW = 0x09
        , KEVENT_UP_ARROW = 0x0A
        , KEVENT_DOWN_ARROW = 0x0B
        , KEVENT_HOME = 0x0C
        , KEVENT_END = 0x0D
        , KEVENT_PGUP = 0x0E
        , KEVENT_PGDOWN = 0x0F
    };

    uint32_t ibuf; // first free position in char buffer
    uint32_t nbuf; // number of char in char buffer
    uint32_t buffer[SIZE_KEYBUF]; // actual char buffer

    uint32_t ibuf_kevent; // first free position
    uint32_t nbuf_kevent; // number of char in char buffer
    uint32_t buffer_kevent[SIZE_KEYBUF_KEVENT]; // actual char buffer

    uint32_t last_char_key;

    int last_chr_unicode;

    uint8_t deadkey;

    enum {
          DEADKEY_NONE  = 0
        , DEADKEY_FOUND = 1
    };

    const Keylayout * keylayout_WORK;  // layout suitable for the client keyboard code

    Keylayout::dkey_t deadkey_pending_def;  // object containing the current deadkey definition if any

    uint32_t verbose;


    // Constructor
    //==============================================================================
    Keymap2(uint32_t verbose = 0)
    //==============================================================================
        : ibuf(0)
        , nbuf(0)
        , ibuf_kevent(0)
        , nbuf_kevent(0)
        , deadkey(DEADKEY_NONE)
        , keylayout_WORK(&keylayout_x00000409)
        , verbose(verbose)
    //------------------------------------------------------------------------------
    {
        memset(this->keys_down, 0, 256 * sizeof(int));
        memset(this->keys_down, 0, SIZE_KEYBUF * sizeof(uint32_t));

        this->key_flags = 0;
        this->last_chr_unicode = 0;

    } // END Constructor


    //==============================================================================
    void synchronize(uint16_t param1)
    //==============================================================================
    {
        this->key_flags = param1 & 0x07;
        // non sticky keys are forced to be UP
        this->keys_down[LEFT_SHIFT] = 0;
        this->keys_down[RIGHT_SHIFT] = 0;
        this->keys_down[LEFT_CTRL] = 0;
        this->keys_down[RIGHT_CTRL] = 0;
        this->keys_down[LEFT_ALT] = 0;
        this->keys_down[RIGHT_ALT] = 0;

    } // END METHOD : synchronize


    //==============================================================================
    void update_keys_flags(int key_state)
    //==============================================================================
    {}


    //==============================================================================
    void update_chr_unicode(int key_state, int device_flags)
    //==============================================================================
    {}


// The TS_KEYBOARD_EVENT structure is a standard T.128 Keyboard Event (see [T128] section
// 8.18.2). RDP keyboard input is restricted to keyboard scancodes, unlike the code-point or virtual
// codes supported in T.128 (a scancode is an 8-bit value specifying a key location on the keyboard).
// The server accepts a scancode value and translates it into the correct character depending on the
// language locale and keyboard layout used in the session.

// keyboardFlags (2 bytes): A 16-bit, unsigned integer. The flags describing the keyboard event.

// +--------------------------+------------------------------------------------+
// | 0x0100 KBDFLAGS_EXTENDED | The keystroke message contains an extended     |
// |                          | scancode. For enhanced 101-key and 102-key     |
// |                          | keyboards, extended keys include the right     |
// |                          | ALT and right CTRL keys on the main section    |
// |                          | of the keyboard; the INS, DEL, HOME, END,      |
// |                          | PAGE UP, PAGE DOWN and ARROW keys in the       |
// |                          | clusters to the left of the numeric keypad;    |
// |                          | and the Divide ("/") and ENTER keys in the     |
// |                          | numeric keypad.                                |
// +--------------------------+------------------------------------------------+
// | 0x4000 KBDFLAGS_DOWN     | Indicates that the key was down prior to this  |
// |                          | event.                                         |
// +--------------------------+------------------------------------------------+
// | 0x8000 KBDFLAGS_RELEASE  | The absence of this flag indicates a key-down  |
// |                          | event, while its presence indicates a          |
// |                          | key-release event.                             |
// +--------------------------+------------------------------------------------+

// keyCode (2 bytes): A 16-bit, unsigned integer. The scancode of the key which
// triggered the event.


    //==============================================================================
    void event(const uint16_t keyboardFlags, const uint16_t keyCode)
    //==============================================================================
    {
        // The scancode and its extended nature are merged in a new variable (whose most significant bit indicates the extended nature)
        uint8_t extendedKeyCode = keyCode|((keyboardFlags >> 1)&0x80);
        // The state of that key is updated in the Keyboard status array (1=Make ; 0=Break)
        this->keys_down[extendedKeyCode] = !(keyboardFlags & KBDFLAGS_RELEASE);
        switch (extendedKeyCode){
        //----------------
        // Lock keys
        //----------------
            case 0x3A: // capslock
                if (this->keys_down[extendedKeyCode]){
                    this->key_flags ^= CAPSLOCK;
                }
                break;
            case 0x45: // numlock
                if (this->keys_down[extendedKeyCode]){
                    this->key_flags ^= NUMLOCK;
                }
                break;
            case 0x46: // scrolllock
                if (this->keys_down[extendedKeyCode]){
                    this->key_flags ^= SCROLLLOCK;
                }
                break;
        //----------------
        // Modifier keys
        //----------------
            case LEFT_SHIFT:  // left shift
            case RIGHT_SHIFT: // right shift
            case LEFT_CTRL:   // left ctrl
            case RIGHT_CTRL:  // right ctrl
            case LEFT_ALT:    // left alt
            case RIGHT_ALT:   // right alt
                break;
        //----------------
        // All other keys
        //----------------
            default: // all other codes
                // This table translates the RDP scancodes to X11 scancodes :
                //  - the fist block (0-127) forces some values to 0
                //  - the second block (128-255) give the code for the extended keys that have a meaningful one
                uint8_t map[256] =  {

                     0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 0x00 - 0x07
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, // 0x08 - 0x0f
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, // 0x10 - 0x17
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, // 0x18 - 0x1f
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, // 0x20 - 0x27
                    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, // 0x28 - 0x2f
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // 0x30 - 0x37
                    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, // 0x38 - 0x3f
                    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, // 0x40 - 0x47
                    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, // 0x48 - 0x4f
                    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, // 0x50 - 0x57
                    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, // 0x58 - 0x5f
                    0x60, 0x61, 0x62,  0x0,  0x0,  0x0, 0x66, 0x67, // 0x60 - 0x67
                    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, // 0x68 - 0x6f
                    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, // 0x70 - 0x77
                    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, // 0x78 - 0x7f

                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0x80 - 0x87
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0x88 - 0x8f
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0x90 - 0x97
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0x98 - 0x9f
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xa0 - 0xa7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xa8 - 0xaf
                     0x0,  0x0,  0x0,  0x0,  0x0, 0x62,  0x0,  0x0, // 0xb0 - 0xb7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xb8 - 0xbf
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xc0 - 0xc7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xc8 - 0xcf
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xd0 - 0xd7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xd8 - 0xdf
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xe0 - 0xe7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xe8 - 0xef
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xf0 - 0xf7
                     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, // 0xf8 - 0xff
                } ;

                // if event is a Make
                if (this->keys_down[extendedKeyCode]){
                    if (this->verbose){
                        LOG(LOG_INFO, "Event is Make for key: Ox%#02x", extendedKeyCode);
                    }
                        const Keylayout::KeyLayout_t * layout = &this->keylayout_WORK->noMod;
                        this->last_char_key = extendedKeyCode;

                    //-------------------------------------------------------------------------
                    // KEYPAD : Keypad keys whose meanings depends on Numlock are handled apart
                    //-------------------------------------------------------------------------
                    if ( ( (extendedKeyCode >= 0x47) && (extendedKeyCode <= 0x49) )
                      || ( (extendedKeyCode >= 0x4b) && (extendedKeyCode <= 0x4d) )
                      || ( (extendedKeyCode >= 0x4f) && (extendedKeyCode <= 0x53) )
                       ){
                        if (this->verbose){
                            LOG(LOG_INFO, "Key from keypad: 0x%02x", extendedKeyCode);
                        }

                        // if numlock is activated AND shift is up, keys are printable characters
                        if (  (this->key_flags & NUMLOCK)
                           && ( ! this->is_shift_pressed())
                           ) {
                            if (this->is_ctrl_pressed()) {
                                layout = &this->keylayout_WORK->ctrl;
                            }
                            else if(this->is_ctrl_pressed() && this->is_alt_pressed()) {
                                layout = &this->keylayout_WORK->altGr;
                            }
                            else {
                                layout = &this->keylayout_WORK->noMod;
                            }
                            // Translate the incoming RDP scancode to a X11 scancode
                            uint8_t sym = map[extendedKeyCode];
                            // Translate the X11 scancode to an unicode code point
                            uint32_t uchar = (*layout)[sym];
                            this->push(uchar);
                        }
                        // if numlock is not activated OR shift is down, keys are NOT printable characters
                        else {
                            switch (extendedKeyCode){
                               /* kEYPAD LEFT ARROW */
                                case 0x4b:
                                    this->push_kevent(KEVENT_LEFT_ARROW);
                                    break;
                                /* kEYPAD UP ARROW */
                                case 0x48:
                                    this->push_kevent(KEVENT_UP_ARROW);
                                    break;
                                /* kEYPAD RIGHT ARROW */
                                case 0x4d:
                                    this->push_kevent(KEVENT_RIGHT_ARROW);
                                    break;
                                /* kEYPAD DOWN ARROW */
                                case 0x50:
                                    this->push_kevent(KEVENT_DOWN_ARROW);
                                    break;
                                /* kEYPAD HOME */
                                case 0x47:
                                    this->push_kevent(KEVENT_HOME);
                                    break;
                                /* kEYPAD PGUP */
                                case 0x49:
                                    this->push_kevent(KEVENT_PGUP);
                                    break;
                                /* kEYPAD PGDOWN */
                                case 0x51:
                                    this->push_kevent(KEVENT_PGDOWN);
                                    break;
                                /* kEYPAD END */
                                case 0x4F:
                                    this->push_kevent(KEVENT_END);
                                    break;
                                /* kEYPAD DELETE */
                                case 0x53:
                                    this->push_kevent(KEVENT_DELETE);
                                default:
                                    break;
                            }
                        }
                    }
                    //--------------------
                    // NOT KEYPAD Specific
                    //--------------------
                    else {

                        if (this->verbose){
                            LOG(LOG_INFO, "Key not from keypad: 0x%02x", extendedKeyCode);
                        }

                        // Set the layout block to be used, depending on active modifier keys and capslock status
                        if (this->is_caps_locked()) {
                            if (this->is_ctrl_pressed() && this->is_alt_pressed() && this->is_shift_pressed()){
                                layout = &this->keylayout_WORK->capslock_shiftAltGr;
                            }
                            else if (this->is_ctrl_pressed() && this->is_alt_pressed()){
                                layout = &this->keylayout_WORK->capslock_altGr;
                            }
                            else if (this->is_ctrl_pressed()){
                                layout = &this->keylayout_WORK->ctrl;
                            }
                            else if (this->is_shift_pressed()){
                                layout = &this->keylayout_WORK->capslock_shift;
                            }
                            else{
                                layout = &this->keylayout_WORK->capslock_noMod;
                            }
                        }
                        else {
                            if (this->is_ctrl_pressed() && this->is_alt_pressed() && this->is_shift_pressed()){
                                layout = &this->keylayout_WORK->shiftAltGr;
                            }
                            else if (this->is_ctrl_pressed() && this->is_alt_pressed()){
                                layout = &this->keylayout_WORK->altGr;
                            }
                            else if (this->is_ctrl_pressed()){
                                layout = &this->keylayout_WORK->ctrl;
                            }
                            else if (this->is_shift_pressed()){

                                layout = &this->keylayout_WORK->shift;
                            }
                            else{
                                layout = &this->keylayout_WORK->noMod;
                            }
                        }
                        // Translate the RDP scancode to an X11 scancode
                        uint8_t sym = map[extendedKeyCode];
                        // Translate the X11 scancode to an unicode code point
                        uint32_t uchar = (*layout)[sym];

                        if (this->verbose){
                            LOG(LOG_INFO, "uchar=0x%02x", uchar);
                        }
                        //----------------------------------------------
                        // uchar is in Printable unicode character range
                        //----------------------------------------------
                        // That is :
                        //  * > 0x20 is for ruling out NUL, but also TAB, ESC and BACKSPACE that has unicode values but
                        //           are not actually printable characters and that we don't want to track
                        //  * And not delete (0x7f) nor a dead key (0x5e, 0xa8, 0x60)
                        if (this->verbose){
                            LOG(LOG_INFO, "nb chars in buffer: %u nb events in buffer %u\n", this->nbuf, this->nbuf_kevent);
                        }
                        if (  (uchar >= 0x20)                                               // Not an ASCII Control
                           && (uchar != 0x7F)                                               // Not the Backspace ASCII code
                           && not (this->keylayout_WORK->isDeadkey(uchar, extendedKeyCode)) // Not a deadkey
                           )
                        {
                            if (this->verbose){
                                LOG(LOG_INFO, "Printable key : uchar=Ox%x02\n", uchar);
                            }
                            // If previous key was a dead key, push a translated unicode char
                            if (this->deadkey == DEADKEY_FOUND){
                                if (this->verbose){
                                    LOG(LOG_INFO, "Dead key : uchar=0x%02x", uchar);
                                }
                                bool deadkeyTranslated = false;
                                // Search for uchar to translate in the current DEADKEY entry
                                for (uint8_t i = 0; i < this->deadkey_pending_def.nbSecondKeys; i++) {
                                     if (this->deadkey_pending_def.secondKeys[i].secondKey == uchar) {

                                        // push the translation into keyboard buffer
                                        this->push(this->deadkey_pending_def.secondKeys[i].modifiedKey);
                                        deadkeyTranslated = true;
                                        break;
                                    }
                                }
                                // If that second key is not associated with that deadkey,
                                // push both deadkey uchar and unmodified second key uchar in keyboard buffer
                                if (not deadkeyTranslated) {
                                    this->push(this->deadkey_pending_def.uchar);
                                    this->push(uchar);
                                }
                                this->deadkey = DEADKEY_NONE;
                            }
                            // If previous key wasn't a dead key, simply push
                            else {
                                if (this->verbose){
                                    LOG(LOG_INFO, "not dead key - so pushing char %02x", uchar);
                                }
                                this->push(uchar);
                            }
                        }
                        //--------------------------------------------------
                        // uchar is NOT in Printable unicode character range
                        //--------------------------------------------------
                        else {
                            if (this->verbose) {
                                LOG(LOG_INFO, "pushing event extendedKeyCode = >0x%02x<", extendedKeyCode);
                            }

                            // Test if the extendedKeyCode is a deadkey in the current keyboard layout
                            for (int i=0; i < keylayout_WORK->nbDeadkeys; i++) {
                                if (   (keylayout_WORK->deadkeys[i].uchar == uchar)
                                   and (keylayout_WORK->deadkeys[i].extendedKeyCode == extendedKeyCode)
                                   )
                                {
                                    this->deadkey = DEADKEY_FOUND;
                                    // set the deadkey definition pointer with the current deadkey definition
                                    this->deadkey_pending_def = keylayout_WORK->deadkeys[i];
                                    break;
                                }
                            }

                            if (this->deadkey == DEADKEY_NONE) {
                                switch (extendedKeyCode){
                                // LEFT ARROW
                                case 0xCB:
                                    this->push_kevent(KEVENT_LEFT_ARROW);
                                    break;
                                // UP ARROW
                                case 0xC8:
                                    this->push_kevent(KEVENT_UP_ARROW);
                                    break;
                                // RIGHT ARROW
                                case 0xCD:
                                    this->push_kevent(KEVENT_RIGHT_ARROW);
                                    break;
                                // DOWN ARROW
                                case 0xD0:
                                    this->push_kevent(KEVENT_DOWN_ARROW);
                                    break;
                                // HOME
                                case 0xC7:
                                    this->push_kevent(KEVENT_HOME);
                                    break;
                                // PGUP
                                case 0xC9:
                                    this->push_kevent(KEVENT_PGUP);
                                    break;
                                // PGDOWN
                                case 0xD1:
                                    this->push_kevent(KEVENT_PGDOWN);
                                    break;
                                // END
                                case 0xCF:
                                    this->push_kevent(KEVENT_END);
                                    break;
                                // TAB
                                case 0x0F:
                                    if (this->is_shift_pressed()){
                                        this->push_kevent(KEVENT_BACKTAB);
                                    }
                                    else {
                                        this->push_kevent(KEVENT_TAB);
                                    }
                                    break;
                                // backspace
                                case 0x0E:
                                    this->push_kevent(KEVENT_BACKSPACE);
                                    break;
                                case 0xD3: // delete
                                    this->push_kevent(KEVENT_DELETE);
                                    break;
                                case 0x53: // numpad delete
                                    this->push_kevent(KEVENT_DELETE);
                                    break;
                                case 0x1C: // enter
                                    this->push_kevent(KEVENT_ENTER);
                                    break;
                                case 0x9C: // numpad enter
                                    this->push_kevent(KEVENT_ENTER);
                                    break;
                                default:
                                    break;
                                }
                            } // IF no deadkey pending
                        } // IF printable ELSE not printable
                    } // IF Keypad ELSE not Keypad
                } // IF event is a Make
            break;

        } // END SWITCH : extendedKeyCode

    } // END METHOD : event


    //==============================================================================
    void push(uint32_t uchar)
    //==============================================================================
    {
        this->push_char(uchar);
        this->push_kevent(KEVENT_KEY);

    } // END METHOD : push


    //==============================================================================
    void push_char(uint32_t uchar)
    //==============================================================================
    {
        if (this->nbuf < SIZE_KEYBUF){
            this->buffer[this->ibuf] = uchar;
            this->ibuf++;
            if (this->ibuf >= SIZE_KEYBUF){
                this->ibuf = 0;
            }
            this->nbuf++;
        }

    } // END METHOD : push_char


    //==============================================================================
    uint32_t get_char()
    //==============================================================================
    {
        if (this->nbuf > 0){
            // remove top KEY KEVENT if present and any event may have occured before it
            if (this->nbuf_kevent > 0 && this->top_kevent() == KEVENT_KEY){
                this->nbuf_kevent--;
            }
            uint32_t res = this->buffer[(SIZE_KEYBUF + this->ibuf - this->nbuf) % SIZE_KEYBUF];

            if (this->nbuf > 0){
                this->nbuf--;
            }
            return res;
        }
        return 0;

    } // END METHOD : get_char


    // head of keyboard buffer (or keyboard buffer of size 1)
    //==============================================================================
    uint32_t top_char() const
    //==============================================================================
    {
        return this->buffer[(SIZE_KEYBUF + this->ibuf - this->nbuf) % SIZE_KEYBUF];

    } // END METHOD : top_char


    //==============================================================================
    uint32_t nb_char_available() const
    //==============================================================================
    {
        return this->nbuf;

    } // END METHOD : nb_char_available


    //==============================================================================
    void push_kevent(uint32_t uevent)
    //==============================================================================
    {
        if (this->nbuf_kevent < SIZE_KEYBUF_KEVENT){
            this->buffer_kevent[this->ibuf_kevent] = uevent;
            this->ibuf_kevent++;
            if (this->ibuf_kevent >= SIZE_KEYBUF_KEVENT){
                this->ibuf_kevent = 0;
            }
            this->nbuf_kevent++;
        }
    } // END METHOD : push_kevent


    //==============================================================================
    uint32_t get_kevent()
    //==============================================================================
    {
        uint32_t res = this->buffer_kevent[(SIZE_KEYBUF_KEVENT + this->ibuf_kevent - this->nbuf_kevent) % SIZE_KEYBUF_KEVENT];

        if (this->nbuf_kevent > 0){
            if (res == KEVENT_KEY && this->nbuf > 0){
                this->nbuf--;
            }
            this->nbuf_kevent--;
        }
        return res;

    } // END METHOD : get_kevent


    // head of keyboard buffer (or keyboard buffer of size 1)
    //==============================================================================
    uint32_t top_kevent() const
    //==============================================================================
    {
        return this->buffer_kevent[this->ibuf_kevent?this->ibuf_kevent-1:SIZE_KEYBUF_KEVENT-1];

    } // END METHOD : top_kevent


    //==============================================================================
    uint32_t nb_kevent_available() const
    //==============================================================================
    {
        return this->nbuf_kevent;

    } // END METHOD : nb_kevent_available


    //==============================================================================
    bool is_caps_locked() const
    //==============================================================================
    {
        return this->key_flags & CAPSLOCK;

    } // END METHOD : is_caps_locked


    //==============================================================================
    bool is_scroll_locked() const
    //==============================================================================
    {
        return this->key_flags & SCROLLLOCK;

    } // END METHOD : is_scroll_locked


    //==============================================================================
    bool is_num_locked() const
    //==============================================================================
    {
        return this->key_flags & NUMLOCK;

    } // END METHOD : is_num_locked


    //==============================================================================
    bool is_left_shift_pressed() const
    //==============================================================================
    {
        return this->keys_down[LEFT_SHIFT];

    } // END METHOD : is_left_shift_pressed


    //==============================================================================
    bool is_right_shift_pressed() const
    //==============================================================================
    {
        return this->keys_down[RIGHT_SHIFT];

    } // END METHOD : is_right_shift_pressed


    //==============================================================================
    bool is_shift_pressed() const
    //==============================================================================
    {
        return this->is_left_shift_pressed() || this->is_right_shift_pressed();

    } // END METHOD : is_shift_pressed


    //==============================================================================
    bool is_left_ctrl_pressed() const
    //==============================================================================
    {
        return this->keys_down[LEFT_CTRL];

    } // END METHOD : is_left_ctrl_pressed


    //==============================================================================
    bool is_right_ctrl_pressed() const
    //==============================================================================
    {
        return this->keys_down[RIGHT_CTRL];

    } // END METHOD : is_right_ctrl_pressed


    //==============================================================================
    bool is_ctrl_pressed() const
    //==============================================================================
    {
        return is_right_ctrl_pressed() || is_left_ctrl_pressed();

    } // END METHOD : is_ctrl_pressed


    //==============================================================================
    bool is_left_alt_pressed() const
    //==============================================================================
    {
        return this->keys_down[LEFT_ALT];

    } // END METHOD : is_left_alt_pressed


    //==============================================================================
    bool is_right_alt_pressed() const // altGr
    //==============================================================================
    {
        return this->keys_down[RIGHT_ALT];

    } // END METHOD : is_right_alt_pressed


    //==============================================================================
    bool is_alt_pressed() const
    //==============================================================================
    {
        return is_right_alt_pressed() || is_left_alt_pressed();

    } // END METHOD : is_alt_pressed


    //==============================================================================
    void init_layout(int LCID)
    //==============================================================================
    {
        bool found = false;
        for (uint8_t i = 0 ; i < sizeof(keylayouts)/sizeof(keylayouts[0]); i++) {
            if (keylayouts[i]->LCID == LCID){
                keylayout_WORK = keylayouts[i];
                found = true;
                break;
            }
        }
        if (!found){
            LOG(LOG_INFO, "Unknown keyboard layout #0x%02x. Reverting to default (English - United States)", LCID);
        }

    } // END METHOD - init_layout


}; // END STRUCT - Keymap2

#endif
