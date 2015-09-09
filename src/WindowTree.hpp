

#ifndef WindowTree_HPP
#define WindowTree_HPP



#include "AllegroAndWindows.hpp"
#include "WindowInfo.hpp"
#include "Mouse.hpp"
#include "VisualLogger.hpp"
#include "MouseController.hpp"

#include <vector>
using std::vector;



BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp);
BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp);



class WindowNode {
public :
   HWND hwindow;

   WindowNode* parent;

   vector<WindowNode> child_windows;

   WindowInfo info;

   friend BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp);
   friend BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp);

public :

///   WindowNode(HWND hwnd);
   WindowNode(HWND hwnd) :
         hwindow(hwnd),
         parent(0),
         child_windows(),
         info(hwnd)
   {

   }

   int NumChildren() {return child_windows.size();}
};


WindowNode* GetTopChild(WindowNode* root_node , POINT pscreen);

void SetParent(vector<WindowNode>* children , WindowNode* node_parent);
void PrintTree(FILE* outfile , vector<WindowNode>* nodevec , int depth = 0);
void PrintNodeToFile(FILE* outfile , WindowNode* node);
string PrintNode(WindowNode* node , int depth = 0);

int MaxDepth(vector<WindowNode>* nodevec);



class WindowTree {

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

   }


   void EnumerateTree();

//   void Print();
   void Print() {
      PrintTree(ManyMouse::log.GetLogFile() ,  &root_windows);
   }

   WindowNode* GetBaseWindowNode(POINT pscreen);
   HWND           GetBaseWindow(POINT pscreen);
   WindowNode* GetTopChildWindowNode(POINT pscreen);

///   void SetOurWindow(HWND program_window_handle);
   void SetOurWindow(HWND program_window_handle) {
      program_window = program_window_handle;
   }

   bool NotOurWindow(HWND hwindow);
   bool IsMouseWindow(HWND hwndA);

   void GetMiceWindows();
};





#endif // WindowTree_HPP

