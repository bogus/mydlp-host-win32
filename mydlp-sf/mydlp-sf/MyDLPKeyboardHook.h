#include <windows.h>
#include "MyDLPEventLogger.h"
#include "MyDLPRemoteConf.h"

//
// some data will be shared across all
// instances of the DLL
//
//#pragma comment(linker, "/SECTION:.SHARED,RWS")
//#pragma data_seg(".SHARED")
int iKeyCount = 0;
HHOOK hKeyboardHook = 0;
HHOOK hMouseHook = 0;
//#pragma data_seg()

//
// instance specific data
//
HMODULE hInstance = 0;

//
// DLL load/unload entry point
//
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
   switch (dwReason)
   {
   case DLL_PROCESS_ATTACH :
      hInstance = (HINSTANCE) hModule;
      break;

   case DLL_THREAD_ATTACH :
      break;

   case DLL_THREAD_DETACH :
      break;

   case DLL_PROCESS_DETACH :
      break;
   }
   return TRUE;
}

//
// keyboard hook
//
LRESULT CALLBACK KeyboardProc(int code,       // hook code
                              WPARAM wParam,  // virtual-key code
                              LPARAM lParam)  // keystroke-message information
{
   printf("---------------\n");
	if(wParam == VK_SNAPSHOT)
	{
		mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen Hit");
	}
	mydlpsf::MyDLPEventLogger::GetInstance()->LogScreenCapture("Print Screen Hit " + wParam.ToString());

   return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}


//
// install keyboard/mouse hooks
//
void InstallHooks(void)
{
   hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, 0);
}

//
// remove keyboard/mouse hooks
//
void RemoveHooks(void)
{
   UnhookWindowsHookEx(hKeyboardHook); 
   hKeyboardHook = 0;
}

//
// retrieve number of keystrokes
//
int FetchKeyCount(bool bClear)
{
   int kc = iKeyCount;
   if (bClear)
      iKeyCount = 0;
   return kc;
}
