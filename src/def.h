#ifndef _DEF_H_
#define _DEF_H_

//~ #define INHIBIT(X) X
#define INHIBIT(X) /* X */

#define SCREEN_WIDTH          320
#define SCREEN_HEIGHT         240
#define SCREEN_BPP            16
#define SURFACE_FLAGS         SDL_SWSURFACE
#define MS_PER_FRAME          33
// Panel
#define LINE_HEIGHT           15
#define NB_VISIBLE_LINES      14
#define Y_LIST                17
#define Y_HEADER              3
#define Y_FOOTER              228
#define H_FOOTER              13
// Dialogs
#define DIALOG_BORDER         2
#define DIALOG_MARGIN         8
// Colors
#define COLOR_KEY             255,0,255
#define COLOR_TEXT_NORMAL     70,27,10
#define COLOR_TEXT_TITLE      233,229,227
#define COLOR_TEXT_DIR        75,70,164
#define COLOR_TEXT_SELECTED   255,0,0
#define COLOR_CURSOR_1        232,152,80
#define COLOR_CURSOR_2        232,201,173
#define COLOR_BG_1            255,255,255
#define COLOR_BG_2            232,228,224
#define COLOR_BORDER          102,85,74

#if defined(PLATFORM_DINGOO)
// Keys for Dingoo
#define MYKEY_UP            SDLK_UP           // Up
#define MYKEY_RIGHT         SDLK_RIGHT        // Right
#define MYKEY_DOWN          SDLK_DOWN         // Down
#define MYKEY_LEFT          SDLK_LEFT         // Left
#define MYKEY_SYSTEM        SDLK_LSHIFT       // Y
#define MYKEY_PAGEUP        SDLK_TAB          // L
#define MYKEY_PAGEDOWN      SDLK_BACKSPACE    // R
#define MYKEY_OPEN          SDLK_LCTRL        // A
#define MYKEY_PARENT        SDLK_LALT         // B
#define MYKEY_OPERATION     SDLK_SPACE        // X
#define MYKEY_SELECT        SDLK_ESCAPE       // SELECT
#define MYKEY_TRANSFER      SDLK_RETURN       // START
#define PATH_DEFAULT        "/boot/local"
#define FILE_SYSTEM         "/dev/mmcblk0p1"
#elif defined(PLATFORM_GCW0)
// Keys for GCW Zero
#define MYKEY_UP            SDLK_UP           // Up
#define MYKEY_RIGHT         SDLK_RIGHT        // Right
#define MYKEY_DOWN          SDLK_DOWN         // Down
#define MYKEY_LEFT          SDLK_LEFT         // Left
#define MYKEY_SYSTEM        SDLK_LSHIFT       // X
#define MYKEY_PAGEUP        SDLK_TAB          // L
#define MYKEY_PAGEDOWN      SDLK_BACKSPACE    // R
#define MYKEY_OPEN          SDLK_LCTRL        // A
#define MYKEY_PARENT        SDLK_LALT         // B
#define MYKEY_OPERATION     SDLK_SPACE        // Y
#define MYKEY_SELECT        SDLK_ESCAPE       // SELECT
#define MYKEY_TRANSFER      SDLK_RETURN       // START
#define PATH_DEFAULT        "/"
#define FILE_SYSTEM         "/dev/mmcblk0p1"
#elif defined(PLATFORM_RG35XX)
// Keys for RG35XX
#define MYKEY_UP            SDLK_w            // Up
#define MYKEY_RIGHT         SDLK_d            // Right
#define MYKEY_DOWN          SDLK_s            // Down
#define MYKEY_LEFT          SDLK_q            // Left
#define MYKEY_SYSTEM        SDLK_y            // Y
#define MYKEY_PAGEUP        SDLK_h            // L
#define MYKEY_PAGEDOWN      SDLK_l            // R
#define MYKEY_OPEN          SDLK_a            // A
#define MYKEY_PARENT        SDLK_b            // B
#define MYKEY_OPERATION     SDLK_x            // X
#define MYKEY_SELECT        SDLK_n            // SELECT
#define MYKEY_TRANSFER      SDLK_m            // START
#define PATH_DEFAULT        "/mnt/mmc/Roms"
#define FILE_SYSTEM         "/dev/mmcblk0p4"
#else
// Keys for PC keyboard
#define MYKEY_UP            SDLK_UP
#define MYKEY_RIGHT         SDLK_RIGHT
#define MYKEY_DOWN          SDLK_DOWN
#define MYKEY_LEFT          SDLK_LEFT
#define MYKEY_SYSTEM        SDLK_q
#define MYKEY_PAGEUP        SDLK_PAGEUP
#define MYKEY_PAGEDOWN      SDLK_PAGEDOWN
#define MYKEY_OPEN          SDLK_RETURN
#define MYKEY_PARENT        SDLK_BACKSPACE
#define MYKEY_OPERATION     SDLK_a
#define MYKEY_SELECT        SDLK_INSERT
#define MYKEY_TRANSFER      SDLK_w
#define PATH_DEFAULT        "/"
#define FILE_SYSTEM         "/dev/sda4"
#endif

#endif
