


#ifndef MouseTracker_HPP
#define MouseTracker_HPP


#include "AllegroAndWindows.hpp"

#include "Mouse.hpp"
#include "MouseStrategy.hpp"


#include <vector>
using std::vector;



class MouseTrackingInfo {
public :
   bool used;
   unsigned int id;
   HANDLE hdev;
   Mouse* mouse;

//   MouseTrackingInfo();
   MouseTrackingInfo() :
         used(false),
         id(0),
         hdev(0),
         mouse(0)
   {

   }
};



/// MouseTracker is responsible for tracking and destroying mice, but does not create them

class MouseTracker {

private :
   vector<MouseTrackingInfo> mtinfo;

   MouseTracker& operator=(const MouseTracker& mt) {(void)mt;return *this;}// disallow copying
   MouseTracker(const MouseTracker& mt) {(void)mt;}// disallow copy constructor

public :

   MouseTracker();

   unsigned int TrackNewMouse(Mouse* mouse , HANDLE hdev);// adopts ownership of Mouse*
   void StopTrackingMouse(HANDLE hdev);

   ~MouseTracker();

   void CleanUp();

   vector<unsigned int> GetIdsInUse();// duh

   Mouse* GetMouseFromHandle(HANDLE hdev);

   unsigned int NMice();// Number of mice present
   Mouse* GetMouseByIndex(unsigned int index);// call NMice() to find out which indices are valid first

   vector<Mouse*> GetMouseVector();

};




#endif // MouseTracker_HPP

