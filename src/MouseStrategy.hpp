



#ifndef MouseStrategy_HPP
#define MouseStrategy_HPP


#include "AllegroAndWindows.hpp"

#include "VisualLogger.hpp"
using ManyMouse::log;

class MouseTracker;
class WindowHandler;



enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_COLLABORATIVE = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2
};

enum HEAVY_MOUSE_STATE {
   HEAVY_MOUSE_FREE = 0,
   HEAVY_MOUSE_GRABBING = 1,
   HEAVY_MOUSE_DRAGGING = 2
};

class MouseStrategy {

protected :
   MouseTracker* mouse_tracker;
   WindowHandler* window_handler;

public :

//   MouseStrategy();
   MouseStrategy() :
         mouse_tracker(0),
         window_handler(0)
   {
   }

   virtual void HandleInput(RAWINPUT input)=0;

   virtual MOUSE_STRATEGY GetStrategy()=0;

};


class NormalMouseStrategy : public MouseStrategy {

public :
   void HandleInput(RAWINPUT input) {}
/*
   void HandleInput(RAWINPUT input) {

      RAWINPUTHEADER hdr = input.header;

      /// If hDevice is NULL, this is from SendInput. ( These messages are currently filtered by
      /// RawInputHandler::HandleRawInput )
      if (!hdr.hDevice) {return;}

      Mouse* mouse = mouse_tracker->GetMouseFromHandle(hdr.hDevice);
      if (!mouse) {
         log.Log("Creating new mouse. hDevice = %p\n" , hdr.hDevice);
         if (!CreateMouse(hdr.hDevice)) {
            log.Log("MouseController::HandleRawInput - Could not create mouse.\n");
         }
         mouse = mouse_tracker.GetMouseFromHandle(hdr.hDevice);
      }

      if (mouse) {
         mouse->HandleRawInput(input);
         if (window_handler && hdr.dwType == RIM_TYPEMOUSE) {
            RAWMOUSE rms = input.data.mouse;

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
   }
*/

   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_NORMAL;}

};


class HeavyMouseStrategy : public MouseStrategy {

   HEAVY_MOUSE_STATE heavy_mouse_state;

public :

   void HandleInput(RAWINPUT input) {}
/*
   void HandleInput(RAWINPUT input) {
      Mouse* active_mouse = mouse_tracker->GetMouseFromHandle(input.header.hDevice);
      switch (heavy_mouse_state) {
      case HEAVY_MOUSE_FREE :

         break;
      case HEAVY_MOUSE_GRABBING :

         break;
      case HEAVY_MOUSE_DRAGGING :

         break;
      }
   }
*/;
   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};



#endif // MouseStrategy_HPP
