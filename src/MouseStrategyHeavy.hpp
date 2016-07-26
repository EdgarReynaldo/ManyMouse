


#ifndef HeavyMouseStrategy_HPP
#define HeavyMouseStrategy_HPP


#include <vector>
#include <utility>
using std::vector;
using std::pair;


#include "MouseStrategy.hpp"


enum HEAVY_MOUSE_STRATEGY_STATE {
   HEAVY_MOUSE_STATE_FREE = 0,
   HEAVY_MOUSE_STATE_DETERMINING_CLICK = 1,
   HEAVY_MOUSE_STATE_GRABBING = 2,
   HEAVY_MOUSE_STATE_DRAGGING = 3
};


class Mouse;


#include "DllBuild.hpp"



class MMDECLSPEC HeavyMouseStrategy : public MouseStrategy {

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

   virtual ALLEGRO_BITMAP* GetMouseImage(Mouse* m);/// This returns a shallow temporary reference, not a new image, so copy it

   bool OtherMiceNear();
   bool MouseNear(Mouse* m);
   
   bool GroupAgrees();/// True if more than half of mice are near and in button 1 down state

//   void HandleInput(RAWINPUT input) {}
//*
//*/;
   
   void SetHeavyMouseStrategyState(HEAVY_MOUSE_STRATEGY_STATE new_state);


   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};



#endif // HeavyMouseStrategy_HPP
