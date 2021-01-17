/*
Copyright 2021 Chenxi Xu (@xsun2001)

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

#include "Memory/NewSegregatedFitAllocator.h"

#ifdef SGE_WINDOWS
#define AlignedAlloc _mm_malloc
#else
#define AlignedAlloc aligned_alloc
#endif

using namespace SpaceGameEngine;

NewSegregatedFitAllocator::NewSegregatedFitAllocator()
{
	for (int i = 0; i < 8; i++)
	{
		allocatorHeaders[i] = new (AlignedAlloc(PAGE_SIZE, PAGE_SIZE)) BitmapFixedSizeAllocator(i);
	}
}
void* NewSegregatedFitAllocator::Allocate(SizeType size, SizeType alignment)
{
	auto level = GetSizeLevel(size);
	if (level == 8)
	{
		return AlignedAlloc(alignment, size);
	}
	else
	{
		auto allocator = allocatorHeaders[level];
		while (true)
		{
			auto block = allocator->Allocate();
			if (block != nullptr)
				return block;

			allocator->LockNextPage();
			auto next_page = allocator->GetNextPage();
			if (next_page == nullptr)
			{
				next_page = new (AlignedAlloc(PAGE_SIZE, PAGE_SIZE)) BitmapFixedSizeAllocator(level);
				allocator->SetNextPage(next_page);
			}
			allocator->UnlockNextPage();

			allocator = next_page;
		}
	}
}
void NewSegregatedFitAllocator::Free(void* ptr, SizeType size, SizeType alignment)
{
	auto level = GetSizeLevel(size);
	if (level == 8)
	{
		free(ptr);
	}
	else
	{
		auto allocator = allocatorHeaders[level];
		while (!allocator->TryFree(ptr))
		{
			/*!
			 * @todo Is Lock necessary?
			 */
			allocator->LockNextPage();
			auto next_page = allocator->GetNextPage();
			allocator->UnlockNextPage();
			allocator = next_page;
		}
	}
}
UInt8 NewSegregatedFitAllocator::GetSizeLevel(UInt32 size)
{
	if (size <= 8)
		return 0;
	if (size > 64)
		return 8;
	return (size >> 3) - 1;
}
