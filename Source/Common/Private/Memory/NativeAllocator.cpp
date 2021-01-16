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

#include "Error.h"
#include <algorithm>
#include "Memory/AllocatorBase.h"
#include "Memory/NativeAllocator.h"

#ifdef SGE_WINDOWS
#include <malloc.h>
#else
#include <mm_malloc.h>
#include <iostream>
#endif

void* SpaceGameEngine::NativeAllocator::Allocate(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);
	return _mm_malloc(size, alignment == 0 ? GetDefaultAlignment(size) : alignment);
}

void SpaceGameEngine::NativeAllocator::Free(void* ptr, SizeType size, SizeType alignment)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	_mm_free(ptr);
}