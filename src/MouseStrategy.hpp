



#ifndef MouseStrategy_HPP
#define MouseStrategy_HPP


#include "AllegroAndWindows.hpp"
#include "VisualLogger.hpp"

#include "Mouse.hpp"

//using ManyMouse::log;
using std::pair;

class MouseTracker;
class WindowHandler;



enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_FCFS = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2,
   MOUSE_STRATEGY_COLLABORATIVE = 3
};
#define NUM_MOUSE_STRATEGIES 4

enum FCFS_STRATEGY_STATE {
   FCFS_STATE_FREE = 0,
   FCFS_STATE_PROCESSING_CLICK = 1,
   FCFS_STATE_DRAG = 2
};

enum HEAVY_MOUSE_STRATEGY_STATE {
   HEAVY_MOUSE_STATE_FREE = 0,
   HEAVY_MOUSE_STATE_DETERMINING_CLICK = 1,
   HEAVY_MOUSE_STATE_GRABBING = 2,
   HEAVY_MOUSE_STATE_DRAGGING = 3
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

   virtual void Reset()=0;

   virtual MOUSE_STRATEGY GetStrategy()=0;

};


class NormalMouseStrategy : public MouseStrategy {

   NORMAL_MOUSE_STATE mouse_state;

public :
   NormalMouseStrategy(MouseTracker* mt , WindowHandler* wh);

   void HandleInput(RAWINPUT input);

   void Reset();
   void ResetImages();
   void ToggleEnabled();

   MOUSE_STRATEGY GetStrategy();
   NORMAL_MOUSE_STATE GetState();
};


class FCFSMouseStrategy : public MouseStrategy {

   FCFS_STRATEGY_STATE fcfs_state;

   ALLEGRO_BITMAP* fcfs_mouse_buffer;

   Mouse* grabbing_mouse;

   double mouse_drag_duration_threshold;
   double grabbing_mouse_starting_time;
   
public :
   FCFSMouseStrategy(MouseTracker* mt , WindowHandler* wh);
   ~FCFSMouseStrategy();

   void FreeBuffer();

   void HandleInput(RAWINPUT input);
   virtual void HandleInput(RAWINPUT input , bool process_input);

   void Reset();
   void ResetImages();
   void DrawPointers(FCFS_STRATEGY_STATE strategy_state);
   
   void ToggleEnabled();

   virtual void SetFCFSStrategyState(FCFS_STRATEGY_STATE new_state);

   virtual MOUSE_STRATEGY GetStrategy();
};



class TurnBasedFCFSMouseStrategy : public FCFSMouseStrategy {
   map<Mouse* , int> mouse_turn_count;
   
}



class HeavyMouseStrategy : public MouseStrategy {

   HEAVY_MOUSE_STRATEGY_STATE heavy_mouse_strategy_state;

   Mouse* grabbing_mouse;

   double helper_distance_threshold;
   double mouse_drag_duration_threshold;
   double grabbing_mouse_starting_time;

   vector<pair<Mouse* , pair<int , int> > >other_mice_offsets;

public :

   HeavyMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
      MouseStrategy(mt,wh),
      heavy_mouse_strategy_state(HEAVY_MOUSE_STATE_FREE),
      grabbing_mouse(0),
      helper_distance_threshold(25.0),
      mouse_drag_duration_threshold(0.5),
      grabbing_mouse_starting_time(0.0),
      other_mice_offsets()
   {
   }

   void HandleInput(RAWINPUT input);
   void HandleInput(RAWINPUT input , bool process_input);


   void Reset();

   bool OtherMiceNear();
   bool MouseNear(Mouse* m);
   
   bool GroupAgrees();/// True if more than half of mice are near and in button 1 down state

//   void HandleInput(RAWINPUT input) {}
//*
//*/;
   
   void SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state);


   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};



#endif // MouseStrategy_HPP
