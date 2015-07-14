




BOOL CALLBACK EnumerateWindowTree(HWND hwindow , LPARAM lp) {
   vector<WindowNode*> children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);
   
   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);
   
   children->push_back(node);
   
   return true;
}


BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp);
BOOL CALLBACK EnumerateWindowTreeChildren(HWND hwindow , LPARAM lp) {
   vector<WindowNode*> children = (vector<WindowNode>*)lp;
   ALLEGRO_ASSERT(children);
   WindowNode node(hwindow);
   
   EnumChildWindows(hwindow , EnumerateWindowTreeChildren, (LPARAM)&node.child_windows);
   
   children->push_back(node);
   return true;
}



class WindowNode {
   HWND hwindow;
   
   WindowNode* parent;
   
   vector<WindowNode> child_windows;
   
   WindowInfo info;
   
   WindowNode(HWND hwnd);
   WindowNode(HWND hwnd) :
         hwindow(hwnd),
         parent(0),
         child_windows(),
         info(hwnd)
   {
      
   }
   
   
   
};

class WindowTree {
   
   vector<WindowNode> root_windows;
   
   
   void EnumerateTree();
   void EnumerateTree() {
      root_windows.clear();
      EnumWindows(EnumerateWindowTree , (LPARAM)(&root_windows));
      SetParents();
   }
   
   
};



HWND GetBaseWindow(POINT pscreen);
HWND GetBaseWindow(POINT pscreen) {
   for (unsigned int i = 0 ; i < root_windows.size() ; ++i) {
      WindowNode& node = root_windows[i];
      HWND window = node.hwindow;
      POINT pc = pscreen;
      if (!ScreenToClient(window , &pc)) {
         return 0;
      }
      RECT rc;
      if (!GetClientRect(window , &rc)) {
         return 0;
      }
      if (pc.x >= rc.left &&
          pc.y >= rc.top &&
          pc.x < rc.right &&
          pc.y < rc.bottom) {
         return window;
      }
   }
}



WindowNode* GetBaseWindowNode(POINT pscreen);
WindowNode* GetBaseWindowNode(POINT pscreen) {
   for (unsigned int i = 0 ; i < root_windows.size() ; ++i) {
      WindowNode& node = root_windows[i];
      HWND window = node.hwindow;
      POINT pc = pscreen;
      if (!ScreenToClient(window , &pc)) {
         return 0;
      }
      RECT rc;
      if (!GetClientRect(window , &rc)) {
         return 0;
      }
      if (pc.x >= rc.left &&
          pc.y >= rc.top &&
          pc.x < rc.right &&
          pc.y < rc.bottom) {
         return &node;
      }
   }
}



HWND GetBaseWindow(POINT pscreen);
HWND GetBaseWindow(POINT pscreen) {
   WindowNode* root_node = GetBaseWindowNode(pscreen);
   if (root_node) {
      return root_node->hwindow;
   }
   return 0;
}



WindowNode* GetTopChildWindowNode(POINT pscreen);
WindowNode* GetTopChildWindowNode(POINT pscreen) {

   WindowNode* root_node = GetBaseWindowNode(pscreen);

   if (!root_node) {
      return 0;
   }
   
   WindowNode* child = root_node;
   
   while (child) {
      vector<WindowNode>& children = child->child_windows;
      for (unsigned int i = 0 ; i < children.size() ; ++i) {
         WindowNode* child_node = &children[i];
         HWND window = child_node->hwindow;
         POINT pc = pscreen;
         if (!ScreenToClient(window , &pc)) {
            continue;
            return 0;
         }
         RECT rc;
         if (!GetClientRect(window , &rc)) {
            continue;
            return 0;
         }
         if (pc.x >= rc.left &&
             pc.y >= rc.top &&
             pc.x < rc.right &&
             pc.y < rc.bottom) {
            child = child_node;
         }
      }
   }
   return child;
}



void SetParent(vector<WindowNode>* children , WindowNode* node_parent);
void SetParent(vector<WindowNode>* children , WindowNode* node_parent) {
   if (!children) {return;}
   
   for (unsigned int i = 0 ; i < children->size() ; ++i) {
      WindowNode* node = &(*children)[i];
      node->parent = node_parent;
      SetParent(&node->child_windows , node);
   }
}



void PrintTree(vector<WindowNode>* nodevec , int depth);
void PrintTree(vector<WindowNode>* nodevec , int depth) {
   if (!nodevec) {return;}
   
   int size = depth*3;
   char buf = new char[size + 1];
   memset(buf , ' ' , size);
   buf[size] = '\0';
   
   for (unsigned int i = 0 ; i < nodevec->size() ; ++i) {
      WindowNode& node = (*nodevec)[i];
      vector<WindowNode>* vec = &(node.child_windows);
      WindowInfo info(node.hwindow);
      vector<string> strs = SplitByNewLines(info.GetWindowInfoString());
      log.Log("\n");
      for (unsigned int j = 0 ; j < strs.size() ; ++j) {
         log.Log("%s%s\n" , buf , strs[j].c_str());
      }
      PrintTree(vec , depth + 1);
   }
   
   delete buf;
}



int MaxDepth(vector<WindowNode>* nodevec);
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


