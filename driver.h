/*
	*Gloomy.cc
 	*https://github.com/Chase1803
	
	Copyright (c) 2022 Chase1803
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN  
#pragma once
#include <windows.h>
#include "tlhelp32.h"
#include "string.hpp"

struct READWRITE
{
	DWORD Pid;
	ULONG64 Address;
	DWORD Size;
	ULONG64 Buffer;
};
typedef struct _MEMORY_BASIC_INFORMATIONA
{
	ULONG64  BaseAddress;
	ULONG64  AllocationBase;
	ULONG  AllocationProtect;
	SIZE_T RegionSize;
	ULONG  State;
	ULONG  Protect;
	ULONG  Type;

} MEMORY_BASIC_INFORMATIONA, *PMEMORY_BASIC_INFORMATIONA;
typedef struct _MyMEMORY_BASIC_INFORMATION {
	ULONG64 BaseAddress;
	ULONG64 AllocationBase;
	ULONG64 AllocationProtect;
	ULONG64 RegionSize;
	ULONG64 State;
	ULONG64 Protect;
	ULONG64 Type;
} MyMEMORY_BASIC_INFORMATION, *PMyMEMORY_BASIC_INFORMATION;
typedef struct _MMEMORY_BASIC_INFORMATION {
	ULONG64 BaseAddress;
	ULONG64 AllocationBase;
	ULONG64 AllocationProtect;
	ULONG64 RegionSize;
	ULONG64 State;
	ULONG64 Protect;
	ULONG64 Type;
} MMEMORY_BASIC_INFORMATION, *PMMEMORY_BASIC_INFORMATION;
typedef struct _QueryMemoryInfo
{
	ULONG64 pid;
	ULONG64 BaseAddress;
	MyMEMORY_BASIC_INFORMATION memoryInfo;
}QueryMemoryInfo, *PQueryMemoryInfo;
class Sandy64
{
public:
	BOOL Init();
	template<typename DataType> DataType Read(ULONG64 Address)
	{
		DataType Buffer;
		ReadPtr(Address, &Buffer, sizeof(DataType));
		return Buffer;
	}

	ULONG64 GetModuleBase(ULONG ProcessPid, LPCSTR ModuleName);
	ULONG64 ApplyMemory(ULONG ProcessPid, DWORD Size);
	BOOL ReadPtr(ULONG ProcessPid, ULONG64 Address, PVOID pBuffer, DWORD Size);
	BOOL WritePtr(ULONG ProcessPid, ULONG64 Address, PVOID pBuffer, DWORD Size);
	BOOL QueryMemory(ULONG ProcessPid, ULONG64 BaseAddress, MEMORY_BASIC_INFORMATIONA &pinfo);
};
