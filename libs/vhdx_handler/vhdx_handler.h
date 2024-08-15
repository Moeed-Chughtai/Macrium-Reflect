#pragma once


#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>

void CreateVHDX(std::wstring path, ULONGLONG size, ULONG sectorSize);
