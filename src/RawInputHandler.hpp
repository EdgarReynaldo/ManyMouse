



#ifndef RawInputHandler_HPP
#define RawInputHandler_HPP

#include "AllegroAndWindows.hpp"
#include "Mouse.hpp"
#include "WindowHandler.hpp"


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
   int sw;
   int sh;
   
   ALLEGRO_DISPLAY* display;
   ALLEGRO_TIMER* timer;
   ALLEGRO_FONT* font;
   ALLEGRO_EVENT_QUEUE* queue;
   ALLEGRO_MUTEX* mutex;
   HWND winhandle;
   
   map<HANDLE , RawInputDevice> dev_info_map;
///   PRAWINPUTDEVICELIST ridlist;
///   RID_DEVICE_INFO* ridinfo;
///   std::string* ridnames;

   UINT device_count;// total number of raw input devices on system
///   UINT keyboard_count;
///   UINT mouse_count;
///   UINT hid_count;
   
   vector<RAWINPUTDEVICELIST> mice;
   vector<RAWINPUTDEVICELIST> keyboards;
   vector<RAWINPUTDEVICELIST> hids;
   
///   vector<RID_DEVICE_INFO_MOUSE> mouse_info;
///   vector<RID_DEVICE_INFO_KEYBOARD> keyboard_info;
///   vector<RID_DEVICE_INFO_HID> hid_info;
   
   vector<RAWINPUTDEVICE> rids;
//   RAWINPUTDEVICE rids[2];
   
   MouseController mouse_controller;   
   
   WindowHandler window_handler;
   
public :
//   RawInputHandler();
   RawInputHandler() :
         sw(1024),
         sh(300),
         display(0),
         timer(0),
         font(0),
         queue(0),
         mutex(0),
         winhandle(0),
         dev_info_map(),
///         ridlist(0),
///         ridinfo(0),
///         ridnames(0),
         device_count(0),
///         keyboard_count(0),
///         mouse_count(0),
///         hid_count(0),
         mice(),
         keyboards(),
         hids(),
///         mouse_info(),
///         keyboard_info(),
///         hid_info(),
         rids(),
         mouse_controller(),
         window_handler(&mouse_controller)
   {
      
   }
   ~RawInputHandler() {
      ShutdownAllegro();
   }

   int SetupAllegro();
   void ShutdownAllegro();
   void InputLoop();

   void FreeRawInfo();
   bool InitRawInfo();
   
   void PrintDeviceInfo();
   
   void SetupDefaultDevices();
   bool RegisterDevices();
   
   bool HandleWindowMessage(UINT message , WPARAM wparam , LPARAM lparam , LRESULT* result);
      
   void PrintRawHeader(RAWINPUTHEADER hdr);

   void PrintRawMouse(RAWINPUTHEADER hdr , RAWMOUSE* rms);
   void PrintRawKeyboard(RAWINPUTHEADER hdr , RAWKEYBOARD* rkb);
   void PrintRawHid(RAWINPUTHEADER hdr , RAWHID* rhid);
   
   void HandleRawInput(RAWINPUT rinput);
};



#endif // RawInputHandler_HPP

