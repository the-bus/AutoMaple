#pragma once
#include "inc.h"

#define ROUND_UP(p, align)   (((DWORD)(p) + (align)-1) & ~((align)-1))
#define ROUND_DOWN(p, align) ((DWORD)(p) & ~((align)-1))

class Memory {

public:
	static int __stdcall Write( void* pvAddress, void* pvWriteBuffer, size_t Size );

};