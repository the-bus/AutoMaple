#include "inc.h"
#define len 512
int32_t MsgBox(PSTR sz, ...)
{
	char ach[len];
	va_list args;
	va_start(args, sz);
	vsnprintf_s(ach, len, sz, args);
	int32_t retval = Message(ach);
	return retval;
}
uint32_t VKtoMS(uint32_t key) {
	return (MapVirtualKey(key, 0) << 16);
}
HWND WINAPI FindProcessWindow(__in_z LPCSTR lpcszWindowClass, __in DWORD dwProcessId)
{
	DWORD	dwWindowId;
	CHAR	pszClassName[200];
	HWND 	hWnd;

	hWnd = GetTopWindow(NULL);

	while (hWnd != NULL)
	{
		if (GetClassNameA(hWnd, pszClassName, 200) > 0)
			if (lstrcmpiA(lpcszWindowClass, pszClassName) == 0)
				if (GetWindowThreadProcessId(hWnd, &dwWindowId))
					if (dwWindowId == dwProcessId)
						return hWnd;

		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return NULL;
}
void GetFilePathExe(char * buf, const char * file, uint64_t sz) {
	GetModuleFileNameA(NULL, buf, sz);
	PathRemoveFileSpecA(buf);
	strcat_s(buf, sz, "\\");
	strcat_s(buf, sz, file);
}
HWND hwnd;
HWND GetMShwnd() {
	if (!IsWindow(hwnd))
		hwnd = FindProcessWindow("MapleStoryClass", GetCurrentProcessId());
	return hwnd;
}