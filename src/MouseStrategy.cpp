


#include "MouseStrategy.hpp"
#include "WindowHandler.hpp"
#include "MouseTracker.hpp"




MMDECLSPEC MouseStrategy::MouseStrategy(MouseTracker* mt , WindowHandler* wh)  :
      mouse_tracker(mt),
      window_handler(wh)
{
}
