



#ifndef MouseStrategy_HPP
#define MouseStrategy_HPP


#include "AllegroAndWindows.hpp"
#include "VisualLogger.hpp"

#include "Mouse.hpp"

//using ManyMouse::log;

class MouseTracker;
class WindowHandler;



enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_COLLABORATIVE = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2
};

#define NUM_MOUSE_STRATEGIES 3


enum HEAVY_MOUSE_STRATEGY_STATE {
   HEAVY_MOUSE_STATE_FREE = 0,
   HEAVY_MOUSE_STATE_GRABBING = 1,
   HEAVY_MOUSE_STATE_DRAGGING = 2
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


class HeavyMouseStrategy : public MouseStrategy {

   HEAVY_MOUSE_STRATEGY_STATE heavy_mouse_strategy_state;

   Mouse* grabbing_mouse;

   double helper_distance_threshold;

public :

   HeavyMouseStrategy(MouseTracker* mt , WindowHandler* wh) :
      MouseStrategy(mt,wh),
      heavy_mouse_strategy_state(HEAVY_MOUSE_STATE_FREE),
      grabbing_mouse(0),
      helper_distance_threshold(25.0)
   {
   }

   void HandleInput(RAWINPUT input);

   void Reset();

   bool OtherMiceNear();
   

//   void HandleInput(RAWINPUT input) {}
//*
//*/;
   
   void SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state);


   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};



#endif // MouseStrategy_HPP
