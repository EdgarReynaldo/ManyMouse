



#ifndef RawInputHandler_HPP
#define RawInputHandler_HPP

#include "AllegroAndWindows.hpp"
#include "Mouse.hpp"
#include "WindowHandler.hpp"
#include "TransparentWindow.hpp"


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
   void GetDeviceInfo();// SetDeviceHandle calls this automatically, call Error() to see if it fails

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
   
   MouseController mouse_controller;   
   
   WindowHandler window_handler;
   
   bool registered;




   HMODULE hMod_hook_dll;
   
   LRESULT CALLBACK (*ll_mouse_hook_func) (int , WPARAM , LPARAM);
   void (*start_mouse_func)();
   void (*stop_mouse_func)();
   HHOOK hhook;
   
   LRESULT CALLBACK (*shell_hook_func) (int , WPARAM , LPARAM);
   HHOOK shellhook;


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
         mouse_controller(),
         window_handler(&mouse_controller),
         registered(false),
         hMod_hook_dll(0),
         ll_mouse_hook_func(0),
         start_mouse_func(0),
         stop_mouse_func(0),
         hhook(0),
         shell_hook_func(0),
         shellhook(0)
   {
      mouse_controller.SetWindowHandler(&window_handler);
/*
      HMODULE WINAPI LoadLibrary(
        _In_ LPCTSTR lpFileName
      );      
LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);

void StopMouse();

void StartMouse();
_Z10StartMousev
*/
      hMod_hook_dll = LoadLibrary("Hooks.dll");
      log.Log("Hook dll %s\n" , hMod_hook_dll?"loaded successfully":"failed to load");
      if (hMod_hook_dll) {
         ll_mouse_hook_func = (LRESULT CALLBACK (*)(int , WPARAM , LPARAM))GetProcAddress(hMod_hook_dll , "_Z17LowLevelMouseHookijl@12");
         if (!ll_mouse_hook_func) {
            log.Log("LowLevelMouseHook failed to load. GetLastError reports %d\n" , GetLastError());
         }
         else {
            log.Log("LowLevelMouseHook loaded successfully.\n");
         }

         start_mouse_func = (void (*) ())GetProcAddress(hMod_hook_dll , "_Z10StartMousev");
         if (!start_mouse_func) {
            log.Log("StartMouse failed to load. GetLastError reports %d\n" , GetLastError());
         }
         else {
            log.Log("StartMouse loaded successfully.\n");
         }

         stop_mouse_func = (void (*) ())GetProcAddress(hMod_hook_dll , "_Z9StopMousev");
         if (!stop_mouse_func) {
            log.Log("StopMouse failed to load. GetLastError reports %d\n" , GetLastError());
         }
         else {
            log.Log("StopMouse loaded successfully.\n");
         }
         
         
         
         
         hhook = SetWindowsHookEx(WH_MOUSE_LL , ll_mouse_hook_func , hMod_hook_dll , 0);
         if (!hhook) {
            log.Log("SetWindowsHookEx failed. GetLastError says %d\n" , GetLastError());
         }
         else {
            log.Log("SetWindowsHookEx succeeded.\n");
         }
         
         /// Setup our hook to monitor shell commands like window creation, activation, and destruction
         /// Make sure to init window handler first
         
         shell_hook_func = (LRESULT CALLBACK (*)(int , WPARAM , LPARAM))
                           GetProcAddress(hMod_hook_dll , "");         


         shellhook = SetWindowsHookEx(WH_SHELL , shell_hook_func , hMod_hook_dll , 0);
         
      }
   }

   ~RawInputHandler() {
      CloseWindows();
      if (shellhook) {
         log.Log("UnhookWindowsHookEx(shellhook) returned %s\n" , UnhookWindowsHookEx(shellhook)?"true":"false");
         shellhook = 0;
      }
      if (hhook) {
         log.Log("UnhookWindowsHookEx returned %s\n" , UnhookWindowsHookEx(hhook)?"true":"false");
         hhook = 0;
      }
      if (hMod_hook_dll) {
         FreeLibrary(hMod_hook_dll);
         hMod_hook_dll = 0;
      }
   }

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

