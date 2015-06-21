

#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>




ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active) {
   
   // Pass -1 for w or h to use default size

   ALLEGRO_BITMAP* cursor_bmp = 0;

   if (!active) {
///      cursor_bmp = al_load_bitmap("Images/CursorRed.png");
      cursor_bmp = al_load_bitmap("Images/DaltonRedCursor.png");
   }
   else {
///      cursor_bmp = al_load_bitmap("Images/CursorGreen.png");
      cursor_bmp = al_load_bitmap("Images/DaltonGreenCursor.png");
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
///   al_draw_rectangle(0.5 , 0.5 , w -0.5 , h - 0.5 , outline_color , 1.0);
*/

///   return userbmp;
}



bool MouseWindowCallback(ALLEGRO_DISPLAY* d , UINT msg , WPARAM wp , LPARAM lp , LRESULT* lr , void* mouse) {
   PAINTSTRUCT ps;
   
   (void)d;
   
   (void)wp;
   (void)lp;
   (void)lr;
   
   Mouse* m = (Mouse*)mouse;
   
   if (msg == WM_PAINT) {
      BeginPaint(m->window , &ps);
      
      m->dib_buffer.DrawBufferToWindowDC();
      
      EndPaint(m->window , &ps);
      
      return true;
   }
   return false;
}



void Mouse::QueuePaintMessage() {
   if (window) {
      RECT clrect;
      GetClientRect(window , &clrect);
      InvalidateRect(window , &clrect , true);
      UpdateWindow(window);
   }
}



void Mouse::DrawMouseToDIB() {
/*
   RECT r1,r2;
   r1.left = 0;
   r1.right = w;
   r1.top = 0;
   r1.bottom = h;
   r2.left = 10;
   r2.right = w - 10;
   r2.top = 10;
   r2.bottom = h - 10;
   HBRUSH trans_brush = CreateSolidBrush(RGB(255,255,255));
   HBRUSH fg_brush = CreateSolidBrush(RGB(0,255,0));
   
   FillRect(dib_buffer.GetBufferDC() , &r1 , fg_brush);
   FillRect(dib_buffer.GetBufferDC() , &r2 , trans_brush);
   
   dib_buffer.Flush();
   
   DeleteObject(trans_brush);
   DeleteObject(fg_brush);
*/

//*
   if (!ms_image) {
      log.Log("DrawMouseToDIB() : ms_image is NULL\n");
      return;
   }

   dib_buffer.Flush();

   ALLEGRO_LOCKED_REGION* alr = al_lock_bitmap(ms_image , ALLEGRO_PIXEL_FORMAT_ARGB_8888 , ALLEGRO_LOCK_READONLY);
   if (!alr) {
      log.Log("Failed to lock bitmap %p in DrawMouseToDIB.\n" , ms_image);
      return;
   }
   
   int maxh = al_get_bitmap_height(ms_image)>h?h:al_get_bitmap_height(ms_image);
   int maxw = al_get_bitmap_width(ms_image)>w?w:al_get_bitmap_width(ms_image);
   
   dib_buffer.ClearToColor(RGB(255,255,255));
///   COLORREF prev = RGB(255,255,255);
   
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
         if (rgb != SetPixel(dib_buffer.GetBufferDC() , x , y , rgb)) {
            log.Log("Mouse::DrawMouseToDIB - Failed to set exact color value with SetPixel\n");
         }
///         prev = rgb;
///         dib_buffer.SetXYRGB(x , y , pdata[1] , pdata[2] , pdata[3]);
      }
   }
   
   al_unlock_bitmap(ms_image);

   dib_buffer.Flush();
//*/
}



void Mouse::Free() {
   
   dib_buffer.Free();
   
   if (display) {
      al_destroy_display(display);
      display = 0;
   }
   window = 0;
   winhdc = 0;
   ms_image = 0;
   
///   x = y = -1;// leave x and y alone so users can preset it
   w = h = -1;
   focusx = 0;
   focusy = 0;
   active = false;
   ready = false;
}




bool Mouse::Create(int width , int height) {

   if ((width < 1) || (height < 1)) {return false;}

   Free();
   
   w = width;//al_get_bitmap_width(mouse_image);
   h = height;//al_get_bitmap_height(mouse_image);
   
///   al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);
   al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);// d3d
///   al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED);

/*
int al_show_native_message_box(ALLEGRO_DISPLAY *display,
char const *title, char const *heading, char const *text,
char const *buttons, int flags)
*/

///   al_set_new_window_position(x,y);

   if (!(display = al_create_display(w , h))) {
      al_show_native_message_box(NULL , "Error" , "Display" , "Mouse::Create - Could not create a display" , 0 , ALLEGRO_MESSAGEBOX_ERROR);
      Free();
      return false;
   }
   
   trans_color = RGB(0,0,0);
   
   window = al_get_win_window_handle(display);
   


   /// Using HWND_TOPMOST has no effect on the focus, except in the initial call
   SetWindowPos(window , HWND_TOPMOST , 0 , 0 , -1 , -1 , SWP_NOMOVE | SWP_NOSIZE);
   
   BringWindowToTop(window);
   
   if (0 == SetWindowLong(window , GWL_EXSTYLE , WS_EX_LAYERED)) {
      log.Log("Couldn't set WS_EX_LAYERED style attribute\n");
   }

   if (!SetLayeredWindowAttributes(window , trans_color , 255 , LWA_COLORKEY | LWA_ALPHA)) {
      log.Log("Couldn't set color key!\n");
   }


   if (!dib_buffer.Create(window)) {
      printf("Failed to create DIB buffer for window %p\n" , (void*)window);
   }
   else {
      printf("Created DIB buffer for mouse window %p\n" , (void*)window);
   }
   
   dib_buffer.ClearToColor(trans_color);
   
///   DrawMouseToDIB();
   
///   dib_buffer.DrawBufferToWindowDC();
   
   if (!al_win_add_window_callback(display , MouseWindowCallback , this)) {
      printf("Couldn't add window callback for mouse %p" , this);
   }
   
   
   
   /// Leave dc alone for now
   /// winhdc = GetDC(window);
   
   SetPos(x,y);
   
   /// This sets up windows to send a WM_PAINT message to our mouse window
   QueuePaintMessage();

   log.Log("Successfully created mouse at %i,%i\n" , x , y);
   
   return true;
   
}



void Mouse::SetHandle(HANDLE hDevice) {
   device_handle = hDevice;
}



bool Mouse::SetImage(ALLEGRO_BITMAP* mouse_image) {
   if (mouse_image && ms_image != mouse_image) {
      int iw = al_get_bitmap_width(mouse_image);
      int ih = al_get_bitmap_height(mouse_image);
      if ((iw != w) || (ih != h)) {
         if (!Create(iw,ih)) {
            printf("Failed to create %d x %d mouse.\n" , iw , ih);
            return false;
         }
      }
      ms_image = mouse_image;

      DrawMouseToDIB();
      
      QueuePaintMessage();
      return true;
   }
   
   return false;
}



void Mouse::SetPos(int newx , int newy) {
   ldx = newx - x;
   ldy = newy - y;
   x = newx;
   y = newy;
   if (display) {
      al_set_window_position(display , x - focusx , y - focusy);
   }
}



void Mouse::MoveBy(int dx , int dy) {
   ldx = dx;
   ldy = dy;
   SetPos(x + dx , y + dy);
}
   

   
void Mouse::Draw() {
   
   if (!dib_buffer.Ready()) {
      log.Log("Mouse::Draw - DIB buffer not ready.\n");
      return;
   }
///   dib_buffer.DrawBufferToWindowDC();
   QueuePaintMessage();
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




/// ------------------ MouseTracker class -----------------------------------------





MouseTracker::MouseTracker() :
      mtinfo()
{
   
}



unsigned int MouseTracker::TrackNewMouse(Mouse* mouse , HANDLE hdev) {
   unsigned int new_id = 0;
   bool found_spot = false;
   for (new_id = 0 ; new_id < mtinfo.size() ; ++new_id) {
      bool used = mtinfo[new_id].used;
      if (!used) {
         found_spot = true;
         break;
      }
   }
   if (!found_spot) {
      mtinfo.resize(mtinfo.size() + 1);
   }
   mtinfo[new_id].used = true;
   mtinfo[new_id].id = new_id;
   mtinfo[new_id].hdev = hdev;
   mtinfo[new_id].mouse = mouse;
   return new_id;
}



void MouseTracker::StopTrackingMouse(HANDLE hdev) {
   for (unsigned int new_id = 0 ; new_id < mtinfo.size() ; ++new_id) {
      HANDLE hdev2 = mtinfo[new_id].hdev;
      if (hdev == hdev2) {
         Mouse* m = mtinfo[new_id].mouse;
         if (m) {
            delete m;
            mtinfo[new_id] = MouseTrackingInfo();// use default constructor to clear values
         }
      }
   }
}



MouseTracker::~MouseTracker() {
   CleanUp();
}



void MouseTracker::CleanUp() {
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         delete m;
      }
      mtinfo[i] = MouseTrackingInfo();// clear to default constructor
   }
   mtinfo.clear();
}



vector<unsigned int> MouseTracker::GetIdsInUse() {
   vector<unsigned int> ids_in_use;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      bool in_use = mtinfo[i].used;
      if (in_use) {
         ids_in_use.push_back(i);
      }
   }
   return ids_in_use;
}



Mouse* MouseTracker::GetMouseFromHandle(HANDLE hdev) {
   // linear search, so what there will probably never be more than 10 devices attached anyway
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      if (mtinfo[i].hdev == hdev) {
         return mtinfo[i].mouse;
      }
   }
   return (Mouse*)0;
}



unsigned int MouseTracker::NMice() {
   unsigned int nmice = 0;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         ++nmice;
      }
   }
   return nmice;
}



Mouse* MouseTracker::GetMouseByIndex(unsigned int index) {
   unsigned int idx = 0;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         if (idx == index) {
            return m;
         }
         ++idx;
      }
   }
   return (Mouse*)0;
}



vector<Mouse*> MouseTracker::GetMouseVector() {
   vector<Mouse*> micevec;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         micevec.push_back(m);
      }
   }
   return micevec;
}



/// ------------------ MouseController class --------------------------------------



void MouseController::DestroyMouse(HANDLE hDevice) {
   
   mouse_tracker.StopTrackingMouse(hDevice);
   
}



bool MouseController::CreateMouse(HANDLE hDevice) {
   
   if (mouse_tracker.GetMouseFromHandle(hDevice)) {
      mouse_tracker.StopTrackingMouse(hDevice);
   }

   Mouse* newmouse = new Mouse();
   
   POINT p;
   GetCursorPos(&p);
   
   newmouse->SetPos(p.x , p.y);

   ALLEGRO_BITMAP* ms_image = enabled?ms_enabled_image:ms_disabled_image;

   if (!newmouse->SetImage(ms_image)) {
      printf("MouseController::CreateMouse : Failed to set image %p.\n" , ms_image);
      return false;
   }
   newmouse->SetHandle(hDevice);
   
   mouse_tracker.TrackNewMouse(newmouse , hDevice);
   
   return true;
}



void MouseController::DestroyMice() {

   mouse_tracker.CleanUp();

}



int MouseController::FlagsToButtonIndex(USHORT flags , bool* down) {
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



MouseController::MouseController() :
      mouse_tracker(),
      ms_enabled_image(0),
      ms_disabled_image(0),
      enabled(true),
      window_handler(0)
{
   
}



MouseController::~MouseController() {
   DestroyMice();
   FreeMouseImages();
}



void MouseController::FreeMouseImages() {
   if (ms_enabled_image) {
      al_destroy_bitmap(ms_enabled_image);
      ms_enabled_image = 0;
   }
   if (ms_disabled_image) {
      al_destroy_bitmap(ms_disabled_image);
      ms_disabled_image = 0;
   }
}



bool MouseController::CreateMouseImages() {
   FreeMouseImages();
   ms_enabled_image = CreateMouseImage(-1,-1,true);
   ms_disabled_image = CreateMouseImage(-1,-1,false);
   return ms_enabled_image && ms_disabled_image;
}



void MouseController::HandleRawInput(RAWINPUT rawinput) {
   
   RAWINPUTHEADER hdr = rawinput.header;
   
   /// If hDevice is NULL, this is from SendInput. ( These messages are currently filtered by
   /// RawInputHandler::HandleRawInput )
   if (!hdr.hDevice) {return;}
   
   Mouse* mouse = mouse_tracker.GetMouseFromHandle(hdr.hDevice);
   if (!mouse) {
      log.Log("Creating new mouse. hDevice = %p\n" , hdr.hDevice);
      if (!CreateMouse(hdr.hDevice)) {
         log.Log("MouseController::HandleRawInput - Could not create mouse.\n");
      }
      mouse = mouse_tracker.GetMouseFromHandle(hdr.hDevice);
   }

   if (mouse) {
      mouse->HandleRawInput(rawinput);
      if (window_handler && hdr.dwType == RIM_TYPEMOUSE) {
         RAWMOUSE rms = rawinput.data.mouse;
         
         USHORT flags = rms.usButtonFlags;
         bool down = false;
         int button = FlagsToButtonIndex(flags , &down);
         if (button) {
            window_handler->HandleButton(button , down , mouse->X() , mouse->Y());
         }
         
         if (mouse->MouseMoved()) {
            window_handler->HandleMouseMove(mouse);
         }
      }
   }
}



void MouseController::Draw() {

   vector<Mouse*> micevec = mouse_tracker.GetMouseVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      micevec[i]->Draw();
   }

}



void MouseController::ToggleMiceEnabled() {
   enabled = !enabled;
   
   ALLEGRO_BITMAP* ms_image = 0;
   
   if (enabled) {
      ms_image = ms_enabled_image;
   }
   else {
      ms_image = ms_disabled_image;
   }


   vector<Mouse*> micevec = mouse_tracker.GetMouseVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      if (!micevec[i]->SetImage(ms_image)) {
         log.Log("Failed to set mouse image for mouse %p and image %p\n" , micevec[i] , ms_image);
      }
   }

}



void MouseController::GetMiceWindows(vector<HWND>* winvec) {

   vector<Mouse*> micevec = mouse_tracker.GetMouseVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      winvec->push_back(m->window);
   }

}



vector<Mouse*> MouseController::GetMice() {
   return mouse_tracker.GetMouseVector();
}



void MouseController::SetWindowHandler(WindowHandler* handler) {
   window_handler = handler;
}



