

#ifndef WindowTree_HPP
#define WindowTree_HPP



#include "AllegroAndWindows.hpp"
#include "WindowInfo.hpp"
#include "Mouse.hpp"
#include "VisualLogger.hpp"
#include "MouseController.hpp"

#include <vector>
using std::vector;


#include "DllBuild.hpp"




class MMDECLSPEC FakeMutex {
   
public :   
   FakeMutex() {}
   ~FakeMutex() {}
   
   void Free() {}
   bool Init() {return true;}
   void Lock() {}
   void Unlock() {}
   
};


extern MMDECLSPEC Mutex tree_mutex;

MMDECLSPEC bool InitTree();


MMDECLSPEC BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp);
MMDECLSPEC BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp);



class MMDECLSPEC WindowNode {
public :
   HWND hwindow;

   WindowNode* parent;

   vector<WindowNode> child_windows;

   WindowInfo info;

   friend MMDECLSPEC BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp);
   friend MMDECLSPEC BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp);

public :

///   WindowNode(HWND hwnd);
   WindowNode();
   WindowNode(HWND hwnd);

   int NumChildren() {return child_windows.size();}
};


MMDECLSPEC bool GetTopChild(WindowNode& root_node , POINT pscreen , WindowNode& store_node);


MMDECLSPEC void PrintWindowTree(vector<WindowNode>& nodevec , int depth = 0);




MMDECLSPEC int MaxDepth(vector<WindowNode>* nodevec);



class MMDECLSPEC WindowTree {

   vector<WindowNode> root_windows;

   HWND desktop_window;
   HWND shell_window;
   HWND taskbar_window;

   HWND program_window;
   HWND log_window;

   vector<HWND> mice_windows;

   MouseController* mc;


public :

///   WindowTree(MouseController* mouse_controller);
   WindowTree(MouseController* mouse_controller) :
         root_windows(),
         desktop_window(0),
         shell_window(0),
         taskbar_window(0),
         program_window(0),
         log_window(0),
         mice_windows(),
         mc(mouse_controller)
   {
      if (!tree_mutex.Init()) {
         ManyMouse::log.Log("Failed to init mutex in WindowTree constructor.\n");
      }
   }


   void EnumerateTree();

//   void Print();
   void Print() {
      PrintWindowTree(root_windows);
   }

   bool GetBaseWindowNode(POINT pscreen , WindowNode& store_node);
   HWND           GetBaseWindow(POINT pscreen);
   bool GetTopChildWindowNode(POINT pscreen , WindowNode& store_node);

///   void SetOurWindow(HWND program_window_handle);
   void SetOurWindow(HWND program_window_handle) {
      program_window = program_window_handle;
   }

   bool NotOurWindow(HWND hwindow);
   bool IsMouseWindow(HWND hwndA);

   void GetMiceWindows();
};





#endif // WindowTree_HPP

