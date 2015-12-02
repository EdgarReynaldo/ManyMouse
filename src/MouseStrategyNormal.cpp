


#include "MouseStrategyNormal.hpp"

#include "Mouse.hpp"

#include "MouseTracker.hpp"

#include "WindowHandler.hpp"

#include "MouseData.hpp"



NormalMouseStrategy::NormalMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
   MouseStrategy(mt,wh),
   mouse_state(NORMAL_MOUSE_ACTIVE)
{
   
}



void NormalMouseStrategy::HandleInput(RAWINPUT input) {

   RAWINPUTHEADER hdr = input.header;

   /// If hDevice is NULL, this is from SendInput. ( These messages are currently filtered by
   /// RawInputHandler::HandleRawInput )
   if (!hdr.hDevice) {return;}

   Mouse* mouse = mouse_tracker->GetMouseFromHandle(hdr.hDevice);

   if (mouse) {
      mouse->HandleRawInput(input);
      if (window_handler && hdr.dwType == RIM_TYPEMOUSE) {
         if (mouse->MouseMoved()) {
            window_handler->HandleMouseMove(mouse);
         }

         RAWMOUSE rms = input.data.mouse;
         USHORT flags = rms.usButtonFlags;

         bool down = false;
         int button = FlagsToButtonIndex(flags , &down);
         if (button) {
            window_handler->HandleButton(mouse , button , down , mouse->X() , mouse->Y());
         }
      }
   }
}



void NormalMouseStrategy::Reset() {
   mouse_state = NORMAL_MOUSE_ACTIVE;
   ResetImages();
}



void NormalMouseStrategy::ResetImages() {
   vector<Mouse*> mice_vec = mouse_tracker->MiceVector();
   for (int i = 0 ; i < (int)mice_vec.size() ; ++i) {
      Mouse* m = mice_vec[i];
      m->SetTransColor(0,0,0);
      ALLEGRO_ASSERT(m->SetImage(normal_mouse_images[mouse_state]));
   }
}



void NormalMouseStrategy::ToggleEnabled() {
   if (mouse_state == NORMAL_MOUSE_ACTIVE) {
      mouse_state = NORMAL_MOUSE_INACTIVE;
   }
   else {
      mouse_state = NORMAL_MOUSE_ACTIVE;
   }
   ResetImages();
}




MOUSE_STRATEGY NormalMouseStrategy::GetStrategy() {return MOUSE_STRATEGY_NORMAL;}



NORMAL_MOUSE_STATE NormalMouseStrategy::GetState() {return mouse_state;}




