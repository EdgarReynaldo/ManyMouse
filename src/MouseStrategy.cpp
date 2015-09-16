

#include "MouseStrategy.hpp"

#include "WindowHandler.hpp"




void NormalMouseStrategy::HandleInput(RAWINPUT input) {

   RAWINPUTHEADER hdr = input.header;

   /// If hDevice is NULL, this is from SendInput. ( These messages are currently filtered by
   /// RawInputHandler::HandleRawInput )
   if (!hdr.hDevice) {return;}

   Mouse* mouse = mouse_tracker->GetMouseFromHandle(hdr.hDevice);

   if (mouse) {
      mouse->HandleRawInput(input);
      if (window_handler && hdr.dwType == RIM_TYPEMOUSE) {
         RAWMOUSE rms = input.data.mouse;

         USHORT flags = rms.usButtonFlags;
///            if (mice_active) {
            bool down = false;
            int button = FlagsToButtonIndex(flags , &down);
            if (button) {
               window_handler->HandleButton(mouse , button , down , mouse->X() , mouse->Y());
            }

            if (mouse->MouseMoved()) {
               window_handler->HandleMouseMove(mouse);
            }
///            }
      }
   }
}



/// --------------------------------------------------------   HeavyMouseStrategy   ------------------------------------------------------




void HeavyMouseStrategy::Reset() {
   SetHeavyMouseStrategyState(HEAVY_MOUSE_FREE);
}



bool HeavyMouseStrategy::OtherMiceNear() {
   if (!grabbing_mouse) {return false;}
   bool all_near = true;
   vector<Mouse*> other_mice = mouse_tracker->GetOtherMouseVector();
   for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
      Mouse* m = other_mice[i];
      double dx = m->X() - grabbing_mouse->X();
      double dy = m->Y() - grabbing_mouse->Y();
      double distance = sqrt(dx*dx + dy*dy);
      if (distance > helper_distance_threshold) {
         all_near = false;
      }
   }
   return all_near;
}
