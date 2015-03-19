#include "Memory.h"

int __stdcall Memory::Write( void* pvAddress, void* pvWriteBuffer, size_t Size )
{
	DWORD dwAddress, dwOldProtectionFlags, dwSize;

	int iReturnCode;

	iReturnCode = 0;

	dwAddress = ROUND_DOWN ( pvAddress, 0x1000 );
	dwSize = ROUND_UP ( Size, 0x1000 );

	if ( VirtualProtect ( ( LPVOID )dwAddress, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionFlags ) )
	{
		memcpy ( pvAddress, pvWriteBuffer, Size );

		if ( VirtualProtect ( ( LPVOID )dwAddress, dwSize, dwOldProtectionFlags, &dwOldProtectionFlags ) )
		{
			if ( FlushInstructionCache ( GetCurrentProcess(), ( LPCVOID )dwAddress, dwSize ) )
			{
				iReturnCode = 0;
			}
			else
			{
				iReturnCode = GetLastError();
			}
		}
		else
		{
			iReturnCode = GetLastError();
		}
	}
	else
	{
		iReturnCode = GetLastError();
	}

	return iReturnCode;
};