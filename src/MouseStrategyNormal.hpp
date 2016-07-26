

#ifndef NormalMouseStrategy_HPP
#define NormalMouseStrategy_HPP

#include "MouseStates.hpp"

#include "MouseStrategy.hpp"



#include "DllBuild.hpp"



class MMDECLSPEC NormalMouseStrategy : public MouseStrategy {

   NORMAL_MOUSE_STATE mouse_state;

public :
   NormalMouseStrategy(MouseTracker* mt , WindowHandler* wh);

   void HandleInput(RAWINPUT input);

   void Reset();
   
   virtual ALLEGRO_BITMAP* GetMouseImage(Mouse* m);/// This returns a shallow temporary reference, not a new image, so copy it
   
   void ResetImages();
   void ToggleEnabled();

   MOUSE_STRATEGY GetStrategy();
   NORMAL_MOUSE_STATE GetState();
};



#endif // NormalMouseStrategy_HPP

