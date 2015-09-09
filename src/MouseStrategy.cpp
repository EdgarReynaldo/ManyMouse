

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
