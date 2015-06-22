

#ifndef AllegroAndWindows_HPP
#define AllegroAndWindows_HPP



// is this necessary?
#ifdef WINVER
   #undef WINVER
#endif
#ifdef _WIN32_WINNT
   #undef _WIN32_WINNT
#endif

// Target XP and higher, as RawInput is not supported on anything earlier
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

// Target Vista and higher for WM_INPUT_DEVICE_CHANGE
//#define WINVER 0x0600
//#define _WIN32_WINNT 0x0600

// Include Allegro library headers
#include "allegro5/allegro.h"
#include "allegro5/allegro_windows.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_native_dialog.h"

// Include windows headers
#include <windows.h>
#include <winuser.h>



typedef bool (*WINDOW_CALLBACK)(ALLEGRO_DISPLAY* , UINT , WPARAM , LPARAM , LRESULT* , void*);



int SetupAllegro();// returns 0 for success, and non-zero for failure

bool AllegroReady();

void DrawBitmapToHDC(HDC hdc , ALLEGRO_BITMAP* bmp);





#endif // #ifdef AllegroAndWindows_HPP




