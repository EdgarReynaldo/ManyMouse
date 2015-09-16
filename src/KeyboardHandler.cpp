

#include "KeyboardHandler.hpp"



static const unsigned char vk_to_al_keycode[256] =
{
   /* 0x00 */    0,                       ALLEGRO_KEY_UNKNOWN+0,     ALLEGRO_KEY_UNKNOWN+1,      ALLEGRO_KEY_UNKNOWN+2,
   /* 0x04 */    ALLEGRO_KEY_UNKNOWN+3,   ALLEGRO_KEY_UNKNOWN+4,     ALLEGRO_KEY_UNKNOWN+5,      0,
   /* 0x08 */    ALLEGRO_KEY_BACKSPACE,   ALLEGRO_KEY_TAB,           0,                          0,
   /* 0x0C */    ALLEGRO_KEY_PAD_5,       ALLEGRO_KEY_ENTER,         0,                          0,
   /* 0x10 */    0/*L or R shift*/,       ALLEGRO_KEY_LCTRL,         ALLEGRO_KEY_ALT,            ALLEGRO_KEY_PAUSE,
   /* 0x14 */    ALLEGRO_KEY_CAPSLOCK,    ALLEGRO_KEY_KANA,          0,                          ALLEGRO_KEY_UNKNOWN+6,
   /* 0x18 */    ALLEGRO_KEY_UNKNOWN+7,   ALLEGRO_KEY_KANJI,         0,                          ALLEGRO_KEY_ESCAPE,
   /* 0x1C */    ALLEGRO_KEY_CONVERT,     ALLEGRO_KEY_NOCONVERT,     ALLEGRO_KEY_UNKNOWN+8,      ALLEGRO_KEY_UNKNOWN+9,
   /* 0x20 */    ALLEGRO_KEY_SPACE,       ALLEGRO_KEY_PAD_9,         ALLEGRO_KEY_PAD_3,          ALLEGRO_KEY_PAD_1,
   /* 0x24 */    ALLEGRO_KEY_PAD_7,       ALLEGRO_KEY_PAD_4,         ALLEGRO_KEY_PAD_8,          ALLEGRO_KEY_PAD_6,
   /* 0x28 */    ALLEGRO_KEY_PAD_2,       ALLEGRO_KEY_UNKNOWN+10,    ALLEGRO_KEY_UNKNOWN+11,     ALLEGRO_KEY_UNKNOWN+12,
   /* 0x2C */    ALLEGRO_KEY_PRINTSCREEN, ALLEGRO_KEY_PAD_0,         ALLEGRO_KEY_PAD_DELETE,     ALLEGRO_KEY_UNKNOWN+13,
   /* 0x30 */    ALLEGRO_KEY_0,           ALLEGRO_KEY_1,             ALLEGRO_KEY_2,              ALLEGRO_KEY_3,
   /* 0x34 */    ALLEGRO_KEY_4,           ALLEGRO_KEY_5,             ALLEGRO_KEY_6,              ALLEGRO_KEY_7,
   /* 0x38 */    ALLEGRO_KEY_8,           ALLEGRO_KEY_9,             0,                          0,
   /* 0x3C */    0,                       0,                         0,                          0,
   /* 0x40 */    0,                       ALLEGRO_KEY_A,             ALLEGRO_KEY_B,              ALLEGRO_KEY_C,
   /* 0x44 */    ALLEGRO_KEY_D,           ALLEGRO_KEY_E,             ALLEGRO_KEY_F,              ALLEGRO_KEY_G,    
   /* 0x48 */    ALLEGRO_KEY_H,           ALLEGRO_KEY_I,             ALLEGRO_KEY_J,              ALLEGRO_KEY_K,
   /* 0x4C */    ALLEGRO_KEY_L,           ALLEGRO_KEY_M,             ALLEGRO_KEY_N,              ALLEGRO_KEY_O,
   /* 0x50 */    ALLEGRO_KEY_P,           ALLEGRO_KEY_Q,             ALLEGRO_KEY_R,              ALLEGRO_KEY_S,
   /* 0x54 */    ALLEGRO_KEY_T,           ALLEGRO_KEY_U,             ALLEGRO_KEY_V,              ALLEGRO_KEY_W,
   /* 0x58 */    ALLEGRO_KEY_X,           ALLEGRO_KEY_Y,             ALLEGRO_KEY_Z,              ALLEGRO_KEY_LWIN,
   /* 0x5C */    ALLEGRO_KEY_RWIN,        ALLEGRO_KEY_MENU,          0,                          0,
   /* 0x60 */    ALLEGRO_KEY_PAD_0,       ALLEGRO_KEY_PAD_1,         ALLEGRO_KEY_PAD_2,          ALLEGRO_KEY_PAD_3,
   /* 0x64 */    ALLEGRO_KEY_PAD_4,       ALLEGRO_KEY_PAD_5,         ALLEGRO_KEY_PAD_6,          ALLEGRO_KEY_PAD_7,
   /* 0x68 */    ALLEGRO_KEY_PAD_8,       ALLEGRO_KEY_PAD_9,         ALLEGRO_KEY_PAD_ASTERISK,   ALLEGRO_KEY_PAD_PLUS,
   /* 0x6C */    ALLEGRO_KEY_UNKNOWN+15,  ALLEGRO_KEY_PAD_MINUS,     ALLEGRO_KEY_PAD_DELETE,     ALLEGRO_KEY_PAD_SLASH,
   /* 0x70 */    ALLEGRO_KEY_F1,          ALLEGRO_KEY_F2,            ALLEGRO_KEY_F3,             ALLEGRO_KEY_F4,
   /* 0x74 */    ALLEGRO_KEY_F5,          ALLEGRO_KEY_F6,            ALLEGRO_KEY_F7,             ALLEGRO_KEY_F8,
   /* 0x78 */    ALLEGRO_KEY_F9,          ALLEGRO_KEY_F10,           ALLEGRO_KEY_F11,            ALLEGRO_KEY_F12,
   /* 0x7C */    ALLEGRO_KEY_UNKNOWN+17,  ALLEGRO_KEY_UNKNOWN+18,    ALLEGRO_KEY_UNKNOWN+19,     ALLEGRO_KEY_UNKNOWN+20,
   /* 0x80 */    ALLEGRO_KEY_UNKNOWN+21,  ALLEGRO_KEY_UNKNOWN+22,    ALLEGRO_KEY_UNKNOWN+23,     ALLEGRO_KEY_UNKNOWN+24,
   /* 0x84 */    ALLEGRO_KEY_UNKNOWN+25,  ALLEGRO_KEY_UNKNOWN+26,    ALLEGRO_KEY_UNKNOWN+27,     ALLEGRO_KEY_UNKNOWN+28,
   /* 0x88 */    0,                       0,                         0,                          0,
   /* 0x8C */    0,                       0,                         0,                          0,
   /* 0x90 */    ALLEGRO_KEY_NUMLOCK,     ALLEGRO_KEY_SCROLLLOCK,    0,                          0,
   /* 0x94 */    0,                       0,                         0,                          0,
   /* 0x98 */    0,                       0,                         0,                          0,
   /* 0x9C */    0,                       0,                         0,                          0,
   /* 0xA0 */    ALLEGRO_KEY_LSHIFT,      ALLEGRO_KEY_RSHIFT,        ALLEGRO_KEY_LCTRL,          ALLEGRO_KEY_RCTRL,
   /* 0xA4 */    ALLEGRO_KEY_ALT,         ALLEGRO_KEY_ALTGR,         0,                          0,
   /* 0xA8 */    0,                       0,                         0,                          0,
   /* 0xAC */    0,                       0,                         0,                          0,
   /* 0xB0 */    0,                       0,                         0,                          0,
   /* 0xB4 */    0,                       0,                         0,                          0,
   /* 0xB8 */    0,                       0,                         ALLEGRO_KEY_SEMICOLON,      ALLEGRO_KEY_EQUALS,
   /* 0xBC */    ALLEGRO_KEY_COMMA,       ALLEGRO_KEY_MINUS,         ALLEGRO_KEY_FULLSTOP,       ALLEGRO_KEY_SLASH,
   /* 0xC0 */    ALLEGRO_KEY_TILDE,       0,                         0,                          0,
   /* 0xC4 */    0,                       0,                         0,                          0,
   /* 0xC8 */    0,                       0,                         0,                          0,
   /* 0xCC */    0,                       0,                         0,                          0,
   /* 0xD0 */    0,                       0,                         0,                          0,
   /* 0xD4 */    0,                       0,                         0,                          0,
   /* 0xD8 */    0,                       0,                         0,                          ALLEGRO_KEY_OPENBRACE,
   /* 0xDC */    ALLEGRO_KEY_BACKSLASH,   ALLEGRO_KEY_CLOSEBRACE,    ALLEGRO_KEY_QUOTE,          0,
   /* 0xE0 */    0,                       0,                         ALLEGRO_KEY_BACKSLASH2,     0,
   /* 0xE4 */    0,                       ALLEGRO_KEY_UNKNOWN+29,    0,                          0,
   /* 0xE8 */    0,                       0,                         0,                          0,
   /* 0xEC */    0,                       0,                         0,                          0,
   /* 0xF0 */    0,                       0,                         0,                          0,
   /* 0xF4 */    0,                       0,                         ALLEGRO_KEY_UNKNOWN+30,     ALLEGRO_KEY_UNKNOWN+31,
   /* 0xF8 */    ALLEGRO_KEY_UNKNOWN+32,  ALLEGRO_KEY_UNKNOWN+33,    ALLEGRO_KEY_UNKNOWN+34,     ALLEGRO_KEY_UNKNOWN+35,
   /* 0xFC */    ALLEGRO_KEY_UNKNOWN+36,  ALLEGRO_KEY_UNKNOWN+37,    ALLEGRO_KEY_UNKNOWN+38,     0
};




int GetAllegroKeyFromVKey(unsigned char vkey) {
   return vk_to_al_keycode[vkey];
}
