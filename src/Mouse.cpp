

#include "AllegroAndWindows.hpp"

#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>

#include "MouseStates.hpp"
#include "MouseData.hpp"



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




Mouse() :
      transparent_window(),
      device_handle(0),
      mouse_device_number(-1),
      x(0),
      y(0),
      focusx(0),
      focusy(0),
      shown(true),
      ready(false),
      ldx(0),
      ldy(0),
      bounds(),
      buttons_down(),
      mouse_font(0)
{
   bounds.left = 0;
   bounds.right = 0;
   bounds.top = 0;
   bounds.bottom = 0;
   for (unsigned int i = 0 ; i < NUM_MOUSE_BUTTONS ; ++i) {
      buttons_down[i] = false;
   }
   
   mouse_font = al_load_font(mouse_font_path , mouse_font_size , mouse_font_flags);
   
}



Mouse::~Mouse() {
   CloseOurWindow();
   al_destroy_font(mouse_font);
   mouse_font = 0;
}



void Mouse::CloseOurWindow() {
   transparent_window.CloseTheWindow();
   ready = false;
}



bool Mouse::CreateOurWindow(ALLEGRO_BITMAP* mouse_image) {
   CloseOurWindow();
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





