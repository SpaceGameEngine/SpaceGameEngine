/*
Copyright 2019 Chenxi Xu (@xsun2001)

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

#include "Memory/MemoryManager.h"
#include "Error.h"
#include <algorithm>

void* SpaceGameEngine::StdAllocator::RawNew(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);
	std::iterator
	return _mm_malloc(size, alignment == 0 ? GetDefaultAlignment(size) : alignment);
}

void SpaceGameEngine::StdAllocator::RawDelete(void* ptr, SizeType size, SizeType alignment)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	_mm_free(ptr);
}