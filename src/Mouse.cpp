

#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>


const char* heavy_mouse_paths[NUM_HEAVY_MOUSE_STATES] = {
   "NewImages/MouseInactive.png",
   "NewImages/MouseActive.png",
   "NewImages/MouseGrabbing.png",
   "NewImages/MouseNeedsHelper.png",
   "NewImages/MouseHelperReady.png",
   "NewImages/MouseDragging.png"
};



ALLEGRO_BITMAP* heavy_mouse_images[NUM_HEAVY_MOUSE_STATES] = {
   0,0,0,0,0,0
};



void FreeMiceImages() {
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = heavy_mouse_images[i];
      if (image) {
         al_destroy_bitmap(image);
         image = 0;
      }
   }
}



bool LoadMiceImages() {
   FreeMiceImages();
   bool success = true;
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(heavy_mouse_paths[i]);
      if (!image) {
         success = false;
         log.Log("Failed to load mouse image \"%s\"\n" , heavy_mouse_paths[i]);
      }
      heavy_mouse_images[i] = image;
   }
   if (!success) {
      FreeMiceImages();
   }
   return success;
}



ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active) {

   // Pass -1 for w or h to use default size


   ALLEGRO_BITMAP* cursor_bmp = 0;

   if (!active) {
///      cursor_bmp = al_load_bitmap("Images/CursorRed.png");
      cursor_bmp = al_load_bitmap("Images/DaltonRedCursor2.png");
///      cursor_bmp = al_load_bitmap("Images/TransMouseOverlayB.png");
   }
   else {
///      cursor_bmp = al_load_bitmap("Images/CursorGreen.png");
      cursor_bmp = al_load_bitmap("Images/DaltonGreenCursor2.png");
///      cursor_bmp = al_load_bitmap("Images/TransMouseOverlayA.png");
   }

   if (!cursor_bmp) {
      log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }

   if ((w == -1) || (h == -1)) {
      // Return unmodified image
      return cursor_bmp;
   }

   /// Scale image to specified width and height
   ALLEGRO_BITMAP* user_bmp = 0;

   user_bmp = al_create_bitmap(w , h);
   if (!user_bmp) {
      al_destroy_bitmap(cursor_bmp);
      return 0;
   }

   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   al_set_target_bitmap(user_bmp);

//void al_draw_scaled_bitmap(ALLEGRO_BITMAP *bitmap,
//   float sx, float sy, float sw, float sh,
//   float dx, float dy, float dw, float dh, int flags)

   al_draw_scaled_bitmap(cursor_bmp , 0.0 , 0.0 , al_get_bitmap_width(cursor_bmp) , al_get_bitmap_height(cursor_bmp),
                                      0.0 , 0.0 , w , h , 0);

   al_destroy_bitmap(cursor_bmp);

   return user_bmp;
/*
   userbmp = al_create_bitmap(w,h);

   al_set_target_bitmap(userbmp);
   al_clear_to_color(al_map_rgb(0,0,0));

   ALLEGRO_COLOR bg_color = al_map_rgb(255,255,255);
   ALLEGRO_COLOR outline_color;
   if (active) {
      outline_color = al_map_rgb(0,255,0);
   }
   else {
      outline_color = al_map_rgb(255,0,0);
   }
   al_draw_filled_rectangle(0,0,w,h , outline_color);
   al_draw_filled_rectangle(5,5,w-5,h-5 , bg_color);
///   al_draw_rectangle(0.5 , 0.5 , w -0.5 , h - 0.ALLEGRO_BITMAP* DrawMouseImage(bool activee){
    ALLEGRO_BITMAP* cursor1 = 0;
    ALLEGRO_BITMAP* circle = 0;
    cursor1 = al_load_bitmap("Images/NormalCursor1.png");
    if (!cursor1) {
          log.Log("Failed to load cursor bitmap.\n");
          return 0;
       }
    circle = al_create_bitmap(32,32);
    if(!circle){
        al_destroy_bitmap(cursor1);
        return 0;
    }
    al_clear_to_color(al_map_rgba(0,0,0,0));
    al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_ZERO);
    al_set_target_bitmap(circle);
    ALLEGRO_COLOR color = al_map_rgba(127,0,0,127);

    if(activee){
        color = al_map_rgba(0,0,127,127);
    }
    al_draw_filled_circle(16,16,8,color);
    al_set_blender(ALLEGRO_ADD,ALLEGRO_ALPHA,ALLEGRO_INVERSE_ALPHA);
    al_draw_bitmap(cursor1,0,0,0);
    al_destroy_bitmap(cursor1);
    return circle;
}
5 , outline_color , 1.0);
*/

///   return userbmp;
}

ALLEGRO_BITMAP* DrawMouseImage(bool active){

   ALLEGRO_BITMAP* cursor1 = 0;
   ALLEGRO_BITMAP* circle = 0;
   cursor1 = al_load_bitmap("Images/NormalCursor1.png");
   if (!cursor1) {
      log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }
   circle = al_create_bitmap(32,32);
   if(!circle){
      al_destroy_bitmap(cursor1);
      return 0;
   }
   al_set_target_bitmap(circle);
   al_clear_to_color(al_map_rgba(0 , 1.0*0.5 , 0 , 0.5));
   //al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
   float a = 0.5;
   ALLEGRO_COLOR black = al_map_rgba_f(0 , 0 , 0 , 1);

   al_draw_filled_rectangle(0,0,16,32,black);
   ALLEGRO_COLOR color = al_map_rgba_f(1.0, 0 , 0 , a);
   if(active){
      color = al_map_rgba_f(0 , 0 , 1.0*a , a);
   }
   al_draw_filled_circle(16,16,16,color);
   //al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
         al_draw_bitmap(cursor1,0,0,0);

   al_destroy_bitmap(cursor1);
   return circle;
}




int FlagsToButtonIndex(USHORT flags , bool* down) {
   assert(down);
   int btn = 0;
   if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
      btn = 1;
      *down = true;
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
      btn = 2;
      *down = true;
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
      btn = 3;
      *down = true;
   }
   if (flags & RI_MOUSE_LEFT_BUTTON_UP) {
      btn = 1;
      *down = false;
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_UP) {
      btn = 2;
      *down = false;
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_UP) {
      btn = 3;
      *down = false;
   }
   return btn;
}




Mouse::~Mouse() {
   CloseOurWindow();
}



void Mouse::CloseOurWindow() {
   transparent_window.CloseTheWindow();
   ready = false;
}



bool Mouse::CreateOurWindow(ALLEGRO_BITMAP* mouse_image) {
   transparent_window.UseAlphaDrawing(true);
   ready = transparent_window.CreateTheWindow(mouse_image);
   return ready;
}



void Mouse::SetHandle(HANDLE hDevice) {
   device_handle = hDevice;
}



bool Mouse::SetImage(ALLEGRO_BITMAP* mouse_image) {
   transparent_window.UseAlphaDrawing(true);
   return transparent_window.SetWindowImage(mouse_image);
}



void Mouse::SetPos(int newx , int newy) {
   if(newx < bounds.left){
      newx = bounds.left;
   }
   if(newy < bounds.top){
      newy = bounds.top;
   }
   if(newx > bounds.right){
      newx = bounds.right;
   }
   if(newy > bounds.bottom){
      newy = bounds.bottom;
   }
   ldx = newx - x;
   ldy = newy - y;
   x = newx;
   y = newy;
   transparent_window.SetWindowPosition(x - focusx , y - focusy);
}



void Mouse::MoveBy(int dx , int dy) {
   ldx = dx;
   ldy = dy;
   SetPos(x + dx , y + dy);
}



void Mouse::Draw() {
   if (!transparent_window.GetAllegroDisplay()) {
      log.Log("Mouse::Draw - transparent window's display is NULL.\n");
      return;
   }
   al_set_target_backbuffer(transparent_window.GetAllegroDisplay());
   transparent_window.PaintTheWindow();
}



void Mouse::HandleRawInput(RAWINPUT rawinput) {
   RAWINPUTHEADER hdr = rawinput.header;
   RAWMOUSE rms = rawinput.data.mouse;

   if (hdr.hDevice != device_handle) {
      log.Log("Mouse::HandleRawInput - Device handle doesn't match.\n");
      return;
   }

   if (hdr.dwType != RIM_TYPEMOUSE) {
      // we only handle mouse messages
      log.Log("Mouse::HandleRawInput - dwType doesn't match.\n");
      return;
   }

   /// RAWMOUSE::usFlags is always 0 so far, which means MOUSE_MOVE_RELATIVE
   /// Their flags could use a better configuration

   if (rms.usFlags == MOUSE_MOVE_RELATIVE) {
//      log.Log("Mouse::HandleRawInput : MOUSE_MOVE_RELATIVE\n");
      MoveBy(rms.lLastX , rms.lLastY);
   }
   else if (rms.usFlags & MOUSE_MOVE_ABSOLUTE) {
//      log.Log("Mouse::HandleRawInput : MOUSE_MOVE_ABSOLUTE\n");
      SetPos(rms.lLastX , rms.lLastY);
   }
   else {
      ldx = ldy = 0;// no movement
   }
/*
   else if (rms.lLastX || rms.lLastY) {
      MoveBy(rms.lLastX , rms.lLastY);
   }
*/
   USHORT flags = rms.usButtonFlags;

   if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
      log.Log("Mouse %p : LMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_LEFT_BUTTON_UP) {
      log.Log("Mouse %p : LMB up\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
      log.Log("Mouse %p : MMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_UP) {
      log.Log("Mouse %p : MMB up\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
      log.Log("Mouse %p : RMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_UP) {
      log.Log("Mouse %p : RMB up\n" , hdr.hDevice);
   }
}



HWND Mouse::GetMouseWindowHandle() {
   return transparent_window.GetWindowHandle();
}



void Mouse::BringMouseToFront() {
   BringWindowToTop(GetMouseWindowHandle());
}



void Mouse::ShowMouse(bool show_mouse) {
   shown = show_mouse;
   HWND window = transparent_window.GetWindowHandle();
   if (shown) {
      ShowWindow(window , SW_SHOW);
   }
   else {
      ShowWindow(window , SW_HIDE);
   }
}


