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