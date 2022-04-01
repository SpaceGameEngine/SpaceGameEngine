/*
Copyright 2022 creatorlxd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "Utility/Endian.h"
#include "MemoryManager.h"

using namespace SpaceGameEngine;

bool SpaceGameEngine::InvalidEndianError::Judge(Endian endian)
{
	return (UInt8)endian > 1;
}

Endian SpaceGameEngine::GetSystemEndian()
{
	static Endian re = []() {
		UInt16 test_val = 0x07ff;
		if (*reinterpret_cast<UInt8*>(&test_val) == 0xff)
			return Endian::Little;
		else
			return Endian::Big;
	}();
	return re;
}

void SpaceGameEngine::ChangeEndian(void* ptr, SizeType size, Endian dst, Endian src)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidEndianError, dst);
	SGE_ASSERT(InvalidEndianError, src);
	if (dst == src)
		return;
	else
	{
		for (SizeType i = 0; i < size / 2; ++i)
		{
			UInt8 buf = (reinterpret_cast<UInt8*>(ptr))[i];
			(reinterpret_cast<UInt8*>(ptr))[i] = (reinterpret_cast<UInt8*>(ptr))[size - 1 - i];
			(reinterpret_cast<UInt8*>(ptr))[size - 1 - i] = buf;
		}
	}
}
