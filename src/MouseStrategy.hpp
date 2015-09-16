



#ifndef MouseStrategy_HPP
#define MouseStrategy_HPP


#include "AllegroAndWindows.hpp"
#include "Mouse.hpp"

#include "VisualLogger.hpp"
using ManyMouse::log;

class MouseTracker;
class WindowHandler;



enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_COLLABORATIVE = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2
};

enum HEAVY_MOUSE_STRATEGY_STATE {
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
   MouseStrategy(MouseTracker* mt , WindowHandler* wh)  :
         mouse_tracker(mt),
         window_handler(wh)
   {
   }

   virtual void HandleInput(RAWINPUT input)=0;

   virtual MOUSE_STRATEGY GetStrategy()=0;

};


class NormalMouseStrategy : public MouseStrategy {

public :
   NormalMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
      MouseStrategy(mt,wh)
   {
   }




   void HandleInput(RAWINPUT input);


   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_NORMAL;}

};


class HeavyMouseStrategy : public MouseStrategy {

   HEAVY_MOUSE_STRATEGY_STATE heavy_mouse_strategy_state;

   Mouse* grabbing_mouse;

   double helper_distance_threshold;

public :

   HeavyMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
      MouseStrategy(mt,wh),
      heavy_mouse_strategy_state(HEAVY_MOUSE_FREE),
      grabbing_mouse(0),
      helper_distance_threshold(25.0)
   {
   }

   void Reset();

   bool OtherMiceNear();
   

//   void HandleInput(RAWINPUT input) {}
//*
   void HandleInput(RAWINPUT input) {
      if (hdr.dwType != RIM_TYPEMOUSE) {
         return;
      }
      Mouse* active_mouse = mouse_tracker->GetMouseFromHandle(input.header.hDevice);
      if (active_mouse) {
         active_mouse->HandleRawInput(input);
      }
      else {
         return;
      }
      RAWMOUSE rms = input.data.mouse;
      USHORT flags = rms.usButtonFlags;
      bool down = false;
      int button = FlagsToButtonIndex(flags , &down);
/*
               if (button) {
                  window_handler->HandleButton(mouse , button , down , mouse->X() , mouse->Y());
               }

               if (mouse->MouseMoved()) {
                  window_handler->HandleMouseMove(mouse);
               }
*/


      switch (heavy_mouse_state) {
      case HEAVY_MOUSE_FREE :
         if ((button == 1) && down) {
            grabbing_mouse = active_mouse;
            SetHeavyMouseStrategyState(HEAVY_MOUSE_GRABBING);
         }
         else {
            window_handler->HandleMouseMove(active_mouse);
         }
         break;
      case HEAVY_MOUSE_GRABBING :
         if ((button == 1) && !down && active_mouse == grabbing_mouse) {
            if (OtherMiceNear()) {
               window_handler->HandleButton(grabbing_mouse , button , true , grabbing_mouse->X() , grabbing_mouse->Y());
               SetHeavyMouseStrategyState(HEAVY_MOUSE_DRAGGING);
            }
            else {
               SetHeavyMouseStrategyState(HEAVY_MOUSE_FREE);
            }
         }
         else {
            if (active_mouse != grabbing_mouse) {
               window_handler->HandleMouseMove(active_mouse);
            }
            SetHeavyMouseStrategyState(HEAVY_MOUSE_GRABBING);// To reset near/far states
         }
         break;
      case HEAVY_MOUSE_DRAGGING :
         if ((button == 1) && down && active_mouse == grabbing_mouse) {
            // User dropped the object
            window_handler->HandleButton(grabbing_mouse , button , false , grabbing_mouse->X() , grabbing_mouse->Y());
            SetHeavyMouseStrategyState(HEAVY_MOUSE_FREE);
         }
         else if (active_mouse == grabbing_mouse) {
            // only let the mouse that is dragging move
            window_handler->HandleMouseMove(grabbing_mouse);
         }
         break;
      }
   }
//*/;
   
//   void SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state);
   void SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state) {
      HEAVY_MOUSE_STRATEGY_STATE old_state = heavy_mouse_strategy_state;
      heavy_mouse_strategy_state = new_state;
      
      vector<Mouse*> all_mice = mouse_tracker->GetMouseVector();
      vector<Mouse*> other_mice = mouse_tracker->GetOtherMouseVector(grabbing_mouse);
      
      
      switch(new_state) {
      case HEAVY_MOUSE_FREE :
         for (int i = 0 ; i < (int)all_mice.size() ; ++i) {
            Mouse* m = all_mice[i];
            m->SetImage(heavy_mouse_images[MOUSE_ACTIVE]);
         }
         break;
      case HEAVY_MOUSE_GRABBING :
         ALLEGRO_ASSERT(grabbing_mouse);
         grabbing_mouse->SetImage(MOUSE_GRABBING);
         for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
            Mouse* m = other_mice[i];
            double dx = m->X() - grabbing_mouse->X();
            double dy = m->Y() - grabbing_mouse->Y();
            double distance = sqrt(dx*dx + dy*dy);
            if (distance <= helper_distance_threshold) {
               m->SetImage(heavy_mouse_images[MOUSE_HELPER_READY]);
            }
            else {
               m->SetImage(heavy_mouse_images[MOUSE_NEEDS_HELPER]);
            }
         }
         break;
      case HEAVY_MOUSE_DRAGGING :
         ALLEGRO_ASSERT(grabbing_mouse);
         grabbing_mouse->SetImage(MOUSE_DRAGGING);
         for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
            Mouse* m = other_mice[i];
            m->SetImage(heavy_mouse_images[MOUSE_INACTIVE]);
         }
         break;
      };
   }


   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};



#endif // MouseStrategy_HPP
