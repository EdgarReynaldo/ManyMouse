

#include "RawInputHandler.hpp"

#include "String.hpp"

#include "VisualLogger.hpp"

#include <cstdio>



bool lograwinput = false;



/// This callback runs on Allegro's thread! As well as HandleWindowMessage and PrintEtc
bool WindowProcCallback(ALLEGRO_DISPLAY *display, UINT message, WPARAM wparam, LPARAM lparam, LRESULT *result, void *userdata) {

   static int init = 0;
   
   if (!init) {
      printf("Thread ID of callback process is 0x%08lx\n" , GetCurrentThreadId());
      RegisterThread("WindowProcCallback thread" , GetCurrentThreadId());
      init = 1;
   }

   (void)display;

//   log.Log("WindowProcCallback received message %u\n" , message); 

   RawInputHandler* handler = (RawInputHandler*)userdata;
   
   if (!handler) {
      log.Log("Failed to get RawInputHandler*\n");
      return false;
   }

   return handler->HandleWindowMessage(message , wparam , lparam , result);
}



/// ------------------------------------ RawDeviceInfo ----------------------------------



void RawInputDevice::SetDeviceHandle(HANDLE hdev) {
   // hdev can be NULL, in that case we're monitoring the SendInput device??? Hell if I know
   hdevice = hdev;
   GetDeviceInfo();
}
   
void RawInputDevice::GetDeviceInfo() {
   error = false;
/*      
UINT WINAPI GetRawInputDeviceInfo(
_In_opt_    HANDLE hDevice,
_In_        UINT   uiCommand,
_Inout_opt_ LPVOID pData,
_Inout_     PUINT  pcbSize
);
UINT WINAPI GetRawInputDeviceInfo(HANDLE hDevice , UINT uiCommand , LPVOID pData , PUINT pcbSize);
*/

   /// Get device info
   UINT size = sizeof(RID_DEVICE_INFO);
   
   memset(&rid_info , -1 , size);
   
   int bytecount = -1;
   
   bytecount = GetRawInputDeviceInfo(hdevice , RIDI_DEVICEINFO , &rid_info , &size);
   
   if (bytecount < 1) {
      error = true;
   }
   
   /// Get device name
   dev_name = "";
   
   int charcount = -1;
   String s;
   UINT rsize = -1;
   TCHAR* buf = (TCHAR*)0;
   
   
//UINT WINAPI GetRawInputDeviceInfo(HANDLE hDevice , UINT uiCommand , LPVOID pData , PUINT pcbSize);
// When pData is NULL, the return value is zero
   charcount = GetRawInputDeviceInfo(hdevice , RIDI_DEVICENAME , 0 , &rsize);
   
   if (charcount != 0) {
      error = true;
   }
   else {
      size = rsize;// must set *pcbSize to correct size!
      buf = (TCHAR*)malloc(rsize);
//         buf = malloc(sizeof(TCHAR)*rsize);// ??? Windows is so damn vague
      if (!buf) {
         error = true;
      }
      else {
         charcount = GetRawInputDeviceInfo(hdevice , RIDI_DEVICENAME , buf , &size);
         if (charcount < 1 || size != rsize) {
            error = true;
            
         }
         else {
            s = buf;
            dev_name = GetStlString(s);
         }
         free(buf);
      }
      
      
   }
}



void RawInputDevice::PrintDeviceInfo() {
   /*
   typedef struct tagRID_DEVICE_INFO {
     DWORD cbSize;
     DWORD dwType;
     union {
       RID_DEVICE_INFO_MOUSE    mouse;
       RID_DEVICE_INFO_KEYBOARD keyboard;
       RID_DEVICE_INFO_HID      hid;
     };
   } RID_DEVICE_INFO, *PRID_DEVICE_INFO, *LPRID_DEVICE_INFO;
   typedef struct tagRID_DEVICE_INFO_MOUSE {
     DWORD dwId;
     DWORD dwNumberOfButtons;
     DWORD dwSampleRate;
     BOOL  fHasHorizontalWheel;
   } RID_DEVICE_INFO_MOUSE, *PRID_DEVICE_INFO_MOUSE;
   typedef struct tagRID_DEVICE_INFO_KEYBOARD {
     DWORD dwType;
     DWORD dwSubType;
     DWORD dwKeyboardMode;
     DWORD dwNumberOfFunctionKeys;
     DWORD dwNumberOfIndicators;
     DWORD dwNumberOfKeysTotal;
   } RID_DEVICE_INFO_KEYBOARD, *PRID_DEVICE_INFO_KEYBOARD;
   typedef struct tagRID_DEVICE_INFO_HID {
     DWORD  dwVendorId;
     DWORD  dwProductId;
     DWORD  dwVersionNumber;
     USHORT usUsagePage;
     USHORT usUsage;
   } RID_DEVICE_INFO_HID, *PRID_DEVICE_INFO_HID;
   */

   
   log.Log("Device name = (%s)\n" , dev_name.c_str());
   
   switch(rid_info.dwType) {
   case RIM_TYPEKEYBOARD :
      log.Log("Keyboard type = %lu\n" , rid_info.keyboard.dwType);
      log.Log("Keyboard sub type = %lu\n" , rid_info.keyboard.dwSubType);
      log.Log("Keyboard mode = %lu\n" , rid_info.keyboard.dwKeyboardMode);
      log.Log("Keyboard # of F-keys = %lu\n" , rid_info.keyboard.dwNumberOfFunctionKeys);
      log.Log("Keyboard # of Indicators = %lu\n" , rid_info.keyboard.dwNumberOfIndicators);
      log.Log("Keyboard total # of keys = %lu\n" , rid_info.keyboard.dwNumberOfKeysTotal);
      break;

   case RIM_TYPEMOUSE :
      log.Log("Mouse ID = %lu\n" , rid_info.mouse.dwId);
      log.Log("Number of mouse buttons = %lu\n" , rid_info.mouse.dwNumberOfButtons);
      log.Log("Mouse sample rate = %lu\n" , rid_info.mouse.dwSampleRate);
      log.Log("Mouse %s horizontal scroll wheel\n" , rid_info.mouse.fHasHorizontalWheel?"has a":"does not have a");
      break;

   case RIM_TYPEHID :
      log.Log("HID vendor Id = %lu\n" , rid_info.hid.dwVendorId);
      log.Log("HID product Id = %lu\n" , rid_info.hid.dwProductId);
      log.Log("HID version number = %lu\n" , rid_info.hid.dwVersionNumber);
      log.Log("HID usage page = %hu\n" , rid_info.hid.usUsagePage);
      log.Log("HID usage = %hu\n" , rid_info.hid.usUsage);
      break;
   }
   log.Log("\n");
}



/// --------------------------------- RawInputHandler ----------------------------------------------



int RawInputHandler::SetupAllegro() {

   
   if (!al_init()) {return 1;}
   
   if (!al_init_primitives_addon()) {return 1;}
   
   if (!al_init_image_addon()) {return 1;}
   
   if (!al_init_font_addon()) {return 1;}
   
   if (!al_init_ttf_addon()) {return 1;}
   
   if (!al_install_mouse()) {return 2;}
   if (!al_install_keyboard()) {return 2;}
   
   if (!al_install_joystick()) {
      return -1;
   }
   
   bool mtx = log.InitMutex();
   log.Log("log.InitMutex() returned %s\n" , mtx?"true":"false");

//   if (!al_install_timer()) {return 2;}
   
   sw = 1024;
   sh = 300;
   
   
//   al_set_new_display_flags(ALLEGRO_FRAMELESS | ALLEGRO_WINDOWED);
   al_set_new_display_flags(ALLEGRO_WINDOWED);
///   al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED);
//   al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);/// todo - this causes assertion failure in vec.c somehow
   
//   display = al_create_display(0,0);
//   display = al_create_display(200,200);
///   display = al_create_display(1280,800);
   display = al_create_display(sw,sh);
   
   if (!display) {return 3;}
   
   al_clear_to_color(al_map_rgb(255,255,255));
///   al_clear_to_color(al_map_rgba(255,255,255,0));
///   al_draw_filled_rectangle(320,0,640,400 , al_map_rgba(0,255,0,255));
   al_flip_display();
   
   timer = al_create_timer(1.0/60.0);
   if (!timer) {return 4;}
   
   queue = al_create_event_queue();
   
   if (!queue) {return 4;}
   
   al_register_event_source(queue , al_get_display_event_source(display));
   al_register_event_source(queue , al_get_keyboard_event_source());
   al_register_event_source(queue , al_get_mouse_event_source());
   al_register_event_source(queue , al_get_timer_event_source(timer));
   
   mutex = al_create_mutex();
   
   if (!mutex) {return 5;}
   
   font = al_load_ttf_font("verdana.ttf" , -12 , ALLEGRO_TTF_MONOCHROME);
   
   if (!font) {return 6;}


   bool imgs_loaded = mouse_controller.CreateMouseImages();

   log.Log("Images %s\n" , imgs_loaded?"loaded successfully":"failed to load");

//HWND al_get_win_window_handle(ALLEGRO_DISPLAY *display)
   winhandle = al_get_win_window_handle(display);
   
   window_handler.SetOurWindows(winhandle , 0);
   
/*
al_win_add_window_callback

bool al_win_add_window_callback(ALLEGRO_DISPLAY *display,
   bool (*callback)(ALLEGRO_DISPLAY *display, UINT message, WPARAM wparam,
   LPARAM lparam, LRESULT *result, void *userdata), void *userdata)   
*/
/*
BOOL WINAPI SetWindowPos(
  _In_     HWND hWnd,
  _In_opt_ HWND hWndInsertAfter,
  _In_     int  X,
  _In_     int  Y,
  _In_     int  cx,
  _In_     int  cy,
  _In_     UINT uFlags
);
*/

   /// Using HWND_TOPMOST has no effect on the focus, except in the initial call
   SetWindowPos(winhandle , HWND_TOPMOST , 0 , 0 , -1 , -1 , SWP_NOMOVE | SWP_NOSIZE);
/*
   if (0 == SetWindowLong(handle , GWL_EXSTYLE , WS_EX_LAYERED)) {
      log.Log("Couldn't set WS_EX_LAYERED style attribute\n");
   }
*/   
   al_win_add_window_callback(display , WindowProcCallback , this);

   return 0;
}



void RawInputHandler::ShutdownAllegro() {
   // mutex , queue , timer , display
   mouse_controller.FreeMouseImages();
   
   if (mutex) {
      al_destroy_mutex(mutex);
      mutex = 0;
   }
   if (queue) {
      al_destroy_event_queue(queue);
      queue = 0;
   }
   if (timer) {
      al_destroy_timer(timer);
      timer = 0;
   }
   if (display) {
      al_destroy_display(display);
      display = 0;
   }
   
   window_handler.SetOurWindows(0 , 0);
}



void RawInputHandler::InputLoop() {

   bool redraw = true;
   bool quit = false;


   al_start_timer(timer);
   while (!quit) {
      
      if (redraw) {
         
         
         al_set_target_backbuffer(display);
         al_clear_to_color(al_map_rgb(0,0,0));
//         printf("Before DrawLog()\n");
         log.DrawLog(font , al_map_rgb(255,255,255) , 10 , sh);
//         printf("After DrawLog()\n");
         al_flip_display();
         
         mouse_controller.Draw();
         
      }
      
      do {
         ALLEGRO_EVENT ev;
         al_wait_for_event(queue , &ev);
         
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            quit = true;
            break;
         }  
         if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            quit = true;
            break;
         }
         
         if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
         }
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_G) {
            mouse_controller.ToggleMiceEnabled();
            printf("Mice image toggled.\n");
         }  
         
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_W) {
            window_handler.EnumerateWindows();
            log.Log("\n");
            window_handler.PrintWindowInfo();
         }
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_D) {
            PrintDeviceInfo();
         }
         
/*         
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_P) {
            window_handler.PrintWindows();
         }  
         if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_A) {
            window_handler.FindAllWindowsZOrder();
            window_handler.PrintAllWindows();
         }  
*/         
      } while (!al_is_event_queue_empty(queue));
      
      
   }
   
}



void RawInputHandler::FreeRawInfo() {
   // Does not handle display
   
   dev_info_map.clear();
   
   device_count = 0;
   
   mice.clear();
   keyboards.clear();
   hids.clear();
   
   rids.clear();

}




bool RawInputHandler::InitRawInfo() {

   bool success = true;

   FreeRawInfo();
   
   SetupDefaultDevices();
   
   // Setup RawInputDevice
/*
typedef struct tagRAWINPUTDEVICE {
USHORT usUsagePage;
USHORT usUsage;
DWORD  dwFlags;
HWND   hwndTarget;
} RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;
*/      

   // Get size
   GetRawInputDeviceList(NULL , &device_count , sizeof(RAWINPUTDEVICELIST));
   
   log.Log("Num raw input devices detected = %u\n" , device_count);

   const UINT FAIL = UINT(-1);
   
   RAWINPUTDEVICELIST init;
   init.hDevice = (HANDLE)-1;
   init.dwType = -1;
   
   vector<RAWINPUTDEVICELIST> ridlist(device_count , init);
   
   UINT result = GetRawInputDeviceList(&ridlist[0] , &device_count , sizeof(RAWINPUTDEVICELIST));
   
   if (FAIL == result) {
      log.Log("GetRawInputDeviceList failed.\n");
      log.Log("GetLastError reported error %lu. (ERROR_INSUFFICENT_BUFFER = %ld)\n" , GetLastError() , ERROR_INSUFFICIENT_BUFFER);
      return false;
   }




/** OLD

   TCHAR* buf = 0;
   UINT rsize = 0;
   UINT size = 0;
   
   
   
   
   for (UINT i = 0 ; i < device_count ; ++i) {
      RAWINPUTDEVICELIST r = ridlist[i];
      RID_DEVICE_INFO* info = &ridinfo[i];
      std::string* namestr = &ridnames[i];
      
      log.Log("Checking device %u\n" , i);
      
      if (GetRawInputDeviceInfo(r.hDevice, RIDI_DEVICENAME, 0 , &rsize) != 0) {// get size of data in rsize
         // error
         log.Log("Error, could not retrieve necessary size for RIDI_DEVICENAME buffer.\n");
         // return false;
         success = false;
         continue;
      }
      
      buf = (TCHAR*)realloc(buf , rsize*sizeof(TCHAR));
      if (!buf) {
         // error no memory
         log.Log("Error, out of memory. Could not allocate buf in InitRawInfo.\n");
         success = false;
         continue;
      }
      memset(buf , 0 , rsize*sizeof(TCHAR));
      
      /// must set size each time before calling GetRawInputDeviceInfo
      size = rsize;
      
      int count = 0;

      count = GetRawInputDeviceInfo(r.hDevice, RIDI_DEVICENAME, buf , &size);// store data in buf
      
      log.Log("Char count is %i\n" , count);
      
//      if (GetRawInputDeviceInfo(r.hDevice, RIDI_DEVICENAME, buf , &size) != rsize) // store data in buf
      if (size != rsize) {
         // error
         log.Log("Error, did not read full string for RIDI_DEVICENAME buffer.\n");
         success = false;
//         continue;
      }
      log.Log("After GRIDI #%u, buf holds (%s)\n" , i , buf);
      
      *namestr = buf;
      
      size = sizeof(RID_DEVICE_INFO);
      info->cbSize = sizeof(RID_DEVICE_INFO);
//      if (GetRawInputDeviceInfo(r.hDevice, RIDI_DEVICEINFO, info, &size) != sizeof(RID_DEVICE_INFO)) 

      count = GetRawInputDeviceInfo(r.hDevice, RIDI_DEVICEINFO, info, &size);

      log.Log("Byte count is %i , sizeof(RID_DEVICE_INFO) = %u\n" , count , sizeof(RID_DEVICE_INFO));
      
      if (size != sizeof(RID_DEVICE_INFO)) {
         // error reading device info
         log.Log("Error, could not read rid device info for device %u.\n" , i);
         // return false;
         success = false;
         continue;
      }

      
      assert(r.dwType == info->dwType);

      switch (info->dwType) {
      case RIM_TYPEKEYBOARD :
         ++keyboard_count;
         keyboards.push_back(r);
         keyboard_info.push_back(info->keyboard);
         break;
      case RIM_TYPEMOUSE :
         ++mouse_count;
         mice.push_back(r);
         mouse_info.push_back(info->mouse);
         break;
      case RIM_TYPEHID : 
         ++hid_count;
         hids.push_back(r);
         hid_info.push_back(info->hid);
         break;
      default : 
         log.Log("Unknown device type for device # %u\n" , i);
         break;
      };
      
   }

   if (buf) {
      free(buf);
      buf = 0;
   }
//*/      
      

   /// NEW ******************************************
   for (UINT i = 0 ; i < device_count ; ++i) {
      RAWINPUTDEVICELIST r = ridlist[i];

      RawInputDevice rid;
      rid.SetDeviceHandle(r.hDevice);
      if (rid.Error()) {
         log.Log("Error retrieving raw device info for hDevice %p\n" , r.hDevice);
         success = false;
         continue;// don't add this device to the map
      }

      dev_info_map[r.hDevice] = rid;
   }
   
///   PrintDeviceInfo();
   
   
   return success;

}



void RawInputHandler::PrintDeviceInfo() {
   map<HANDLE , RawInputDevice>::iterator it = dev_info_map.begin();
   int i = 0;
   while (it != dev_info_map.end()) {
      log.Log("\nDevice # %u Handle %p\n" , i , it->first);
      it->second.PrintDeviceInfo();
      ++i;
      ++it;
   }
}



void RawInputHandler::SetupDefaultDevices() {
   
   RAWINPUTDEVICE default_devices[2];

   

   // adds HID mouse and also ignores legacy mouse messages
   default_devices[0].usUsagePage = 0x01; // generic desktop controls
   default_devices[0].usUsage = 0x02; // mouse
//      default_devices[0].dwFlags = RIDEV_NOLEGACY;   
   default_devices[0].dwFlags = RIDEV_INPUTSINK;   
//   default_devices[0].dwFlags = 0;
///   default_devices[0].dwFlags = RIDEV_NOLEGACY;
/// RIDEV_INPUTSINK doesn't work for some reason, registration fails
///   default_devices[0].dwFlags = RIDEV_INPUTSINK | RIDEV_NOLEGACY;
   default_devices[0].hwndTarget = winhandle;
//   default_devices[0].hwndTarget = 0;

   // adds HID keyboard and also ignores legacy keyboard messages
   default_devices[1].usUsagePage = 0x01; // generic desktop controls
   default_devices[1].usUsage = 0x06; // keyboard
//      default_devices[1].dwFlags = RIDEV_NOLEGACY;// don't use nolegacy or allegro won't know about key presses
   default_devices[1].dwFlags = RIDEV_INPUTSINK;
//   default_devices[1].dwFlags = 0;
///   default_devices[1].dwFlags = RIDEV_NOLEGACY;
/// RIDEV_INPUTSINK doesn't work for some reason, registration fails
///   default_devices[1].dwFlags = RIDEV_INPUTSINK | RIDEV_NOLEGACY;
   default_devices[1].hwndTarget = winhandle;
//   default_devices[1].hwndTarget = 0;

   rids.push_back(default_devices[0]);
   rids.push_back(default_devices[1]);
   
}



bool RawInputHandler::RegisterDevices() {
//   bool success = true;
/*   
BOOL WINAPI RegisterRawInputDevices(
  _In_ PCRAWINPUTDEVICE pRawInputDevices,
  _In_ UINT             uiNumDevices,
  _In_ UINT             cbSize
);
*/
   
   bool ret = false;

   log.Log("rids.size() = %u\n" , rids.size());

   ret = RegisterRawInputDevices(&rids[0] , rids.size() , sizeof(RAWINPUTDEVICE));
   
   if (!ret) {
      log.Log("Register Raw Input Devices unsuccessful. GetLastError reports %lu.\n" , GetLastError());
   }
   
   return ret;
}



bool RawInputHandler::HandleWindowMessage(UINT message , WPARAM wparam , LPARAM lparam , LRESULT* result) {
   
   bool ret = false;// false indicates we didn't handle this message
   
   (void)result;// unused for now
   
   /**
      /// Only on Vista or higher, would have to change _WIN32_WINNT to 0x0600
      if (message == WM_INPUT_DEVICE_CHANGE) {
         log.Log("WM_INPUT_DEVICE_CHANGE received.\n");
         return true;
      }
   */
   
   // return true to consume event, false to let allegro handle it as well
   
   /// We never get WM_CREATE because the callback is registered after the window is created


   if (message == WM_INPUT) {
      ret = true;// we handle all WM_INPUT messages
      
///      printf("WM_INPUT received. ");
      
      UINT rsize = 0;
      UINT size = 0;
      
      UINT count = GetRawInputData((HRAWINPUT)lparam , RID_INPUT , NULL , &rsize , sizeof(RAWINPUTHEADER));
      
      if (count != 0) {
         log.Log("Failed to get size of data from GetRawInputData.\n");
      }
      else {// GetRawInputData returned 0, success reading size
         
///      void* rawmem = malloc(4*(rsize/4 + 1));// some people experienced alignment issues
         void* rawmem = malloc(rsize);// 
         
         if (!rawmem) {
            log.Log("Ran out of memory.\n");
         }
         else {
            memset(rawmem , 0 , rsize);
            size = rsize;
            
            count = GetRawInputData((HRAWINPUT)lparam , RID_INPUT , rawmem , &size , sizeof(RAWINPUTHEADER));
            if (count != rsize) {
               log.Log("Failed to retrieve RAWINPUT data from GetRawIputData. return value = %u\n" , count);
            }

            RAWINPUT* raw = (RAWINPUT*)rawmem;
            RAWMOUSE* rms = 0;
            RAWKEYBOARD* rkb = 0;
            RAWHID* rhid = 0;
            
            if (lograwinput) {
               switch(raw->header.dwType) {
               case RIM_TYPEKEYBOARD :
                  rkb = (RAWKEYBOARD*)&raw->data.keyboard;
                  PrintRawKeyboard(raw->header , rkb);
   //               HandleRawKeyboard(raw->header , rkb);
                  break;
               case RIM_TYPEMOUSE :
                  rms = (RAWMOUSE*)&raw->data.mouse;
                  PrintRawMouse(raw->header , rms);
   //               HandleRawMouse(raw->header , rms);
                  break;
               case RIM_TYPEHID :
                  rhid = (RAWHID*)&raw->data.hid;
                  PrintRawHid(raw->header , rhid);
   //               HandleRawHid(raw->header , rhid);
                  break;
               default :
                  assert(0);
                  break;
               }
            }
            HandleRawInput(*raw);
         }
         
         if (rawmem) {
            free(rawmem);
            rawmem = 0;
         }
      
         
      }
      
      /// In the case of WM_INPUT, we need to let windows see the message too
      DefWindowProc(winhandle , message , wparam , lparam);
      
   }// end if (message == WM_INPUT)
   
   return ret;
   
}
/*         
typedef struct tagRAWINPUT {
  RAWINPUTHEADER header;
  union {
    RAWMOUSE    mouse;
    RAWKEYBOARD keyboard;
    RAWHID      hid;
  } data;
} RAWINPUT, *PRAWINPUT, *LPRAWINPUT;

typedef struct tagRAWINPUTHEADER {
  DWORD  dwType;
  DWORD  dwSize;
  HANDLE hDevice;
  WPARAM wParam;
} RAWINPUTHEADER, *PRAWINPUTHEADER;

typedef struct tagRAWMOUSE {
  USHORT usFlags;
  union {
    ULONG  ulButtons;
    struct {
      USHORT usButtonFlags;
      USHORT usButtonData;
    };
  };
  ULONG  ulRawButtons;
  LONG   lLastX;
  LONG   lLastY;
  ULONG  ulExtraInformation;
} RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

typedef struct tagRAWKEYBOARD {
  USHORT MakeCode;
  USHORT Flags;
  USHORT Reserved;
  USHORT VKey;
  UINT   Message;
  ULONG  ExtraInformation;
} RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;

typedef struct tagRAWHID {
  DWORD dwSizeHid;
  DWORD dwCount;
  BYTE  bRawData[1];
} RAWHID, *PRAWHID, *LPRAWHID;
*/
/*
UINT WINAPI GetRawInputData(
  _In_      HRAWINPUT hRawInput,
  _In_      UINT      uiCommand,
  _Out_opt_ LPVOID    pData,
  _Inout_   PUINT     pcbSize,
  _In_      UINT      cbSizeHeader
);
*/


void RawInputHandler::PrintRawHeader(RAWINPUTHEADER hdr) {

   const char* typestr = 0;

   switch(hdr.dwType) {
   case RIM_TYPEKEYBOARD :
      typestr = "Keyboard";
      break;
   case RIM_TYPEMOUSE :
      typestr = "Mouse";
      break;
   case RIM_TYPEHID :
      typestr = "Hid";
      break;
   default :
      typestr = "Unknown";
      break;
   }
   
   log.Log("%s input RAWHDR : hDevice = 0x%08x , wParam = %d , dwType = %s , dwSize = %lu\n" ,
           typestr , (UINT)(hdr.hDevice) , hdr.wParam , typestr , hdr.dwSize);
}
   
void RawInputHandler::PrintRawMouse(RAWINPUTHEADER hdr , RAWMOUSE* rms) {
   PrintRawHeader(hdr);
/*

typedef struct tagRAWMOUSE {
  USHORT usFlags;
  union {
    ULONG  ulButtons;
    struct {
      USHORT usButtonFlags;
      USHORT usButtonData;
    };
  };
  ULONG  ulRawButtons;
  LONG   lLastX;
  LONG   lLastY;
  ULONG  ulExtraInformation;
} RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

*/
   
   log.Log("usFlags = %hu , ulButtons = %u {usButtonFlags = %hu , usButtonData = %hu}\n" ,
           rms->usFlags , rms->ulButtons , rms->usButtonFlags , rms->usButtonData);
   log.Log("ulRawButtons = %u , lLastX = %li , lLastY = %li , ulExtraInformation = %lu\n" , 
           rms->ulRawButtons , rms->lLastX , rms->lLastY , rms->ulExtraInformation);
   
}
void RawInputHandler::PrintRawKeyboard(RAWINPUTHEADER hdr , RAWKEYBOARD* rkb) {
   PrintRawHeader(hdr);
/*

typedef struct tagRAWKEYBOARD {
  USHORT MakeCode;
  USHORT Flags;
  USHORT Reserved;
  USHORT VKey;
  UINT   Message;
  ULONG  ExtraInformation;
} RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;

*/
   log.Log("MakeCode = %hu , Flags = %hu , Reserved = %hu\n" , 
           rkb->MakeCode , rkb->Flags , rkb->Reserved);
   log.Log("VKey = %hu , Message = %u , ExtraInformation = %lu",
           rkb->VKey , rkb->Message , rkb->ExtraInformation);
}
void RawInputHandler::PrintRawHid(RAWINPUTHEADER hdr , RAWHID* rhid) {
   PrintRawHeader(hdr);
/*

typedef struct tagRAWHID {
  DWORD dwSizeHid;
  DWORD dwCount;
  BYTE  bRawData[1];
} RAWHID, *PRAWHID, *LPRAWHID;

*/
   log.Log("dwSizeHid = %lu , dwCount = %lu , bRawData = %p" ,
           rhid->dwSizeHid , rhid->dwCount  , rhid->bRawData);
}

void RawInputHandler::HandleRawInput(RAWINPUT rinput) {

   HANDLE hdev = rinput.header.hDevice;
   map<HANDLE , RawInputDevice>::iterator it = dev_info_map.find(hdev);
   if (it == dev_info_map.end()) {
      RawInputDevice rid;
      rid.SetDeviceHandle(hdev);
      if (!rid.Error()) {
         dev_info_map[hdev] = rid;
      }
      else {
         log.Log("Error getting device info for handle %p\n" , hdev);
      }
   }
   if (rinput.header.dwType == RIM_TYPEMOUSE) {
      mouse_controller.HandleRawInput(rinput);
///      window_handler.GetMiceWindows();
   }
}



/*




typedef struct tagRAWMOUSE {
  USHORT usFlags;
  union {
    ULONG  ulButtons;
    struct {
      USHORT usButtonFlags;
      USHORT usButtonData;
    };
  };
  ULONG  ulRawButtons;
  LONG   lLastX;
  LONG   lLastY;
  ULONG  ulExtraInformation;
} RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

Members

usFlags

    Type: USHORT

    The mouse state. This member can be any reasonable combination of the following.
    Value	Meaning

    MOUSE_ATTRIBUTES_CHANGED
    0x04

    	

    Mouse attributes changed; application needs to query the mouse attributes.

    MOUSE_MOVE_RELATIVE
    0

    	

    Mouse movement data is relative to the last mouse position.

    MOUSE_MOVE_ABSOLUTE
    1

    	

    Mouse movement data is based on absolute position.

    MOUSE_VIRTUAL_DESKTOP
    0x02

    	

    Mouse coordinates are mapped to the virtual desktop (for a multiple monitor system).

     
ulButtons

    Type: ULONG

    Reserved.
usButtonFlags

    Type: USHORT

    The transition state of the mouse buttons. This member can be one or more of the following values.
    Value	Meaning

    RI_MOUSE_LEFT_BUTTON_DOWN
    0x0001

    	

    Left button changed to down.

    RI_MOUSE_LEFT_BUTTON_UP
    0x0002

    	

    Left button changed to up.

    RI_MOUSE_MIDDLE_BUTTON_DOWN
    0x0010

    	

    Middle button changed to down.

    RI_MOUSE_MIDDLE_BUTTON_UP
    0x0020

    	

    Middle button changed to up.

    RI_MOUSE_RIGHT_BUTTON_DOWN
    0x0004

    	

    Right button changed to down.

    RI_MOUSE_RIGHT_BUTTON_UP
    0x0008

    	

    Right button changed to up.

    RI_MOUSE_BUTTON_1_DOWN
    0x0001

    	

    RI_MOUSE_LEFT_BUTTON_DOWN

    RI_MOUSE_BUTTON_1_UP
    0x0002

    	

    RI_MOUSE_LEFT_BUTTON_UP

    RI_MOUSE_BUTTON_2_DOWN
    0x0004

    	

    RI_MOUSE_RIGHT_BUTTON_DOWN

    RI_MOUSE_BUTTON_2_UP
    0x0008

    	

    RI_MOUSE_RIGHT_BUTTON_UP

    RI_MOUSE_BUTTON_3_DOWN
    0x0010

    	

    RI_MOUSE_MIDDLE_BUTTON_DOWN

    RI_MOUSE_BUTTON_3_UP
    0x0020

    	

    RI_MOUSE_MIDDLE_BUTTON_UP

    RI_MOUSE_BUTTON_4_DOWN
    0x0040

    	

    XBUTTON1 changed to down.

    RI_MOUSE_BUTTON_4_UP
    0x0080

    	

    XBUTTON1 changed to up.

    RI_MOUSE_BUTTON_5_DOWN
    0x100

    	

    XBUTTON2 changed to down.

    RI_MOUSE_BUTTON_5_UP
    0x0200

    	

    XBUTTON2 changed to up.

    RI_MOUSE_WHEEL
    0x0400

    	

    Raw input comes from a mouse wheel. The wheel delta is stored in usButtonData.

     
usButtonData

    Type: USHORT

    If usButtonFlags is RI_MOUSE_WHEEL, this member is a signed value that specifies the wheel delta.
ulRawButtons

    Type: ULONG

    The raw state of the mouse buttons.
lLastX

    Type: LONG

    The motion in the X direction. This is signed relative motion or absolute motion, depending on the value of usFlags.
lLastY

    Type: LONG

    The motion in the Y direction. This is signed relative motion or absolute motion, depending on the value of usFlags.
ulExtraInformation

    Type: ULONG

    The device-specific additional information for the event.


*/

