


#include "MouseTracker.hpp"




/// ------------------ MouseTracker class -----------------------------------------





MouseTracker::MouseTracker() :
      mtinfo()
{

}



unsigned int MouseTracker::TrackNewMouse(Mouse* mouse , HANDLE hdev) {
   unsigned int new_id = 0;
   bool found_spot = false;
   for (new_id = 0 ; new_id < mtinfo.size() ; ++new_id) {
      bool used = mtinfo[new_id].used;
      if (!used) {
         found_spot = true;
         break;
      }
   }
   if (!found_spot) {
      mtinfo.resize(mtinfo.size() + 1);
   }
   mtinfo[new_id].used = true;
   mtinfo[new_id].id = new_id;
   mtinfo[new_id].hdev = hdev;
   mtinfo[new_id].mouse = mouse;
   return new_id;
}



void MouseTracker::StopTrackingMouse(HANDLE hdev) {
   for (unsigned int new_id = 0 ; new_id < mtinfo.size() ; ++new_id) {
      HANDLE hdev2 = mtinfo[new_id].hdev;
      if (hdev == hdev2) {
         Mouse* m = mtinfo[new_id].mouse;
         if (m) {
            delete m;
            mtinfo[new_id] = MouseTrackingInfo();// use default constructor to clear values
         }
      }
   }
}



MouseTracker::~MouseTracker() {
   CleanUp();
}



void MouseTracker::CleanUp() {
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         delete m;
      }
      mtinfo[i] = MouseTrackingInfo();// clear to default constructor
   }
   mtinfo.clear();
}



vector<unsigned int> MouseTracker::GetIdsInUse() {
   vector<unsigned int> ids_in_use;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      bool in_use = mtinfo[i].used;
      if (in_use) {
         ids_in_use.push_back(i);
      }
   }
   return ids_in_use;
}



Mouse* MouseTracker::GetMouseFromHandle(HANDLE hdev) {
   // linear search, so what there will probably never be more than 10 devices attached anyway
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      if (mtinfo[i].hdev == hdev) {
         return mtinfo[i].mouse;
      }
   }
   return (Mouse*)0;
}



unsigned int MouseTracker::NMice() {
   unsigned int nmice = 0;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         ++nmice;
      }
   }
   return nmice;
}



Mouse* MouseTracker::GetMouseByIndex(unsigned int index) {
   unsigned int idx = 0;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         if (idx == index) {
            return m;
         }
         ++idx;
      }
   }
   return (Mouse*)0;
}



vector<Mouse*> MouseTracker::GetMouseVector() {
   vector<Mouse*> micevec;
   for (unsigned int i = 0 ; i < mtinfo.size() ; ++i) {
      Mouse* m = mtinfo[i].mouse;
      if (m) {
         micevec.push_back(m);
      }
   }
   return micevec;
}


