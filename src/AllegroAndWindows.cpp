

#include "AllegroAndWindows.hpp"

#include "VisualLogger.hpp"

using namespace ManyMouse;


#include "DIBbuffer.hpp"

bool allegro_ready = false;



MMDECLSPEC int SetupAllegro(int (*atexit_ptr)(void (*)(void))) {
   // returns 0 for success, and non-zero for failure
   
   // setup allegro system
///   if (!al_init()) {return 1;}/// Can't use al_init directly inside our dll
   if (!al_install_system(ALLEGRO_VERSION_INT , atexit_ptr)) {
      return 1;
   }
///   if (!al_init()) {return 1;}
   
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



MMDECLSPEC bool AllegroReady() {
   return allegro_ready;
}



MMDECLSPEC void DrawBitmapToHDC(HDC hdc , ALLEGRO_BITMAP* bmp) {
   assert(0);
   if (!hdc) {
      ManyMouse::log.Log("DrawBitmapToHDC() : hdc is NULL\n");
      return;
   }
   if (!bmp) {
      ManyMouse::log.Log("DrawBitmapToHDC() : bmp is NULL\n");
      return;
   }

   ALLEGRO_LOCKED_REGION* alr = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ARGB_8888 , ALLEGRO_LOCK_READONLY);
   if (!alr) {
      ManyMouse::log.Log("DrawBitmapToHDC() - Failed to lock bitmap %p.\n" , bmp);
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
            ManyMouse::log.Log("DrawBitmapToHDC() - Failed to set exact color value with SetPixel\n");
         }
      }
   }
   
   al_unlock_bitmap(bmp);
   
}


MMDECLSPEC int lock_fail_count = 0;
MMDECLSPEC int lock_success_count = 0;

MMDECLSPEC void DrawBitmapToDIB(const DIBbuffer& dib_buf , ALLEGRO_BITMAP* bmp) {
   
   assert(dib_buf.ready);
   
   BITMAPINFOHEADER bmihdr = dib_buf.bm_info.bmiHeader;
   char* dib_data = (char*)dib_buf.hbm_DIBdata;
   
   assert(bmihdr.biBitCount == 32);
   
   if (!dib_data) {
      ManyMouse::log.Log("DrawBitmapToDIB() : dib_data is NULL\n");
      return;
   }
   if (!bmp) {
      ManyMouse::log.Log("DrawBitmapToDIB() : bmp is NULL\n");
      return;
   }

   ALLEGRO_LOCKED_REGION* alr = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ARGB_8888 , ALLEGRO_LOCK_READONLY);
   if (!alr) {
      lock_fail_count++;
      ManyMouse::log.Log("DrawBitmapToDIB() - Failed to lock bitmap %p. failcount = %d\n" , bmp , lock_fail_count);
///      assert(0);
      return;
   }
   
   lock_success_count++;
///   ManyMouse::log.Log("DrawBitmapToDIB() - al_lock_bitmap succeeded. success count is %d\n" , lock_success_count);
///   assert(0);
   
   int maxw = al_get_bitmap_width(bmp);
   int maxh = al_get_bitmap_height(bmp);
   int dibw = bmihdr.biWidth;
   int dibh = bmihdr.biHeight;
   if (dibw < maxw) {maxw = dibw;}
   if (abs(dibh) < maxh) {maxh = abs(dibh);}
   
///   dib_buffer.ClearToColor(RGB(255,255,255));

   
   /// 32 bit DIB format is ARGB 8888
   /// Source and destination data should both have the same stride and format now, so use memcpy directly
///   memcpy(dib_data , alr->data , 4*maxw*maxh);// pitch can be negative - this won't work
   
//*
   
   int* fakedata = new int[maxw];
   for (int i = 0 ; i < maxw ; i+=4) {
      fakedata[i] = 0xff0000ff;
      fakedata[i+1] = 0xff0000ff;
      fakedata[i+2] = 0xffffffff;
      fakedata[i+3] = 0xffffffff;
   }
   assert(maxw);
   int stride = alr->pitch;
   char* pdata = (char*)alr->data;
   for (int y = 0 ; y < maxh ; ++y) {
      
//*
      for (int x = 0 ; x < 4*maxw ; x += 4) {
         int bmpdata = *(int*)&pdata[x];
///         if (
///         ManyMouse::log.Log(" 0x%x",bmpdata);
         *(int*)&dib_data[x] = bmpdata;/// | 0x0000ff00;
///         *(int*)&dib_data[x] = bmpdata;/// | 0xff0000ff;
///         *(int*)&dib_data[x] = bmpdata | 0xffffffff;
      }
//*/
//      memcpy(dib_data , fakedata , 4*maxw);
//      memcpy(dib_data , pdata , abs(stride));
      pdata += stride;// bitmap data may have negative pitch
///      dib_data += abs(stride);// dib data is top down
      dib_data += abs(dibw*4);
   }
//*/   
   al_unlock_bitmap(bmp);
}




MMDECLSPEC ALLEGRO_BITMAP* CreateAllegroBitmap(int w , int h) {
   ALLEGRO_BITMAP* bmp = al_create_bitmap(w,h);
   ManyMouse::log.Log("CreateAllegroBitmap : Created %d X %d bitmap at %p\n" , w , h , bmp);
   ALLEGRO_ASSERT(bmp);
   return bmp;
}



MMDECLSPEC ALLEGRO_DISPLAY* CreateAllegroDisplay(int w , int h) {
   ALLEGRO_DISPLAY* display = al_create_display(w,h);
   ManyMouse::log.Log("CreateAllegroDisplay : Created %d X %d allegro display at %p\n" , w , h , display);
   ALLEGRO_ASSERT(display);
   return display;
}



MMDECLSPEC void DestroyAllegroBitmap(ALLEGRO_BITMAP*& bmp) {
   ManyMouse::log.Log("DestroyAllegroBitmap - Destroying %d x %d allegro bitmap %p\n" ,
                      al_get_bitmap_width(bmp) , al_get_bitmap_height(bmp) , bmp);
   ALLEGRO_ASSERT(bmp);
   al_destroy_bitmap(bmp);
   bmp = 0;
}



MMDECLSPEC void DestroyAllegroDisplay(ALLEGRO_DISPLAY*& display) {
   ManyMouse::log.Log("DestroyAllegroDisplay - Destroying %d x %d allegro display %p\n" , 
                      al_get_display_width(display) , al_get_display_height(display) , display);
   ALLEGRO_ASSERT(display);
   al_destroy_display(display);
   display = 0;
}



MMDECLSPEC void AllegroSetTargetBitmap(ALLEGRO_BITMAP* bmp) {
   ManyMouse::log.Log("AllegroSetTargetBitmap - setting target bitmap %p\n" , bmp);
   al_set_target_bitmap(bmp);
}








