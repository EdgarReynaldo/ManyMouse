

#include "AllegroAndWindows.hpp"


class WindowHandler;
class MouseController;

MMDECLSPEC void SetWindowHandler(WindowHandler* wh);
MMDECLSPEC void SetMouseController(MouseController* mc);


LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);

MMDECLSPEC void StopMouse();

MMDECLSPEC void StartMouse();

LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam);




