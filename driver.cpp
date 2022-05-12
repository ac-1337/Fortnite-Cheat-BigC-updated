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
#include "driver.h"
HANDLE hDrive;

BOOL Sandy64::Init()
{
	hDrive = ::CreateFileA(XorStr("\\\\.\\Sandy64").c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
	return hDrive != 0;
}
ULONG64 Sandy64::GetModuleBase(ULONG ProcessPid, LPCSTR ModuleName)
{
	READWRITE ReadWrite = { ProcessPid,0,0,(ULONG64)ModuleName };
	BYTE* Temp = new BYTE[8];
	::memset(Temp, 0, 8);
	BOOL bRet = ::DeviceIoControl(hDrive, 0x22200C, &ReadWrite, sizeof(READWRITE), Temp, 8, NULL, NULL);
	ULONG64 temp = 0;
	if (bRet == TRUE)
	{
		memcpy(&temp, Temp, 8);
	}
	delete[] Temp;
	return temp;
}
BOOL Sandy64::ReadPtr(ULONG ProcessPid,ULONG64 Address, PVOID pBuffer, DWORD Size)
{
	READWRITE ReadWrite = { ProcessPid,Address,Size,0 };
	BYTE* Temp = new BYTE[Size];
	memset(Temp, 0, Size);
	BOOL bRet = ::DeviceIoControl(hDrive, 0x222000, &ReadWrite, sizeof(READWRITE), Temp, Size, NULL, NULL);
	if (bRet)
	{
		memcpy(pBuffer, Temp, Size);
	}
	delete[] Temp;
	return bRet;
}
BOOL Sandy64::WritePtr(ULONG ProcessPid,ULONG64 Address, PVOID pBuffer, DWORD Size)
{
	READWRITE ReadWrite = { ProcessPid,Address,Size,(ULONG64)pBuffer };
	BOOL bRet = ::DeviceIoControl(hDrive, 0x222004, &ReadWrite, sizeof(READWRITE), NULL, NULL, NULL, NULL);
	return bRet;
}
ULONG64 Sandy64::ApplyMemory(ULONG ProcessPid, DWORD Size)
{
	READWRITE ReadWrite = { ProcessPid,0,Size,0 };
	BYTE* Temp = new BYTE[8];
	BOOL bRet = ::DeviceIoControl(hDrive, 0x222008, &ReadWrite, sizeof(READWRITE), Temp, 8, NULL, NULL);
	ULONG64 temp = 0;
	if (bRet == TRUE)
	{
		memcpy(&temp, Temp, 8);
	}
	delete[] Temp;
	return temp;
}

BOOL Sandy64::QueryMemory(ULONG ProcessPid, ULONG64 BaseAddress, MEMORY_BASIC_INFORMATIONA &pinfo)
{
	READWRITE ReadWrite = { ProcessPid,BaseAddress,0,(ULONG64)&pinfo.BaseAddress };

	BOOL bRet = ::DeviceIoControl(hDrive, 0x222010, &ReadWrite, sizeof(READWRITE), NULL, NULL, NULL, NULL);
	return bRet;
}
