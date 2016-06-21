

#include <tchar.h>

#include "WindowTree.hpp"

#include "VisualLogger.hpp"
//using ManyMouse::log;

#include "String.hpp"

#include <cstdio>

MMDECLSPEC Mutex tree_mutex;

MMDECLSPEC bool InitTree() {
   return tree_mutex.Init();
}


/// ---------------------------------------    Window tree enumeration functions   ---------------------------------------------



MMDECLSPEC BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp) {
   vector<WindowNode>* children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);

   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);

   if (IsWindowVisible(hwindow)) {
      children->push_back(node);
   }

   return true;
}



MMDECLSPEC BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp) {
   vector<WindowNode>* children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);

   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);

   children->push_back(node);
   return true;
}



/// Private global function declarations
void MMDECLSPEC SetParent(vector<WindowNode>* children , WindowNode* node_parent);
void MMDECLSPEC PrintTree(vector<WindowNode>* nodevec , int depth = 0);
void MMDECLSPEC PrintNodeToFile(WindowNode* node);
string MMDECLSPEC PrintNode(WindowNode* node , int depth = 0);




/// -------------------------------------------   WindowNode class   -------------------------------------------------------


MMDECLSPEC WindowNode::WindowNode() :
      hwindow(0),
      parent(0),
      child_windows(),
      info()
{

}



MMDECLSPEC WindowNode::WindowNode(HWND hwnd) :
      hwindow(hwnd),
      parent(0),
      child_windows(),
      info(hwnd)
{

}




/// -------------------------------------------   WindowTree class   -------------------------------------------------------



MMDECLSPEC void WindowTree::EnumerateTree() {

   tree_mutex.Lock();

   root_windows.clear();
   EnumWindows(EnumerateWindowTree , (LPARAM)(&root_windows));
   SetParent(&root_windows , NULL);

   desktop_window = GetDesktopWindow();
   shell_window = GetShellWindow();
   taskbar_window = FindWindow(_T("Shell_TrayWnd") , NULL);

   GetMiceWindows();

   vector<WindowNode>::iterator vit = root_windows.begin();
   while (vit != root_windows.end()) {

      WindowNode* wnode = &(*vit);
      HWND hwndA = wnode->hwindow;

      WindowInfo* pwi = &wnode->info;

      if (hwndA == desktop_window) {
         pwi->SetWindowTypeString("The desktop window");
      }
      else if (hwndA == shell_window) {
         pwi->SetWindowTypeString("The shell window");
      }
      else if (hwndA == taskbar_window) {
         pwi->SetWindowTypeString("The taskbar window");
      }
      else if (hwndA == program_window) {
         pwi->SetWindowTypeString("Our program window");
      }
      else if (hwndA == log_window) {
         pwi->SetWindowTypeString("Our log window");
      }
      else if (IsMouseWindow(hwndA)) {
         for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
            HWND hwndmouse = mice_windows[i];
            if (hwndA == hwndmouse) {
               pwi->SetWindowTypeString(StringPrintF("Our mouse window #%u" , i));
            }
         }
      }
      else {
         pwi->SetWindowTypeString("A user window");
      }

      ++vit;

   }

   tree_mutex.Unlock();

}


MMDECLSPEC bool WindowTree::GetBaseWindowNode(POINT pscreen , WindowNode& store_node) {

   tree_mutex.Lock();

   for (unsigned int i = 0 ; i < root_windows.size() ; ++i) {
      WindowNode& node = root_windows[i];
      HWND window = node.hwindow;

      if (!NotOurWindow(window)) {continue;}


      POINT pc = pscreen;
/*
      if (!ScreenToClient(window , &pc)) {
         ManyMouse::log.Log("WindowTree::GetBaseWindowNode() - ScreenToClient failed. GetLastError reports %d\n" , GetLastError());
         continue;
      }
*/
      RECT rc;
      if (!GetWindowRect(window , &rc)) {
         ManyMouse::log.Log("WindowTree::GetBaseWindowNode() - GetWindowRect failed. GetLastError reports %d\n" , GetLastError());
         continue;
      }
      if (pc.x >= rc.left &&
          pc.y >= rc.top &&
          pc.x < rc.right &&
          pc.y < rc.bottom) {


         store_node = node;

         tree_mutex.Unlock();

         return true;
      }
   }

   tree_mutex.Unlock();

   return false;
}






MMDECLSPEC HWND WindowTree::GetBaseWindow(POINT pscreen) {
   WindowNode store_node;
   if (GetBaseWindowNode(pscreen , store_node)) {
      return store_node.hwindow;
   }
   return 0;
}



MMDECLSPEC bool WindowTree::GetTopChildWindowNode(POINT pscreen , WindowNode& store_node) {

   WindowNode base_node;

   if (!GetBaseWindowNode(pscreen , base_node)) {
      return false;
   }

   return GetTopChild(base_node , pscreen , store_node);
}



MMDECLSPEC bool WindowTree::NotOurWindow(HWND hwindow) {

   if ((hwindow != program_window) &&
       (hwindow != log_window) &&
       !(IsMouseWindow(hwindow)) )
   {
      return true;
   }
   return false;
}



MMDECLSPEC bool WindowTree::IsMouseWindow(HWND hwndA) {
   for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
      HWND hwndmouse = mice_windows[i];
      if (hwndA == hwndmouse) {
         return true;
      }
   }
   return false;
}



MMDECLSPEC void WindowTree::GetMiceWindows() {
   if (mc) {
      mice_windows.clear();
      mc->GetMiceWindows(&mice_windows);
   }
}



/// -------------------------------------------------------   Global functions   ------------------------------------------------------------




MMDECLSPEC bool GetTopChild(WindowNode& root_node , POINT pscreen , WindowNode& store_node) {

   vector<WindowNode>& children = root_node.child_windows;

   for (unsigned int i = 0 ; i < children.size() ; ++i) {
      WindowNode& child_node = children[i];
      HWND window = child_node.hwindow;
      POINT pc = pscreen;
      RECT rc;
      if (!GetWindowRect(window , &rc)) {
         ManyMouse::log.Log("GetTopChild() - GetWindowRect failed. GetLastError reports %d" , GetLastError());
         continue;
      }
      if (pc.x >= rc.left &&
          pc.y >= rc.top &&
          pc.x < rc.right &&
          pc.y < rc.bottom) {
///         over_child = true;
         return GetTopChild(child_node , pscreen , store_node);
      }
   }
   store_node = root_node;
   return true;
}



MMDECLSPEC void SetParent(vector<WindowNode>* children , WindowNode* node_parent) {
   if (!children) {return;}

   for (unsigned int i = 0 ; i < children->size() ; ++i) {
      WindowNode* node = &(*children)[i];
      node->parent = node_parent;
      SetParent(&node->child_windows , node);
   }
}



MMDECLSPEC void PrintNodeToFile(WindowNode* node) {
   string s = PrintNode(node , 0);
   ManyMouse::log.Log("%s\n" , s.c_str());
}



MMDECLSPEC string PrintNode(WindowNode* node, int depth) {
   if (!node) {
      return "";
   }
   string s;
   int size = depth*3;
   char* buf = new char[size + 1];
   memset(buf , ' ' , size);
   buf[size] = '\0';

   WindowInfo info = node->info;
   string istr = info.GetWindowInfoString();
   vector<string> lines = SplitByNewLines(istr);
   for (unsigned int i = 0 ; i < lines.size() ; ++i) {
         s += buf;
         s += lines[i];
         s += '\n';
   }


   vector<WindowNode>& children = node->child_windows;
   for (unsigned int i = 0 ; i < children.size() ; ++i) {
      WindowNode* child_node = &children[i];
        s += PrintNode(child_node , depth + 1);
   }

   delete buf;

   return s;
}



MMDECLSPEC void PrintTree(vector<WindowNode>* nodevec , int depth) {
   if (!nodevec) {return;}

   int size = depth*3;
   char* buf = new char[size + 1];
   memset(buf , ' ' , size);
   buf[size] = '\0';

   for (unsigned int i = 0 ; i < nodevec->size() ; ++i) {
      WindowNode& node = (*nodevec)[i];
      vector<WindowNode>* vec = &(node.child_windows);
      WindowInfo info(node.hwindow);
      string infostr = info.GetWindowInfoString();
      const char* str = infostr.c_str();
      ManyMouse::log.Log("%s%s\n" , buf , str);
/*      
      vector<string> strs = SplitByNewLines(info.GetWindowInfoString());
      fprintf(outfile , "\n");
      for (unsigned int j = 0 ; j < strs.size() ; ++j) {
         fprintf(outfile , "%s%s\n" , buf , strs[j].c_str());
      }
*/
      PrintTree(vec , depth + 1);
   }

   delete buf;
}



MMDECLSPEC int MaxDepth(vector<WindowNode>* nodevec) {
   int maxdepth = 1;
   for (unsigned int i = 0 ; i < nodevec->size() ; ++i) {
      WindowNode* node = &(*nodevec)[i];
      vector<WindowNode>* vec = &(node->child_windows);
      int depth = MaxDepth(vec);
      if (depth > maxdepth) {maxdepth = depth;}
   }
   return maxdepth - 1;
}



MMDECLSPEC void PrintWindowTree(vector<WindowNode>& nodevec , int depth) {
   tree_mutex.Lock();
   PrintTree(&nodevec , depth);
   tree_mutex.Unlock();
}



