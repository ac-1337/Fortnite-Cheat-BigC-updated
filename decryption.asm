#pragma once
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

/*
__int64 __fastcall sub_7FF6119D0ED0(unsigned __int16 *a1, _WORD *a2) //a1 index
{
  _WORD *v2; // rdi
  unsigned __int16 *v3; // rbx
  int v4; // ebx
  unsigned int v5; // eax
  unsigned int v6; // er9
  unsigned int v7; // er8
  __int64 result; // rax
  unsigned int v9; // er9
  int v10; // ecx

  v2 = a2;
  v3 = a1;
  ((void (__fastcall *)(_WORD *, unsigned __int16 *, unsigned __int64))unk_7FF60EA8CC72)(
	a2,
	a1 + 2,
	2 * ((unsigned __int64)*a1 >> 6));
  v4 = *v3 >> 6;
  v5 = sub_7FF60EAA60C0();
  v6 = v5;
  v7 = v5 ^ 0x9C677CC5;
  result = 1041204193 * v5;
  v9 = v6 % 0x21;
  v10 = 0;
  if ( v4 )
  {
	do
	{
	  result = v10++ + v9;
	  v7 += result;
	  *v2 ^= v7;
	  ++v2;
	}
	while ( v10 < v4 );
  }
  return result;
}
*/


static std::string RealGetNameFromFName(int key)
{
    uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
    uint16_t NameOffset = (uint16_t)key;
 
    uint64_t NamePoolChunk = read<uint64_t>(g_pid, g_base_address + 0xC67DF00 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
    uint16_t nameEntry = read<uint16_t>(g_pid, NamePoolChunk);
 
    int nameLength = nameEntry >> 6;
    char buff[1024];
    if ((uint32_t)nameLength)
    {
        for (int x = 0; x < nameLength; ++x)
        {
            buff[x] = read<char>(g_pid, NamePoolChunk + 4 + x);
        }
        char* v2 = buff; // rbx
        unsigned int v4 = nameLength;
        unsigned int v5; // eax
        int v6; // eax
        int v7; // ecx
        unsigned int v8; // kr00_4
        __int64 v9; // ecx
 
        v5 = 0;
        v6 = read<unsigned int>(g_pid, g_base_address + 0xC543318) >> 5;
        if (v4)
        {
            do
            {
                v7 = *v2++;
                v6 += v5 + 96;
                ++v5;
                *(v2 - 1) = (16 * v7) | (v6 ^ (v7 >> 4)) & 0xF;
            } while (v5 < v4);
        }
        buff[nameLength] = '\0';
        return std::string(buff);
    }
    else
    {
        return "";
    }
}
 
static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;
 
	uint64_t NamePoolChunk = read<uint64_t>(g_pid, g_base_address + 0xC67DF00 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (read<uint16_t>(g_pid, NamePoolChunk) < 64)
	{
		auto a1 = read<DWORD>(g_pid, NamePoolChunk + 4);
		return RealGetNameFromFName(a1);
	}
	else
	{
		return RealGetNameFromFName(key);
	}
}
