#include <windows.h>
#include <winuser.h>
/*bool HookMouse();
bool HookMouse(){

}*/
__declspec(dllexport) LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);

__declspec(dllexport) void StopMouse();

__declspec(dllexport) void StartMouse();






