

#ifndef NormalMouseStrategy_HPP
#define NormalMouseStrategy_HPP

#include "MouseStates.hpp"

#include "MouseStrategy.hpp"



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



#endif // NormalMouseStrategy_HPP

