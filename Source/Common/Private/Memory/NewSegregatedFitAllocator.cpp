#include "Memory/NewSegregatedFitAllocator.h"
NewSegregatedFitAllocator::NewSegregatedFitAllocator()
{
	for (int i = 0; i < 8; i++)
	{
		allocatorHeaders[i] = new (aligned_alloc(PAGE_SIZE, PAGE_SIZE)) BitmapFixedSizeAllocator(i);
	}
}
void* NewSegregatedFitAllocator::Allocate(SizeType size, SizeType alignment)
{
	auto level = GetSizeLevel(size);
	if (level == 8)
	{
		return aligned_alloc(alignment, size);
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
				next_page = new (aligned_alloc(PAGE_SIZE, PAGE_SIZE)) BitmapFixedSizeAllocator(i);
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
	auto level = (size >> 3) - 1;
	return level >= 8 ? 8 : level;
}
