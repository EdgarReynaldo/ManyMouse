




#include "AllegroAndWindows.hpp"
#include "VisualLogger.hpp"

#include "DIBbuffer.hpp"


#include <cstdlib>
#include <cstdio>


#include <GL/gl.h>



void PrintPixel(HDC hdc , int x , int y);




int main(int argc, char** argv) {
   
   
   if (!al_init()) {return 1;}
   
   if (!al_install_keyboard()) {return 2;}
   if (!al_install_mouse()) {return 2;}
   
   if (!al_init_font_addon()) {return 3;}
   if (!al_init_ttf_addon()) {return 3;}
   if (!al_init_primitives_addon()) {return 3;}
   
   ALLEGRO_DISPLAY* display = 0;
   ALLEGRO_EVENT_QUEUE* queue = 0;
   ALLEGRO_TIMER* timer = 0;
   ALLEGRO_FONT* verdana12 = 0;
   
   int sw = 640;
   int sh = 400;
//   int sw = 1280;
//   int sh = 800;
   
   VisualLogger log("TransWindowLog.txt");
   
   HWND handle = 0;



//   al_set_new_display_flags(ALLEGRO_OPENGL);
   al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
//   al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);
//   al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
   
   al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);
   
   display = al_create_display(sw,sh);
   
   if (!display) {return 4;}
   
   al_clear_to_color(al_map_rgb(253,254,255));
   al_flip_display();
   
   
   queue = al_create_event_queue();
   timer = al_create_timer(1.0/60.0);
   
   if (!queue || !timer) {return 5;}
   
   al_register_event_source(queue , al_get_display_event_source(display));
   al_register_event_source(queue , al_get_keyboard_event_source());
   al_register_event_source(queue , al_get_mouse_event_source());
   al_register_event_source(queue , al_get_timer_event_source(timer));
   
   verdana12 = al_load_font("verdana.ttf" , -12 , 0);
   
   if (!verdana12) {return 6;}
   
   
   // Now that allegro is created, init the log's mutex
   if (!log.InitMutex()) {return 10;}
   
   
   handle = al_get_win_window_handle(display);

   if (0 == SetWindowLong(handle , GWL_EXSTYLE , WS_EX_LAYERED)) {
      log.Log("Couldn't set WS_EX_LAYERED style attribute\n");
   }
   
   COLORREF trans_key = RGB(0,0,0);
   COLORREF white = RGB(255,255,255);
   
   if (!SetLayeredWindowAttributes(handle , trans_key , 255 , LWA_COLORKEY)) {
      log.Log("Couldn't set color key!\n");
   }
   
   HDC hdc = GetDC(handle);

   PrintPixel(hdc , 0 , 0);
   PrintPixel(hdc , 0 , sh - 1);
   PrintPixel(hdc , sw - 1 , 0);
   PrintPixel(hdc , sw - 1 , sh - 1);
   
   ReleaseDC(handle , hdc);

   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);

   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glFlush();
   al_flip_display();

///   al_clear_to_color(al_map_rgb(255,255,255));
///   al_flip_display();
   
   


   hdc = GetDC(handle);

   PrintPixel(hdc , 0 , 0);
   PrintPixel(hdc , 0 , sh - 1);
   PrintPixel(hdc , sw - 1 , 0);
   PrintPixel(hdc , sw - 1 , sh - 1);

   al_rest(5.0);


/*
   HBRUSH hbrush_trans = CreateSolidBrush(trans_key);
   if (!hbrush_trans) {
      log.Log("Failed to create HBRUSH.\n");
      ReleaseDC(handle , hdc);
      return 12;
   }
*/
/*   
LONG WINAPI SetWindowLong(
  _In_ HWND hWnd,
  _In_ int  nIndex,
  _In_ LONG dwNewLong
);
BOOL WINAPI SetLayeredWindowAttributes(
  _In_ HWND     hwnd,
  _In_ COLORREF crKey,
  _In_ BYTE     bAlpha,
  _In_ DWORD    dwFlags
);
*/

///   SetWindowPos(handle , HWND_TOPMOST , 0 , 0 , -1 , -1 , SWP_NOMOVE | SWP_NOSIZE);

///   al_win_add_window_callback(display , WindowProcCallback , this);
   
   log.Log("Testing visual logger for feedback1\n");
   log.Log("Testing visual logger for feedback2\n");
   log.Log("Testing visual logger for feedback3\n");
   
   DIBbuffer dibbuf;
   
   if (!dibbuf.Create(handle)) {
      log.Log("Failed to create DIB buffer.\n");
   }
   else {
      log.Log("Created DIB buffer.\n");
   }
   
   dibbuf.ClearToColor(RGB(255,255,255));
   dibbuf.FillAreaRect(0.25 , 0.25 , 0.5 , 0.5 , RGB(0,0,0));
   
   dibbuf.DrawBufferToWindowDC();
   
   
   bool redraw = true;
   bool quit = false;

   al_start_timer(timer);
   
   int alpha = 0;
   int dir = 1;
   
/*   
typedef struct _RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT;
*/
/*   
   for (int i = 0 ; i < sw*sh/10 ; ++i) {
      int x = (rand()/(float)RAND_MAX)*sw;
      int y = (rand()/(float)RAND_MAX)*sh;
      SetPixel(hdc, x , y , trans_key);
   }
*/   
//   DeleteObject(hbrush_trans);
//   hbrush_trans = 0;
   
   while (!quit) {
      
      if (redraw) {
            
/*         
         al_clear_to_color(al_map_rgb(255,255,255));
         
         
         RECT r1;
         r1.left = sw/3;
         r1.right = (2*sw)/3;
         r1.top = 0;
         r1.bottom = sh;
         
         RECT r2;
         r2.left = 0;
         r2.right = sw;
         r2.top = sh/3;
         r2.bottom = (2*sh)/3;

         al_draw_filled_rectangle(r1.left , r1.top , r1.right , r1.bottom , al_map_rgba(0,0,0,0));
         al_draw_filled_rectangle(r2.left , r2.top , r2.right , r2.bottom , al_map_rgba(0,0,0,255));

         al_flip_display();

         RECT r3;
         r3.left = sw/3;
         r3.right = (2*sw)/3;
         r3.top = sh/3;
         r3.bottom = (2*sh)/3;

         
         FillRect(hdc , &r , hbrush_trans);
         FillRect(hdc , &r2 , hbrush_trans);
         
         FillRect(hdc , &r3 , hbrush_trans);
*/         

         if (!SetLayeredWindowAttributes(handle , trans_key , alpha , LWA_COLORKEY | LWA_ALPHA)) {
            log.Log("Couldn't set color key!\n");
         }
//         al_clear_to_color(al_map_rgba(0,0,0,0));
//         log.DrawLog(verdana12 , al_map_rgb(255,255,255) , 10 , sh);
//         al_flip_display();
/*
         if (!SetLayeredWindowAttributes(handle , black_key , 255 , LWA_COLORKEY)) {
            log.Log("Couldn't set color key!\n");
         }
*/
      }
      
      do {
         ALLEGRO_EVENT ev;
         al_wait_for_event(queue , &ev);
         
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            quit = true;
            break;
         }  
         if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            quit = true;
            break;
         }
         
         if (ev.type == ALLEGRO_EVENT_TIMER) {
            
            
            if (dir > 0) {
               alpha += dir;
               if (alpha > 255) {
                  alpha = 255;
                  dir *= -1;
               }
            }
            else if (dir < 0) {
               alpha += dir;
               if (alpha < 0) {
                  alpha = 0;
                  dir *= -1;
               }
            }
            
            redraw = true;
         }
         
      } while (!al_is_event_queue_empty(queue));
      
      
   }
   
   ReleaseDC(handle , hdc);
   
   return 0;
};



void PrintPixel(HDC hdc , int x , int y) {
   printf("WTF\n");
   COLORREF cref = GetPixel(hdc , 0 , 0);
   if (cref == CLR_INVALID) {
      log.Log("GetPixel(hdc,%i,%i) returned CLR_INVALID\n" , x , y);
      return;
   }
   log.Log("Pixel at %i,%i has an RGB of (%i,%i,%i)\n" , x , y ,
           (int)GetRValue(cref) , (int)GetGValue(cref) , (int)GetBValue(cref));
}




