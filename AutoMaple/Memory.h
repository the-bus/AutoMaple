#pragma once
#include "inc.h"

#define ROUND_UP(p, align)   (((DWORD)(p) + (align)-1) & ~((align)-1))
#define ROUND_DOWN(p, align) ((DWORD)(p) & ~((align)-1))

class Memory {

public:
	static int __stdcall Write( void* pvAddress, void* pvWriteBuffer, size_t Size );

public:
	static class Pointer {

	private:
		static bool valid(unsigned long base, unsigned long offset);
	public:
		static int read_int(unsigned long base, unsigned long offset);
		static void write_int(unsigned long base, unsigned long offset, int value);

		static string read_str(unsigned long base, unsigned long offset);
		static void write_str(unsigned long base, unsigned long offset, string value);

		static float read_float(unsigned long base, unsigned long offset);
		static void write_float(unsigned long base, unsigned long offset, float value);

		static double read_double(unsigned long base, unsigned long offset);
		static void write_double(unsigned long base, unsigned long offset, double value);
	};
};