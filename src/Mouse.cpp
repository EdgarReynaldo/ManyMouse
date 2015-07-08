

#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>




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

   RECT r;
   if (GetWindowRect(GetDesktopWindow() , &r)) {
      newmouse->SetBounds(r);
   }
   else {
      log.Log("Failed to SetBounds for new mouse. GetLastError says %d\n" , GetLastError());
   }

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
//   ms_enabled_image = DrawMouseImage(true);
//   ms_disabled_image = DrawMouseImage(false);
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
            window_handler->HandleButton(mouse , button , down , mouse->X() , mouse->Y());
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
      winvec->push_back(m->GetMouseWindowHandle());
   }

}



vector<Mouse*> MouseController::GetMice() {
   return mouse_tracker.GetMouseVector();
}



void MouseController::SetWindowHandler(WindowHandler* handler) {
   window_handler = handler;
}



void MouseController::BringMiceToFront() {
   vector<Mouse*> micevec = mouse_tracker.GetMouseVector();
   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      m->BringMouseToFront();
   }
}



bool MouseController::IsMouseWindow(HWND hwnd) {
   vector<Mouse*> micevec = mouse_tracker.GetMouseVector();
   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      if (m->GetMouseWindowHandle() == hwnd) {return true;}
   }
   return false;
}


