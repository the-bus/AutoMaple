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

bool Memory::Pointer::valid(unsigned long base, unsigned long offset)
{
	ULONG_PTR* ulBase = (ULONG_PTR *)base;
	if ( !IsBadReadPtr((VOID*)ulBase, sizeof(ULONG_PTR)) )
		if ( !IsBadReadPtr((VOID*)((*(ULONG_PTR*)ulBase)+offset), sizeof(ULONG_PTR)) )
			return true;
	return false;
};

int Memory::Pointer::read_int(unsigned long base, unsigned long offset)
{
	if( Memory::Pointer::valid( base, offset ) )
		return *(int*)(*(unsigned long*)base + offset);
	else
		return 0;
};

void Memory::Pointer::write_int(unsigned long base, unsigned long offset, int value)
{
	if( Memory::Pointer::valid( base, offset ) )
		*(int*)(*(unsigned long*)base + offset) = value;
	return;
};

string Memory::Pointer::read_str(unsigned long base, unsigned long offset)
{
	if( Memory::Pointer::valid( base, offset ) )
		return *(string*)(*(unsigned long*)base + offset);
	else
		return "";
};

void Memory::Pointer::write_str(unsigned long base, unsigned long offset, string value)
{
	if( Memory::Pointer::valid( base, offset ) )
		*(string*)(*(unsigned long*)base + offset) = value;
	return;
};

float Memory::Pointer::read_float(unsigned long base, unsigned long offset)
{
	if( Memory::Pointer::valid( base, offset ) )
		return *(float*)(*(unsigned long*)base + offset);
	else
		return 0;
};

void Memory::Pointer::write_float(unsigned long base, unsigned long offset, float value)
{
	if( Memory::Pointer::valid( base, offset ) )
		*(float*)(*(unsigned long*)base + offset) = value;
	return;
};

double Memory::Pointer::read_double(unsigned long base, unsigned long offset)
{
	if( Memory::Pointer::valid( base, offset ) )
		return *(double*)(*(unsigned long*)base + offset);
	else
		return 0;
};

void Memory::Pointer::write_double(unsigned long base, unsigned long offset, double value)
{
	if( Memory::Pointer::valid( base, offset ) )
		*(double*)(*(unsigned long*)base + offset) = value;
	return;
};
