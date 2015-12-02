


#include "MouseStrategyHeavy.hpp"

#include "Mouse.hpp"

#include "VisualLogger.hpp"

#include "MouseTracker.hpp"

#include "WindowHandler.hpp"

#include "MouseData.hpp"

#include <cmath>



/// --------------------------------------------------------   HeavyMouseStrategy   ------------------------------------------------------




void HeavyMouseStrategy::HandleInput(RAWINPUT input) {
   HandleInput(input , true);
}



void HeavyMouseStrategy::HandleInput(RAWINPUT input , bool process_input) {
   
   RAWINPUTHEADER hdr = input.header;
   if (hdr.dwType != RIM_TYPEMOUSE) {
      return;
   }
   
   Mouse* active_mouse = mouse_tracker->GetMouseFromHandle(input.header.hDevice);
   if (!active_mouse) {
      return;
   }
/*   
   if (process_input) {
      active_mouse->HandleRawInput(input);
   }
*/   
   RAWMOUSE rms = input.data.mouse;
   USHORT flags = rms.usButtonFlags;
   bool down = false;
   int button = FlagsToButtonIndex(flags , &down);

   double grabbing_mouse_held_duration = 0.0;
   switch (heavy_mouse_strategy_state) {
   case HEAVY_MOUSE_STATE_FREE :
      active_mouse->HandleRawInput(input);
      
      if ((button == 1) && down) {
         grabbing_mouse = active_mouse;
         grabbing_mouse_starting_time = al_get_time();
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_DETERMINING_CLICK);
      }
      else {
         
         if (active_mouse->MouseMoved()) {
            window_handler->HandleMouseMove(active_mouse);
         }
      }
      break;
   case HEAVY_MOUSE_STATE_DETERMINING_CLICK :
      /// Allow all mice to move
      active_mouse->HandleRawInput(input);
      
      if (active_mouse->MouseMoved()) {
         window_handler->HandleMouseMove(active_mouse);
      }
      grabbing_mouse_held_duration = al_get_time() - grabbing_mouse_starting_time;
      /// If mouse is held for certain duration, then we are initiating a drag
      if (grabbing_mouse_held_duration > mouse_drag_duration_threshold) {
         /// Don't send mouse down to window handler now - send it later when group agrees
///         window_handler->HandleButton(grabbing_mouse , 1 , true , grabbing_mouse->X() , grabbing_mouse->Y());
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_GRABBING);
         HandleInput(input , false);/// Process input for new state without reprocessing mouse input
         break;
      }
      else {
         /// If grabbing mouse's primary mouse button comes up before drag threshold reached, send click and release and reset state
         if ((button == 1) && !down && (active_mouse == grabbing_mouse)) {
            window_handler->HandleButton(grabbing_mouse , 1 , true , grabbing_mouse->X() , grabbing_mouse->Y());
            window_handler->HandleButton(grabbing_mouse , 1 , false , grabbing_mouse->X() , grabbing_mouse->Y());
            SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
            break;
         }
      }
      break;
   case HEAVY_MOUSE_STATE_GRABBING :
      active_mouse->HandleRawInput(input);
      
      /// Only allow non-grabbing mice to move (ie. freeze grabbing mouse)
      if ((active_mouse != grabbing_mouse) && (active_mouse->MouseMoved())) {
         window_handler->HandleMouseMove(active_mouse);
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_GRABBING);/// To reset mice near/far states
      }
      
      /// Drag request already initiated
      /// If grabbing mouse's button comes up, cancel grab
      if ((button == 1) && !down && active_mouse == grabbing_mouse) {
         /// Don't send button up to window handler
///         window_handler->HandleButton(grabbing_mouse , 1 , false , grabbing_mouse->X() , grabbing_mouse->Y());
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
         break;
      }
      
      /// Watch other mice for button up and down, indicating consent to drag
      if ((button == 1) && (active_mouse != grabbing_mouse) && GroupAgrees()) {
         /// If more than half of the mice are down, transistion to dragging state
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_DRAGGING);
///         HandleInput(input , false);/// Process input for new state without reprocessing mouse input
         break;
      }
      break;
   case HEAVY_MOUSE_STATE_DRAGGING :
      
      /// Group has consented to drag, object being dragged
      

      /// Only allow dragging mouse to move the object
      if (active_mouse == grabbing_mouse &&
          (((rms.usFlags == MOUSE_MOVE_RELATIVE) && (rms.lLastX || rms.lLastY)) ||
           (rms.usFlags == MOUSE_MOVE_ABSOLUTE)))
      {
         active_mouse->HandleRawInput(input);
         
         /// Move other mice in sync with the grabbing mouse
         for (int i = 0 ; i < (int)other_mice_offsets.size() ; ++i) {
            pair<Mouse* , pair<int , int> > pmxy = other_mice_offsets[i];
            Mouse* m = pmxy.first;
            pair<int , int> pxy = pmxy.second;
            m->SetPos(grabbing_mouse->X() + pxy.first , grabbing_mouse->Y() + pxy.second);
         }
         
         
         window_handler->HandleMouseMove(grabbing_mouse);
         /// Hack to prevent mouse flicker - keep the system mouse at the same position as the dragging mouse
         SetCursorPos(grabbing_mouse->X()  , grabbing_mouse->Y());
      }
      
      /// Monitor left button up in case a user no longer consents
      if ((active_mouse != grabbing_mouse) && (button == 1) && !down && !GroupAgrees()) {
         active_mouse->HandleRawInput(input);
         /// Group no longer agrees, go back to grabbing state
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_GRABBING);
      }
      
      /// If user who initiated drag lets go of mouse button, drop object
      if ((active_mouse == grabbing_mouse) && (button == 1) && !down) {
         active_mouse->HandleRawInput(input);
         window_handler->HandleButton(grabbing_mouse , 1 , false , grabbing_mouse->X() , grabbing_mouse->Y());
         SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
      }
      break;
   }
}



void HeavyMouseStrategy::Reset() {
   SetHeavyMouseStrategyState(HEAVY_MOUSE_STATE_FREE);
}



bool HeavyMouseStrategy::OtherMiceNear() {
   if (!grabbing_mouse) {return false;}
   vector<Mouse*> other_mice = mouse_tracker->OtherMiceVector(grabbing_mouse);
   int near_count = 0;
   for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
      near_count += MouseNear(other_mice[i])?1:0;
   }
   if (near_count >= (int)mouse_tracker->NMice()/2) {
      return true;
   }
   return false;
}



bool HeavyMouseStrategy::MouseNear(Mouse* m) {
   if (!grabbing_mouse) {
      return false;
   }
   double dx = m->X() - grabbing_mouse->X();
   double dy = m->Y() - grabbing_mouse->Y();
   double distance = sqrt(dx*dx + dy*dy);
   if (distance > helper_distance_threshold) {
      return false;
   }
   return true;
}



bool HeavyMouseStrategy::GroupAgrees() {
   std::vector<Mouse*> other_mice = mouse_tracker->OtherMiceVector(grabbing_mouse);
   int consent_count = 1;
   for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
      Mouse* other_mouse = other_mice[i];
      consent_count += (MouseNear(other_mouse) && other_mouse->GetButtonState(1))?1:0;
   }
   if (consent_count > (int)mouse_tracker->NMice()/2) {
      return true;
   }
   return false;
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
         m->SetTransColor(255,0,255);
         ALLEGRO_ASSERT(m->SetImage(heavy_mouse_images[HEAVY_MOUSE_ACTIVE]));
      }
      grabbing_mouse_starting_time = 0.0;
      break;
   case HEAVY_MOUSE_STATE_DETERMINING_CLICK :
      break;
   case HEAVY_MOUSE_STATE_GRABBING :
      ALLEGRO_ASSERT(grabbing_mouse);
      grabbing_mouse->SetTransColor(255,0,255);
      grabbing_mouse->SetImage(heavy_mouse_images[HEAVY_MOUSE_GRABBING]);
      for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
         Mouse* m = other_mice[i];
         m->SetTransColor(255,0,255);
         if (MouseNear(m)) {
            ALLEGRO_ASSERT(m->SetImage(heavy_mouse_images[HEAVY_MOUSE_HELPER_READY]));
         }
         else {
            ALLEGRO_ASSERT(m->SetImage(heavy_mouse_images[HEAVY_MOUSE_NEEDS_HELPER]));
         }
      }
      break;
   case HEAVY_MOUSE_STATE_DRAGGING :
      ALLEGRO_ASSERT(grabbing_mouse);
      
      grabbing_mouse->SetTransColor(255,0,255);
      grabbing_mouse->SetImage(heavy_mouse_images[HEAVY_MOUSE_DRAGGING]);
      for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
         Mouse* m = other_mice[i];
         m->SetTransColor(255,0,255);
         ALLEGRO_ASSERT(m->SetImage(heavy_mouse_images[HEAVY_MOUSE_INACTIVE]));
      }
      
      /// Send mouse down to window handler now - group agrees
      window_handler->HandleButton(grabbing_mouse , 1 , true , grabbing_mouse->X() , grabbing_mouse->Y());
      
      other_mice_offsets.clear();
      for (int i = 0 ; i < (int)other_mice.size() ; ++i) {
         pair<int , int> pxy;
         pxy.first = other_mice[i]->X() - grabbing_mouse->X();
         pxy.second = other_mice[i]->Y() - grabbing_mouse->Y();
         
         pair<Mouse* , pair < int,int> > pmxy;
         pmxy.first = other_mice[i];
         pmxy.second = pxy;
         
         other_mice_offsets.push_back(pmxy);
      }
      
      break;
   };
}
