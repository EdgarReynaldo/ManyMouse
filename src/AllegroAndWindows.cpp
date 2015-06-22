

#include "AllegroAndWindows.hpp"

#include "VisualLogger.hpp"



bool allegro_ready = false;



int SetupAllegro() {
   // returns 0 for success, and non-zero for failure
   
   // setup allegro system
   if (!al_init()) {return 1;}
   
   // setup allegro addons
   if (!al_init_primitives_addon()) {return 2;}
   if (!al_init_image_addon()) {return 2;}
   if (!al_init_font_addon()) {return 2;}
   if (!al_init_ttf_addon()) {return 2;}
   
   // setup allegro input
   if (!al_install_mouse()) {return 3;}
   if (!al_install_keyboard()) {return 3;}
   
   allegro_ready = true;
   return 0;// success
}



bool AllegroReady() {
   return allegro_ready;
}



void DrawBitmapToHDC(HDC hdc , ALLEGRO_BITMAP* bmp) {
   if (!hdc) {
      log.Log("DrawBitmapToHDC() : hdc is NULL\n");
      return;
   }
   if (!bmp) {
      log.Log("DrawBitmapToHDC() : bmp is NULL\n");
      return;
   }

   ALLEGRO_LOCKED_REGION* alr = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ARGB_8888 , ALLEGRO_LOCK_READONLY);
   if (!alr) {
      log.Log("DrawBitmapToHDC() - Failed to lock bitmap %p.\n" , bmp);
      return;
   }
   
///   int maxh = al_get_bitmap_height(bmp)>h?h:al_get_bitmap_height(bmp);
///   int maxw = al_get_bitmap_width(bmp)>w?w:al_get_bitmap_width(bmp);
   int maxh = al_get_bitmap_height(bmp);
   int maxw = al_get_bitmap_width(bmp);
   
///   dib_buffer.ClearToColor(RGB(255,255,255));

   
   for (int y = 0 ; y < maxh ; ++y) {
      for (int x = 0 ; x < maxw ; ++x) {
         unsigned int* pdata = (unsigned int*)&((const char*)alr->data)[y*alr->pitch + 4*x];
///         These won't work if the endianness isn't what we are expecting!!!
//         unsigned char r = pdata[1];
//         unsigned char g = pdata[2];
//         unsigned char b = pdata[3];
         unsigned char r = (unsigned char)(((*pdata) & 0x00ff0000) >> 16);
         unsigned char g = (unsigned char)(((*pdata) & 0x0000ff00) >> 8);
         unsigned char b = (unsigned char)(((*pdata) & 0x000000ff) >> 0);
         COLORREF rgb = RGB(r , g , b);
//         if (prev != rgb) {
//            printf("Drawing RGB(%i,%i,%i) at %i,%i\n" , (int)r , (int)g , (int)b , x , y);
//         }
         if (rgb != SetPixel(hdc , x , y , rgb)) {
            log.Log("DrawBitmapToHDC() - Failed to set exact color value with SetPixel\n");
         }
      }
   }
   
   al_unlock_bitmap(bmp);
   
}





