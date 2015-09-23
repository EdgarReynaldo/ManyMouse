


#include "WindowTree.hpp"

#include "VisualLogger.hpp"
using ManyMouse::log;

#include "String.hpp"

#include <cstdio>

Mutex tree_mutex;

bool InitTree() {
   return tree_mutex.Init();
}


/// ---------------------------------------    Window tree enumeration functions   ---------------------------------------------



BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp) {
   vector<WindowNode>* children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);

   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);

   if (IsWindowVisible(hwindow)) {
      children->push_back(node);
   }

   return true;
}


BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp) {
   vector<WindowNode>* children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);

   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);

   children->push_back(node);
   return true;
}



/// Private global function declarations
void SetParent(vector<WindowNode>* children , WindowNode* node_parent);
void PrintTree(FILE* outfile , vector<WindowNode>* nodevec , int depth = 0);
void PrintNodeToFile(FILE* outfile , WindowNode* node);
string PrintNode(WindowNode* node , int depth = 0);




/// -------------------------------------------   WindowTree class   -------------------------------------------------------



void WindowTree::EnumerateTree() {

   tree_mutex.Lock();

   root_windows.clear();
   EnumWindows(EnumerateWindowTree , (LPARAM)(&root_windows));
   SetParent(&root_windows , NULL);

   desktop_window = GetDesktopWindow();
   shell_window = GetShellWindow();
   taskbar_window = FindWindow("Shell_TrayWnd" , NULL);

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


bool WindowTree::GetBaseWindowNode(POINT pscreen , WindowNode& store_node) {

   tree_mutex.Lock();

   for (unsigned int i = 0 ; i < root_windows.size() ; ++i) {
      WindowNode& node = root_windows[i];
      HWND window = node.hwindow;

      if (!NotOurWindow(window)) {continue;}


      POINT pc = pscreen;
/*
      if (!ScreenToClient(window , &pc)) {
         log.Log("WindowTree::GetBaseWindowNode() - ScreenToClient failed. GetLastError reports %d\n" , GetLastError());
         continue;
      }
*/
      RECT rc;
      if (!GetWindowRect(window , &rc)) {
         log.Log("WindowTree::GetBaseWindowNode() - GetWindowRect failed. GetLastError reports %d\n" , GetLastError());
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






HWND WindowTree::GetBaseWindow(POINT pscreen) {
   WindowNode store_node;
   if (GetBaseWindowNode(pscreen , store_node)) {
      return store_node.hwindow;
   }
   return 0;
}



bool WindowTree::GetTopChildWindowNode(POINT pscreen , WindowNode& store_node) {

   WindowNode base_node;
   WindowNode child_node;

   if (!GetBaseWindowNode(pscreen , base_node)) {
      return false;
   }

   return GetTopChild(base_node , pscreen , child_node);
}



bool WindowTree::NotOurWindow(HWND hwindow) {

   if ((hwindow != program_window) &&
       (hwindow != log_window) &&
       !(IsMouseWindow(hwindow)) )
   {
      return true;
   }
   return false;
}



bool WindowTree::IsMouseWindow(HWND hwndA) {
   for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
      HWND hwndmouse = mice_windows[i];
      if (hwndA == hwndmouse) {
         return true;
      }
   }
   return false;
}



void WindowTree::GetMiceWindows() {
   if (mc) {
      mice_windows.clear();
      mc->GetMiceWindows(&mice_windows);
   }
}



/// -------------------------------------------------------   Global functions   ------------------------------------------------------------




bool GetTopChild(WindowNode& root_node , POINT pscreen , WindowNode& store_node) {

   vector<WindowNode>& children = root_node.child_windows;

   for (unsigned int i = 0 ; i < children.size() ; ++i) {
      WindowNode& child_node = children[i];
      HWND window = child_node.hwindow;
      POINT pc = pscreen;
      RECT rc;
      if (!GetWindowRect(window , &rc)) {
         log.Log("GetTopChild() - GetWindowRect failed. GetLastError reports %d" , GetLastError());
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



void SetParent(vector<WindowNode>* children , WindowNode* node_parent) {
   if (!children) {return;}

   for (unsigned int i = 0 ; i < children->size() ; ++i) {
      WindowNode* node = &(*children)[i];
      node->parent = node_parent;
      SetParent(&node->child_windows , node);
   }
}



void PrintNodeToFile(FILE* outfile , WindowNode* node) {
   string s = PrintNode(node , 0);
   fprintf(outfile , s.c_str());
/*
   vector<string> lines = SplitByNewLines(s);
   for (unsigned int i = 0 ; i < lines.size() ; ++i) {

   }
*/
}



string PrintNode(WindowNode* node, int depth) {
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



void PrintTree(FILE* outfile , vector<WindowNode>* nodevec , int depth) {
   if (!nodevec) {return;}
   if (!outfile) {return;}

   int size = depth*3;
   char* buf = new char[size + 1];
   memset(buf , ' ' , size);
   buf[size] = '\0';

   for (unsigned int i = 0 ; i < nodevec->size() ; ++i) {
      WindowNode& node = (*nodevec)[i];
      vector<WindowNode>* vec = &(node.child_windows);
      WindowInfo info(node.hwindow);
      vector<string> strs = SplitByNewLines(info.GetWindowInfoString());
      fprintf(outfile , "\n");
      for (unsigned int j = 0 ; j < strs.size() ; ++j) {
         fprintf(outfile , "%s%s\n" , buf , strs[j].c_str());
      }
      PrintTree(outfile , vec , depth + 1);
   }

   delete buf;
}



int MaxDepth(vector<WindowNode>* nodevec) {
   int maxdepth = 1;
   for (unsigned int i = 0 ; i < nodevec->size() ; ++i) {
      WindowNode* node = &(*nodevec)[i];
      vector<WindowNode>* vec = &(node->child_windows);
      int depth = MaxDepth(vec);
      if (depth > maxdepth) {maxdepth = depth;}
   }
   return maxdepth - 1;
}



void PrintWindowTree(FILE* outfile , vector<WindowNode>& nodevec , int depth) {
   tree_mutex.Lock();
   PrintTree(outfile , &nodevec , depth);
   tree_mutex.Unlock();
}



