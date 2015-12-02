


#include "MouseStrategyFCFS.hpp"


#include "AllegroAndWindows.hpp"
#include "VisualLogger.hpp"

#include "Mouse.hpp"
#include "MouseTracker.hpp"
#include "MouseData.hpp"

#include "WindowHandler.hpp"



/// ----------------------------------------------------------   FCFSMouseStrategy    -----------------------------------------------------


FCFSMouseStrategy::FCFSMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
      MouseStrategy(mt,wh),
      fcfs_mouse_buffer(0),
      grabbing_mouse(0),
      mouse_drag_duration_threshold(0.5),
      grabbing_mouse_starting_time(0.0)
{
   
}



FCFSMouseStrategy::~FCFSMouseStrategy() {
   FreeBuffer();
}



void FCFSMouseStrategy::FreeBuffer() {
   if (fcfs_mouse_buffer) {
      al_destroy_bitmap(fcfs_mouse_buffer);
      fcfs_mouse_buffer = 0;
   }
}



void FCFSMouseStrategy::HandleInput(RAWINPUT input) {
   HandleInput(input , true);
}



void FCFSMouseStrategy::HandleInput(RAWINPUT input , bool process_input) {
   
   RAWINPUTHEADER hdr = input.header;
   if (hdr.dwType != RIM_TYPEMOUSE) {
      return;
   }
   
   Mouse* active_mouse = mouse_tracker->GetMouseFromHandle(input.header.hDevice);
   if (!active_mouse) {
      return;
   }
   
   /// Allow all mice to move and receive state changes
   if (process_input) {
      active_mouse->HandleRawInput(input);
   }
   
   RAWMOUSE rms = input.data.mouse;
   USHORT flags = rms.usButtonFlags;
   bool down = false;
   int button = FlagsToButtonIndex(flags , &down);

   double grabbing_mouse_held_duration = 0.0;

   switch(fcfs_state) {
   case FCFS_STATE_FREE :
      /// First come first serve
      if ((button == 1) && down) {
         grabbing_mouse = active_mouse;
         SetFCFSStrategyState(FCFS_STATE_PROCESSING_CLICK);
      }
      break;
   case FCFS_STATE_PROCESSING_CLICK :
      /// Only send active mouse's movement to OS and window
      if (active_mouse->MouseMoved()) {
         window_handler->HandleMouseMove(active_mouse);
      }
      grabbing_mouse_held_duration = al_get_time() - grabbing_mouse_starting_time;
      /// If mouse is held for certain duration, then we are initiating a drag
      if (grabbing_mouse_held_duration > mouse_drag_duration_threshold) {
         window_handler->HandleButton(grabbing_mouse , 1 , true , grabbing_mouse->X() , grabbing_mouse->Y());
         SetFCFSStrategyState(FCFS_STATE_DRAG);
         HandleInput(input , false);/// Process input for new state without reprocessing mouse input
         break;
      }
      else {
         /// If grabbing mouse's primary mouse button comes up before drag threshold reached, send click and release and reset state
         if ((button == 1) && !down && (active_mouse == grabbing_mouse)) {
            window_handler->HandleButton(grabbing_mouse , 1 , true , grabbing_mouse->X() , grabbing_mouse->Y());
            window_handler->HandleButton(grabbing_mouse , 1 , false , grabbing_mouse->X() , grabbing_mouse->Y());
            SetFCFSStrategyState(FCFS_STATE_FREE);
            break;
         }
      }
      break;
   case FCFS_STATE_DRAG :
      if ((active_mouse == grabbing_mouse) && !down && (button == 1)) {
         /// Dragging mouse released LMB
         window_handler->HandleButton(grabbing_mouse , 1 , false , grabbing_mouse->X() , grabbing_mouse->Y());
         SetFCFSStrategyState(FCFS_STATE_FREE);
      }
      break;
   default :
      ALLEGRO_ASSERT(0);
      break;
   };
}



void FCFSMouseStrategy::Reset() {
   ResetImages();
   
}

void FCFSMouseStrategy::ResetImages() {
   FreeBuffer();
   ALLEGRO_ASSERT(fcfs_mouse_images[0]);
   
   int w = al_get_bitmap_width(fcfs_mouse_images[0]);
   int h = al_get_bitmap_height(fcfs_mouse_images[0]);
   fcfs_mouse_buffer = CreateAllegroBitmap(w,h);

   ALLEGRO_ASSERT(fcfs_mouse_buffer);

   if (!fcfs_mouse_buffer) {
      return;
   }

   SetFCFSStrategyState(FCFS_STATE_FREE);
}

void FCFSMouseStrategy::DrawPointers(FCFS_STRATEGY_STATE strategy_state) {
   
   ALLEGRO_ASSERT(fcfs_mouse_buffer);
   

   ALLEGRO_BITMAP* image = 0;
   
   vector<Mouse*> mouse_vec = mouse_tracker->MiceVector();
   for (unsigned i = 0 ; i < mouse_vec.size() ; ++i) {
      Mouse* m = mouse_vec[i];
      al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
      
      al_set_target_backbuffer(NULL);
      
//      al_hold_bitmap_drawing(false);
      al_set_target_bitmap(fcfs_mouse_buffer);
      al_clear_to_color(al_map_rgb(0,0,0));
      switch(strategy_state) {
      case FCFS_STATE_FREE :
         image = fcfs_mouse_images[FCFS_MOUSE_ACTIVE];
         break;
      case FCFS_STATE_PROCESSING_CLICK :
         if (m == grabbing_mouse) {
            image = fcfs_mouse_images[FCFS_MOUSE_GRABBING];
         }
         else {
            image = fcfs_mouse_images[FCFS_MOUSE_INACTIVE];
         }
         break;
      case FCFS_STATE_DRAG :
         if (m == grabbing_mouse) {
            image = fcfs_mouse_images[FCFS_MOUSE_DRAGGING];
         }
         else {
            image = fcfs_mouse_images[FCFS_MOUSE_INACTIVE];
         }
         break;
      }
      ALLEGRO_ASSERT(image);
         
      al_draw_bitmap(image , 0 , 0 , 0);
//void al_draw_textf(const ALLEGRO_FONT *font, ALLEGRO_COLOR color,
//   float x, float y, int flags,
//   const char *format, ...)

      al_set_blender(ALLEGRO_ADD , ALLEGRO_ALPHA , ALLEGRO_INVERSE_ALPHA);

      al_draw_textf(m->GetMouseFont() , al_map_rgb(255,127,0) ,
            al_get_bitmap_width(fcfs_mouse_buffer) - al_get_text_width(m->GetMouseFont() , "0") ,
            al_get_bitmap_height(fcfs_mouse_buffer) - al_get_font_line_height(m->GetMouseFont()),
            0,
            "%d",m->MouseDeviceNumber());
      
      printf("FCFS DrawPointers : Setting image for mouse device #%d\n" , m->MouseDeviceNumber());
      
      m->SetTransColor(255,0,255);
      
      ALLEGRO_ASSERT(m->SetImage(fcfs_mouse_buffer));
   }
}



void FCFSMouseStrategy::ToggleEnabled() {
   
}



void FCFSMouseStrategy::SetFCFSStrategyState(FCFS_STRATEGY_STATE new_state) {
   fcfs_state = new_state; 
   DrawPointers(fcfs_state);

/*
   switch(fcfs_state) {
   case FCFS_STATE_FREE :
      
      break;
   case FCFS_STATE_PROCESSING_CLICK :
      
      break;
   case FCFS_STATE_DRAG :
      
      break;
      
   };
*/
}



MOUSE_STRATEGY FCFSMouseStrategy::GetStrategy() {
   return MOUSE_STRATEGY_FCFS;
}


