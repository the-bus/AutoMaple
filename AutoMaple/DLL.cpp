#include <Windows.h>

//some stuff is taken from Derp trainer
VOID setTokenPrivilege(LPCSTR cPrivilege, HANDLE hProc)
{ // sh debugging
    TOKEN_PRIVILEGES tp;
    int bResult = 0;
    HANDLE hToken = NULL;
    unsigned long dwSize = 0;

    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;

    if(OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) &&
        LookupPrivilegeValue(NULL, cPrivilege, &tp.Privileges[0].Luid))
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        bResult = AdjustTokenPrivileges(hToken, 0, &tp, 0, NULL, &dwSize);
    }
    CloseHandle(hToken);
}
HMODULE mod = NULL;
HANDLE hThread = NULL;
extern void Main(void); // reserve main method
BOOL WINAPI DllWork ( __in ::HMODULE hModule )
{
	setTokenPrivilege((LPCSTR)SE_DEBUG_NAME, GetCurrentProcess()); 
	Main();
	return true;
}
BOOL WINAPI DllMain(__in::HMODULE hModule, __in::DWORD dwReason, __in __reserved::LPVOID lpvReserved)
{
	if ( dwReason == DLL_PROCESS_ATTACH )
	{
		mod = hModule;
		if (( hThread = ::CreateThread(NULL, 0, (::LPTHREAD_START_ROUTINE)&DllWork, (::HMODULE)hModule, 0, NULL) ) == NULL )
			return FALSE;
	}
	return TRUE;
}
#ifdef WIN
BOOL WINAPI WinMain( __in ::HINSTANCE hInstance, __in ::HINSTANCE hPrevInstance, __in ::LPSTR lpCmdLine, __in int nCmdShow ){
	DllWork(hInstance);
}
#endif