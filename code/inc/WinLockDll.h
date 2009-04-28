#ifndef WINLOCKDLL_H
#define WINLOCKDLL_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _THREAD_DATA
{
	HDESK hDesk;
	char  szDesktopName[20];
} THREAD_DATA;

#ifdef  _DLL_
#define DLL_EXP_IMP __declspec(dllexport)
#else
#define DLL_EXP_IMP __declspec(dllimport)
#endif

#ifndef BUILDING_WINLOCK_DLL
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifdef _DEBUG
			#pragma comment(lib, "WinLock_d.lib")
		#else
			#pragma comment(lib, "WinLock.lib")
		#endif			
	#endif
#endif

DLL_EXP_IMP int WINAPI ShowDesktop(BOOL bShow);
DLL_EXP_IMP int WINAPI ShowStartButton(BOOL bShow);
DLL_EXP_IMP int WINAPI ShowTaskbar(BOOL bShow);
DLL_EXP_IMP int WINAPI ShowClock(BOOL bShow);
DLL_EXP_IMP int WINAPI EnableKeys(BOOL bEnable);
DLL_EXP_IMP int WINAPI EnableAltTab1(BOOL bEnable);
DLL_EXP_IMP int WINAPI EnableAltTab2(HWND hWnd, BOOL bEnable);
DLL_EXP_IMP int WINAPI EnableTaskSwitching(BOOL bEnable);
DLL_EXP_IMP int WINAPI EnableTaskManager(BOOL bEnable);
DLL_EXP_IMP int WINAPI EnableCtrlAltDel(BOOL bEnable);
DLL_EXP_IMP int WINAPI Thread_Desktop(LPTHREAD_START_ROUTINE ThreadFunc, THREAD_DATA *td);
DLL_EXP_IMP int WINAPI Process_Desktop(char *szDesktopName, char *szPath);

#ifdef __cplusplus
}
#endif
#endif
