

#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>


ALLEGRO_FONT* mouse_font = 0;

const char* mouse_font_path = "verdana.ttf";

const char* normal_mouse_paths[NUM_NORMAL_MOUSE_STATES] = {
   "Images/DaltonRedCursor2.png",
   "Images/DaltonGreenCursor2.png"
};



ALLEGRO_BITMAP* normal_mouse_images[NUM_NORMAL_MOUSE_STATES] = {
   0,0
};



const char* fcfs_mouse_paths[NUM_FCFS_MOUSE_STATES] = {
   "NewImages/MouseInactive2.png",
   "NewImages/MouseActive2.png",
   "NewImages/MouseGrabbing2.png",
   "NewImages/MouseDragging2.png"
};

ALLEGRO_BITMAP* fcfs_mouse_images[NUM_FCFS_MOUSE_STATES] = {
   0,0,0,0
};



const char* heavy_mouse_paths[NUM_HEAVY_MOUSE_STATES] = {
   "NewImages/MouseInactive2.png",
   "NewImages/MouseActive2.png",
   "NewImages/MouseGrabbing2.png",
   "NewImages/MouseNeedsHelper2.png",
   "NewImages/MouseHelperReady2.png",
   "NewImages/MouseDragging2.png"
};



ALLEGRO_BITMAP* heavy_mouse_images[NUM_HEAVY_MOUSE_STATES] = {
   0,0,0,0,0,0
};



void FreeMiceImages() {
   for (int i = 0 ; i < NUM_NORMAL_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = normal_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
   for (int i = 0 ; i < NUM_FCFS_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = fcfs_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = heavy_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
   if (mouse_font) {
      al_destroy_font(mouse_font);
      mouse_font = 0;
   }
}



bool LoadMiceImages() {
   FreeMiceImages();
   bool success = true;
   for (int i = 0 ; i < NUM_NORMAL_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(normal_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , normal_mouse_paths[i]);
      }
      normal_mouse_images[i] = image;
   }
   for (int i = 0 ; i < NUM_FCFS_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(fcfs_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , fcfs_mouse_paths[i]);
      }
      fcfs_mouse_images[i] = image;
   }
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(heavy_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , heavy_mouse_paths[i]);
      }
      heavy_mouse_images[i] = image;
   }
   mouse_font = al_load_ttf_font(mouse_font_path , 20 , ALLEGRO_TTF_MONOCHROME);
   if (!mouse_font) {
      ManyMouse::log.Log("Failed to load mouse font at '%s'\n" , mouse_font_path);
      success = false;
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
      ManyMouse::log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }

   if ((w == -1) || (h == -1)) {
      // Return unmodified image
      return cursor_bmp;
   }

   /// Scale image to specified width and height
   ALLEGRO_BITMAP* user_bmp = 0;

   user_bmp = CreateAllegroBitmap(w , h);
   if (!user_bmp) {
      al_destroy_bitmap(cursor_bmp);
      return 0;
   }

   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   AllegroSetTargetBitmap(user_bmp);

//void al_draw_scaled_bitmap(ALLEGRO_BITMAP *bitmap,
//   float sx, float sy, float sw, float sh,
//   float dx, float dy, float dw, float dh, int flags)

   al_draw_scaled_bitmap(cursor_bmp , 0.0 , 0.0 , al_get_bitmap_width(cursor_bmp) , al_get_bitmap_height(cursor_bmp),
                                      0.0 , 0.0 , w , h , 0);

   al_destroy_bitmap(cursor_bmp);

   return user_bmp;

}

ALLEGRO_BITMAP* DrawMouseImage(bool active){

   ALLEGRO_BITMAP* cursor1 = 0;
   ALLEGRO_BITMAP* circle = 0;
   cursor1 = al_load_bitmap("Images/NormalCursor1.png");
   if (!cursor1) {
      ManyMouse::log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }
   circle = CreateAllegroBitmap(32,32);
   if(!circle){
      DestroyAllegroBitmap(cursor1);
      return 0;
   }
   AllegroSetTargetBitmap(circle);
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

   DestroyAllegroBitmap(cursor1);
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
      ManyMouse::log.Log("Mouse::Draw - transparent window's display is NULL.\n");
      return;
   }
   al_set_target_backbuffer(transparent_window.GetAllegroDisplay());
   transparent_window.PaintTheWindow();
}



void Mouse::HandleRawInput(RAWINPUT rawinput) {
   RAWINPUTHEADER hdr = rawinput.header;
   RAWMOUSE rms = rawinput.data.mouse;

   if (hdr.hDevice != device_handle) {
      ManyMouse::log.Log("Mouse::HandleRawInput - Device handle doesn't match.\n");
      return;
   }

   if (hdr.dwType != RIM_TYPEMOUSE) {
      // we only handle mouse messages
      ManyMouse::log.Log("Mouse::HandleRawInput - dwType doesn't match.\n");
      return;
   }

   /// RAWMOUSE::usFlags is always 0 so far, which means MOUSE_MOVE_RELATIVE
   /// Their flags could use a better configuration

   if (rms.usFlags == MOUSE_MOVE_RELATIVE) {
//      ManyMouse::log.Log("Mouse::HandleRawInput : MOUSE_MOVE_RELATIVE\n");
      MoveBy(rms.lLastX , rms.lLastY);
   }
   else if (rms.usFlags & MOUSE_MOVE_ABSOLUTE) {
//      ManyMouse::log.Log("Mouse::HandleRawInput : MOUSE_MOVE_ABSOLUTE\n");
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

   bool button_down = false;
   int button_state = FlagsToButtonIndex(flags , &button_down);
   
   if (button_state) {
      buttons_down[button_state - 1] = button_down;
   }

   if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
      ManyMouse::log.Log("Mouse %p : LMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_LEFT_BUTTON_UP) {
      ManyMouse::log.Log("Mouse %p : LMB up\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
      ManyMouse::log.Log("Mouse %p : MMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_MIDDLE_BUTTON_UP) {
      ManyMouse::log.Log("Mouse %p : MMB up\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
      ManyMouse::log.Log("Mouse %p : RMB down\n" , hdr.hDevice);
   }
   if (flags & RI_MOUSE_RIGHT_BUTTON_UP) {
      ManyMouse::log.Log("Mouse %p : RMB up\n" , hdr.hDevice);
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



void Mouse::SetDeviceNumber(int dev_num) {
   if (dev_num < -1) {
         dev_num = -1;
   }
   mouse_device_number = dev_num;
}



bool Mouse::GetButtonState(int button) {
   ALLEGRO_ASSERT(button > 0 && button < NUM_MOUSE_BUTTONS + 1);
   return buttons_down[button - 1];
}





