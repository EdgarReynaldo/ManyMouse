


#include "MouseController.hpp"
#include "WindowHandler.hpp"
#include "VisualLogger.hpp"
#include "MouseTracker.hpp"



#include <cstdio>

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

   ALLEGRO_BITMAP* ms_image = mouse_image_enabled?ms_enabled_image:ms_disabled_image;

   ++mouse_device_count;
   printf("Adding mouse device #%d\n" , mouse_device_count);
   
   newmouse->SetDeviceNumber(mouse_device_count);
   
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





MouseController::MouseController(WindowHandler* wh) :
      mouse_tracker(),
      normal_strategy(&mouse_tracker , wh),
      fcfs_strategy(&mouse_tracker , wh),
      heavy_strategy(&mouse_tracker , wh),
//      collaborative_strategy(),
      active_strategy(&normal_strategy),
      ms_enabled_image(0),
      ms_disabled_image(0),
      mouse_image_enabled(true),
      mice_active(true),
//      mice_enabled(true),
      mice_shown(true),
      window_handler(0),
      mouse_device_count(0)
{
   if (!LoadMiceImages()) {
      log.Log("Failed to load mice images.\n");
      ALLEGRO_ASSERT(0);// failed to load mice images
   }
   normal_strategy.Reset();
   fcfs_strategy.Reset();
   heavy_strategy.Reset();
}



MouseController::~MouseController() {
   DestroyMice();
   FreeMiceImages();
   MouseController::FreeMouseImages();
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

   if (mice_active && active_strategy) {
      active_strategy->HandleInput(rawinput);
   }



/*
   if (mouse) {
      mouse->HandleRawInput(rawinput);
      if (window_handler && hdr.dwType == RIM_TYPEMOUSE) {
         RAWMOUSE rms = rawinput.data.mouse;

         USHORT flags = rms.usButtonFlags;
         if (mice_active) {
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
*/
}



void MouseController::Draw() {

   vector<Mouse*> micevec = mouse_tracker.MiceVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      micevec[i]->Draw();
   }

}



void MouseController::ToggleMouseImage() {
   
   if (active_strategy->GetStrategy() != MOUSE_STRATEGY_NORMAL) {
      return;
   }
   mouse_image_enabled = !mouse_image_enabled;

   ALLEGRO_BITMAP* ms_image = 0;

   normal_strategy.ToggleEnabled();

   ms_image = normal_mouse_images[normal_strategy.GetState()];

   vector<Mouse*> micevec = mouse_tracker.MiceVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      if (!m->SetImage(ms_image)) {
         log.Log("Failed to set mouse image for mouse %p and image %p\n" , micevec[i] , ms_image);
      }
   }

}



void MouseController::ActivateMice(bool activate_mice) {
   mice_active = activate_mice;
   ShowMice(mice_active);
}



void MouseController::ShowMice(bool show_mice) {
   mice_shown = show_mice;

   vector<Mouse*> mice = mouse_tracker.MiceVector();
   for (unsigned int i = 0 ; i < mice.size() ; ++i) {
      Mouse* m = mice[i];
      m->ShowMouse(mice_shown);
   }
}



void MouseController::GetMiceWindows(vector<HWND>* winvec) {

   vector<Mouse*> micevec = mouse_tracker.MiceVector();

   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      winvec->push_back(m->GetMouseWindowHandle());
   }

}



vector<Mouse*> MouseController::GetMice() {
   return mouse_tracker.MiceVector();
}



void MouseController::SetWindowHandler(WindowHandler* handler) {
   window_handler = handler;
}



void MouseController::BringMiceToFront() {
   vector<Mouse*> micevec = mouse_tracker.MiceVector();
   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      m->BringMouseToFront();
   }
}



bool MouseController::IsMouseWindow(HWND hwnd) {
   vector<Mouse*> micevec = mouse_tracker.MiceVector();
   for (unsigned int i = 0 ; i < micevec.size() ; ++i) {
      Mouse* m = micevec[i];
      if (m->GetMouseWindowHandle() == hwnd) {return true;}
   }
   return false;
}



void MouseController::SetMouseStrategy(MOUSE_STRATEGY strategy) {
   MouseStrategy* strategies[NUM_MOUSE_STRATEGIES] = {
      &normal_strategy,
      &fcfs_strategy,
      &heavy_strategy,
      0
   };
   active_strategy = strategies[strategy];
   ALLEGRO_ASSERT(active_strategy);
   active_strategy->Reset();
}



