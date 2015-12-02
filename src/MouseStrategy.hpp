



#ifndef MouseStrategy_HPP
#define MouseStrategy_HPP

#include "AllegroAndWindows.hpp"


class MouseTracker;
class WindowHandler;



enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_FCFS = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2,
   MOUSE_STRATEGY_COLLABORATIVE = 3
};
#define NUM_MOUSE_STRATEGIES 4


class MouseTracker;
class WindowHandler;



class MouseStrategy {

protected :
   MouseTracker* mouse_tracker;
   WindowHandler* window_handler;

public :

//   MouseStrategy();
   MouseStrategy(MouseTracker* mt , WindowHandler* wh);

   virtual void HandleInput(RAWINPUT input)=0;

   virtual void Reset()=0;

   virtual MOUSE_STRATEGY GetStrategy()=0;

};



#endif // MouseStrategy_HPP
