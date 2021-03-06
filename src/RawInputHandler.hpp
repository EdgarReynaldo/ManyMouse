



#ifndef RawInputHandler_HPP
#define RawInputHandler_HPP

#include "AllegroAndWindows.hpp"
#include "Mouse.hpp"
#include "WindowHandler.hpp"
#include "TransparentWindow.hpp"
#include "Hooks.hpp"
#include "KeyboardHandler.hpp"

#include <string>
#include <vector>
using std::vector;
using std::string;

#include <cstdio>

extern bool lograwinput;// value of lograwinput specifies whether or not the raw input is logged





class RawInputDevice {

   HANDLE hdevice;

   RID_DEVICE_INFO rid_info;

   string dev_name;

   bool error;

public :

///   RawInputDevice();
RawInputDevice() :
      hdevice((HANDLE)0),
      rid_info(),
      dev_name(""),
      error(false)
{
   memset(&rid_info , -1 , sizeof(RID_DEVICE_INFO));
}

   void SetDeviceHandle(HANDLE hdev);
   void GetDeviceInfo();/// SetDeviceHandle calls this automatically, call Error() to see if it fails

   bool Error() {return error;}

   void PrintDeviceInfo();

   RID_DEVICE_INFO Info() {return rid_info;}

};




class RawInputHandler {

protected :
public:
   int tww;// transparent window width
   int twh;// transparent window height

   ALLEGRO_DISPLAY* display;
   ALLEGRO_TIMER* timer;
   ALLEGRO_FONT* font;
   ALLEGRO_EVENT_QUEUE* queue;
   ALLEGRO_MUTEX* mutex;
   HWND winhandle;

   ALLEGRO_BITMAP* allegro_buffer;
   DIBbuffer dib_buffer;

   ALLEGRO_DISPLAY* log_display;
   int lww;// log window width
   int lwh;// log window height

   map<HANDLE , RawInputDevice> dev_info_map;

   UINT device_count;// total number of raw input devices on system

   vector<RAWINPUTDEVICELIST> mice;
   vector<RAWINPUTDEVICELIST> keyboards;
   vector<RAWINPUTDEVICELIST> hids;

   vector<RAWINPUTDEVICE> rids;
//   RAWINPUTDEVICE rids[2];

   MOUSE_STRATEGY new_mouse_strategy;
   MOUSE_STRATEGY current_mouse_strategy;

   MouseController mouse_controller;

   WindowHandler window_handler;

   KeyboardHandler keyboard_handler;

   bool registered;




   HMODULE hMod_hook_dll;

   HOOKPROC ll_mouse_hook_func;
   HOOKPROC mouse_hook_func;
   ///   LRESULT CALLBACK (*ll_mouse_hook_func) (int , WPARAM , LPARAM);
///   LRESULT CALLBACK (*mouse_hook_func) (int , WPARAM , LPARAM);
   void (*start_mouse_func)();
   void (*stop_mouse_func)();
   HHOOK ll_mouse_hook;
   HHOOK mouse_hook;

   HOOKPROC shell_hook_func;
///   LRESULT CALLBACK (*shell_hook_func) (int , WPARAM , LPARAM);
   HHOOK shellhook;



///   WindowTree window_tree;



/*
FARPROC WINAPI GetProcAddress(
  _In_ HMODULE hModule,
  _In_ LPCSTR  lpProcName
);
/// From Hooks.dll

LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);

void StopMouse();

void StartMouse();

*/


   void DrawHandlerToDIB();


public :
//   RawInputHandler();
   RawInputHandler() :
         tww(640),
         twh(400),
         display(0),
         timer(0),
         font(0),
         queue(0),
         mutex(0),
         winhandle(0),
         allegro_buffer(0),
         dib_buffer(),
         log_display(0),
         lww(1024),
         lwh(300),
         dev_info_map(),
         device_count(0),
         mice(),
         keyboards(),
         hids(),
         rids(),
         new_mouse_strategy(MOUSE_STRATEGY_NORMAL),
         current_mouse_strategy(MOUSE_STRATEGY_NORMAL),
         mouse_controller(&window_handler),
         window_handler(&mouse_controller),
         keyboard_handler(),
         registered(false),
         hMod_hook_dll(0),
         ll_mouse_hook_func(0),
         mouse_hook_func(0),
         start_mouse_func(0),
         stop_mouse_func(0),
         ll_mouse_hook(0),
         mouse_hook(0),
         shell_hook_func(0),
         shellhook(0)
///         window_tree()
   {
      mouse_controller.SetWindowHandler(&window_handler);
      SetupHooks();
   }

   ~RawInputHandler() {
      CloseWindows();
      FreeHooks();
   }

   void SetupHooks();
   void FreeHooks();
   int SetupWindows();
   void CloseWindows();

   void InputLoop();

   void FreeRawInfo();
   bool InitRawInfo();

   void PrintDeviceInfo();

   void SetupDefaultDevices(bool swallow_mouse);
   bool RegisterDevices(bool swallow_mouse);
   void UnRegisterDevices();

   void QueuePaintMessage();

   bool HandleWindowMessage(UINT message , WPARAM wparam , LPARAM lparam , LRESULT* result);

   void PrintRawHeader(RAWINPUTHEADER hdr);

   void PrintRawMouse(RAWINPUTHEADER hdr , RAWMOUSE* rms);
   void PrintRawKeyboard(RAWINPUTHEADER hdr , RAWKEYBOARD* rkb);
   void PrintRawHid(RAWINPUTHEADER hdr , RAWHID* rhid);

   void HandleRawInput(RAWINPUT rinput);
   
};



#endif // RawInputHandler_HPP

