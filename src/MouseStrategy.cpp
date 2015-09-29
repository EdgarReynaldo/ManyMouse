

#include <cmath>


#include "WindowHandler.hpp"
#include "MouseTracker.hpp"
#include "MouseStrategy.hpp"




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



void NormalMouseStrategy::Reset() {
   mouse_state = NORMAL_MOUSE_ACTIVE;
   ResetImages();
}



void NormalMouseStrategy::ResetImages() {
   vector<Mouse*> mice_vec = mouse_tracker->MiceVector();
   for (int i = 0 ; i < (int)mice_vec.size() ; ++i) {
      Mouse* m = mice_vec[i];
      m->SetImage(normal_mouse_images[mouse_state]);
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






/// --------------------------------------------------------   HeavyMouseStrategy   ------------------------------------------------------




void HeavyMouseStrategy::HandleInput(RAWINPUT input) {
   
   RAWINPUTHEADER hdr = input.header;
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

   switch (heavy_mouse_strategy_state) {
   case HEAVY_MOUSE_STATE_FREE :
      if ((button == 1) && down) {
         grabbing_mouse = active_mouse;
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_GRABBING);
      }
      else {
         window_handler->HandleMouseMove(active_mouse);
      }
      break;
   case HEAVY_MOUSE_STATE_GRABBING :
      /// Mouse up gives the click that starts dragging
      
      if ((button == 1) && !down && active_mouse == grabbing_mouse) {
         if (OtherMiceNear()) {
            window_handler->HandleButton(grabbing_mouse , button , true , grabbing_mouse->X() , grabbing_mouse->Y());
            SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_DRAGGING);
         }
         else {
            SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
         }
      }
      else {
         if (active_mouse != grabbing_mouse) {
            window_handler->HandleMouseMove(active_mouse);
         }
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_GRABBING);// To reset near/far states
      }
      break;
   case HEAVY_MOUSE_STATE_DRAGGING :
      if ((button == 1) && down && active_mouse == grabbing_mouse) {
         // User dropped the object
         window_handler->HandleButton(grabbing_mouse , button , false , grabbing_mouse->X() , grabbing_mouse->Y());
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
      }
      else if (active_mouse == grabbing_mouse) {
         // only let the mouse that is dragging move
         window_handler->HandleMouseMove(grabbing_mouse);
      }
      break;
   }
}



void HeavyMouseStrategy::Reset() {
   SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
}



bool HeavyMouseStrategy::OtherMiceNear() {
   if (!grabbing_mouse) {return false;}
   bool all_near = true;
   vector<Mouse*> other_mice = mouse_tracker->OtherMiceVector(grabbing_mouse);
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



void HeavyMouseStrategy::SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state) {
//   HEAVY_MOUSE_STRATEGY_STATE old_state = heavy_mouse_strategy_state;
   heavy_mouse_strategy_state = new_state;
   
   vector<Mouse*> all_mice = mouse_tracker->MiceVector();
   vector<Mouse*> other_mice = mouse_tracker->OtherMiceVector(grabbing_mouse);
   
   
   switch(new_state) {
   case HEAVY_MOUSE_STATE_FREE :
      for (int i = 0 ; i < (int)all_mice.size() ; ++i) {
         Mouse* m = all_mice[i];
         m->SetImage(heavy_mouse_images[HEAVY_MOUSE_ACTIVE]);
      }
      break;
   case HEAVY_MOUSE_STATE_GRABBING :
      ALLEGRO_ASSERT(grabbing_mouse);
      grabbing_mouse->SetImage(heavy_mouse_images[HEAVY_MOUSE_GRABBING]);
      for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
         Mouse* m = other_mice[i];
         double dx = m->X() - grabbing_mouse->X();
         double dy = m->Y() - grabbing_mouse->Y();
         double distance = sqrt(dx*dx + dy*dy);
         if (distance <= helper_distance_threshold) {
            m->SetImage(heavy_mouse_images[HEAVY_MOUSE_HELPER_READY]);
         }
         else {
            m->SetImage(heavy_mouse_images[HEAVY_MOUSE_NEEDS_HELPER]);
         }
      }
      break;
   case HEAVY_MOUSE_STATE_DRAGGING :
      ALLEGRO_ASSERT(grabbing_mouse);
      grabbing_mouse->SetImage(heavy_mouse_images[HEAVY_MOUSE_DRAGGING]);
      for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
         Mouse* m = other_mice[i];
         m->SetImage(heavy_mouse_images[HEAVY_MOUSE_INACTIVE]);
      }
      break;
   };
}
