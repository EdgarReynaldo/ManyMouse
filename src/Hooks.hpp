

#include "AllegroAndWindows.hpp"
///#include <windows.h>
///#include <winuser.h>



class WindowHandler;

void SetWindowHandler(WindowHandler* wh);

///__declspec(dllexport) LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);

void StopMouse();
///__declspec(dllexport) void StopMouse();

///__declspec(dllexport) void StartMouse();
void StartMouse();

LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam);
///__declspec(dllexport) LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam);



