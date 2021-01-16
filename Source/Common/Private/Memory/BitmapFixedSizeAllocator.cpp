#include "Memory/BitmapFixedSizeAllocator.h"

// | next_page | next_page_lock | page_type | remaining | bitmap | blocks |
// |     8     |       1        |     1     |     2     |  bc*4  |  bc*bs | 4096
// sp + si + bmc * si + bc * bs = pagesize;
// bitmap_count = ceil(block_count/32)
// bitmap_count * (si + 32 * bs) = pagesize - sp - si;
// bitmap_count = (pagesize - sp - si) / (si + 32 * bs);
// pagesize = 4096, sp = 8, si = 4, bs = 8
// bitmap_count = 15.707692308 < 16

using namespace SpaceGameEngine;

const UInt16 block_count[] = {502, 253, 169, 127, 101, 84, 72, 63};
const UInt8 block_size[] = {8, 16, 24, 32, 40, 48, 56, 64};
const UInt8 bitmap_count[] = {16, 8, 6, 4, 4, 3, 3, 2};
const UInt8 bitmap_offset[] = {10, 3, 23, 1, 27, 12, 24, 1};

BitmapFixedSizeAllocator* BitmapFixedSizeAllocator::GetNextPage()
{
	// return mm_page + 0
	return *reinterpret_cast<BitmapFixedSizeAllocator**>(mm_page);
}

std::atomic<bool>* BitmapFixedSizeAllocator::GetNextPageLock()
{
	// return mm_page + 8
	return reinterpret_cast<std::atomic<bool>*>(reinterpret_cast<uint64_t>(mm_page) | 0x8);
}

UInt8 BitmapFixedSizeAllocator::GetPageType()
{
	// return mm_page + 9
	return *reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(mm_page) | 0x9);
}

std::atomic<UInt16>* BitmapFixedSizeAllocator::GetRemaining()
{
	// return mm_page + 10
	return reinterpret_cast<std::atomic<uint16_t>*>(reinterpret_cast<uint64_t>(mm_page) | 0xA);
}

// 1 is free
atomic_uint* BitmapFixedSizeAllocator::GetBitmap()
{
	// return mm_page + 12
	return reinterpret_cast<atomic_uint*>(reinterpret_cast<uint64_t>(mm_page) | 0xC);
}

void* BitmapFixedSizeAllocator::GetBlockBase()
{
	// return mm_page + 12 + bmc * 4
	return reinterpret_cast<void*>((reinterpret_cast<uint64_t>(mm_page) | 0xC) + (bitmap_count[GetPageType()] << 2));
}

void BitmapFixedSizeAllocator::SetNextPage(void* next_page)
{
	*reinterpret_cast<void**>(mm_page) = next_page;
}

#ifdef SGE_WINDOWS
#include <Windows.h>
#define PAUSE_THREAD YieldProcessor
#else
#define PAUSE_THREAD __builtin_ia32_pause
#endif

void BitmapFixedSizeAllocator::LockNextPage()
{
	for (;;)
	{
		if (!GetNextPageLock()->exchange(true, std::memory_order_acquire))
		{
			break;
		}
		while (GetNextPageLock()->load(std::memory_order_relaxed))
		{
			PAUSE_THREAD();
		}
	}
}

void BitmapFixedSizeAllocator::UnlockNextPage()
{
	GetNextPageLock()->store(false, std::memory_order_release);
}

BitmapFixedSizeAllocator::BitmapFixedSizeAllocator(UInt8 page_type)
{
	// Init next_page next_page_lock page_type remaining
	// Use placement new because atomic_init will be de deprecated in c++20
	*reinterpret_cast<void**>(mm_page) = nullptr;
	new (GetNextPageLock()) std::atomic<bool>(false);
	*reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(mm_page) | 0x9) = page_type;
	new (GetRemaining()) std::atomic<uint16_t>(block_count[page_type]);

	// Init bitmap
	atomic_uint* bitmap = GetBitmap();
	for (int i = 0; i < bitmap_count[page_type] - 1; i++)
	{
		new (bitmap + i) atomic_uint(0xFFFFFFFF);
	}
	new (bitmap + (bitmap_count[page_type] - 1)) atomic_uint(0xFFFFFFFF >> bitmap_offset[page_type]);
}

BitmapFixedSizeAllocator::~BitmapFixedSizeAllocator()
{
	LockNextPage();
	auto next_page = GetNextPage();
	delete reinterpret_cast<BitmapFixedSizeAllocator*>(next_page);
	SetNextPage(nullptr);
	UnlockNextPage();
}

void* BitmapFixedSizeAllocator::Allocate()
{
	if (GetRemaining()->load(std::memory_order_relaxed) == 0)
	{
		return nullptr;
	}
	const uint8_t page_type = GetPageType(), bmc = bitmap_count[page_type], bs = block_size[page_type];
	atomic_uint* bitmap = GetBitmap();
	uint32_t old_bitmap, new_bitmap;

#ifdef SGE_WINDOWS
	unsigned long ffs;
#else
	uint32_t ffs;
#endif

	uint64_t block_base = reinterpret_cast<uint64_t>(GetBlockBase()), allocated_block;
	for (int i = 0; i < bmc; i++)
	{
		do
		{
			old_bitmap = bitmap[i].load(std::memory_order_relaxed);

#ifdef SGE_WINDOWS
			if (!_BitScanForward(&ffs, old_bitmap))
			{
				break;
			}
#else
			ffs = __builtin_ffs(old_bitmap);
			if (ffs == 0)
			{
				break;
			}
			--ffs;
#endif

			allocated_block = block_base + (((i << 5) | ffs) * bs);
			new_bitmap = old_bitmap & ~(1 << ffs);
			++ffs;
		} while (!bitmap[i].compare_exchange_weak(old_bitmap, new_bitmap));
		if (ffs)
		{
			GetRemaining()->fetch_sub(1u);
			return reinterpret_cast<void*>(allocated_block);
		}
	}
	return nullptr;
}
void BitmapFixedSizeAllocator::Free(void* block)
{
	const uint8_t page_type = GetPageType(), bs = block_size[page_type];
	const uint16_t block_id = (reinterpret_cast<uint64_t>(block) - reinterpret_cast<uint64_t>(GetBlockBase())) / bs;
	const uint8_t bitmap_idx = block_id >> 5, bitmap_bit = block_id & 31;
	atomic_uint* bitmap = GetBitmap() + bitmap_idx;
	unsigned int old_bitmap, new_bitmap;
	do
	{
		old_bitmap = bitmap->load();
		new_bitmap = old_bitmap | (1 << bitmap_bit);
	} while (!bitmap->compare_exchange_weak(old_bitmap, new_bitmap));
	GetRemaining()->fetch_add(1u);
}
bool BitmapFixedSizeAllocator::TryFree(void* ptr)
{
	if ((reinterpret_cast<UInt64>(ptr) >> 12) == (reinterpret_cast<UInt64>(this) >> 12))
	{
		Free(ptr);
		return true;
	}
	return false;
}

#ifdef DEBUG

#include <mutex>
#include <thread>
std::mutex dump_mutex;
void dump_memory_page(void* mm_page)
{
	const std::lock_guard<std::mutex> lock(dump_mutex);
	printf("----------\n");
	printf("Next Header: %p\n", get_next_page(mm_page));
	int page_type = get_page_type(mm_page);
	printf("Page Type: %d\n", page_type);
	printf("Remaining: %d\n", atomic_load(get_remaining(mm_page)));
	printf("Bitmap:");
	atomic_uint* bitmap = get_bitmap(mm_page);
	for (int i = bitmap_count[page_type] - 1; i >= 0; i--)
	{
		if ((bitmap_count[page_type] - 1 - i) % 4 == 0)
			printf("\n");
		printf("%08X ", atomic_load(bitmap + i));
	}
	printf("\nMemory Blocks:\n");
	uint32_t* address = reinterpret_cast<uint32_t*>(get_block_base(mm_page));
	uint32_t* end = reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(mm_page) + sysconf(_SC_PAGESIZE));
	int idx = 0;
	while (address < end)
	{
		printf("%08X ", *address);
		if (++idx == 16)
		{
			idx = 0;
			printf("\n");
		}
		++address;
	}
	printf("\n----------\n");
}

#endif