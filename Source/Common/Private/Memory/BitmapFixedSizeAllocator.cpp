#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "Platform.hpp"

// | next_page | next_page_lock | page_type | remaining | bitmap | blocks |
// |     8     |       1        |     1     |     2     |  bc*4  |  bc*bs | 4096
// sp + si + bmc * si + bc * bs = pagesize;
// bitmap_count = ceil(block_count/32)
// bitmap_count * (si + 32 * bs) = pagesize - sp - si;
// bitmap_count = (pagesize - sp - si) / (si + 32 * bs);
// pagesize = 4096, sp = 8, si = 4, bs = 8
// bitmap_count = 15.707692308 < 16

typedef std::atomic<uint32_t> atomic_uint;

const uint16_t block_count[] = {502, 253, 169, 127, 101, 84, 72, 63};
const uint8_t block_size[] = {8, 16, 24, 32, 40, 48, 56, 64};
const uint8_t bitmap_count[] = {16, 8, 6, 4, 4, 3, 3, 2};
const uint8_t bitmap_offset[] = {10, 3, 23, 1, 27, 12, 24, 1};

void* get_next_page(void* mm_page)
{
	// return mm_page + 0
	return *reinterpret_cast<void**>(mm_page);
}

std::atomic_flag* get_next_page_lock(void* mm_page)
{
	// return mm_page + 8
	return reinterpret_cast<std::atomic_flag*>(reinterpret_cast<uint64_t>(mm_page) | 0x8);
}

uint8_t get_page_type(void* mm_page)
{
	// return mm_page + 9
	return *reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(mm_page) | 0x9);
}

std::atomic<uint16_t>* get_remaining(void* mm_page)
{
	// return mm_page + 10
	return reinterpret_cast<std::atomic<uint16_t>*>(reinterpret_cast<uint64_t>(mm_page) | 0xA);
}

// 1 is free
atomic_uint* get_bitmap(void* mm_page)
{
	// return mm_page + 12
	return reinterpret_cast<atomic_uint*>(reinterpret_cast<uint64_t>(mm_page) | 0xC);
}

void* get_block_base(void* mm_page)
{
	// return mm_page + 12 + bc * 8
	return reinterpret_cast<void*>((reinterpret_cast<uint64_t>(mm_page) | 0xC) + (bitmap_count[get_page_type(mm_page)] << 3));
}

void set_next_page(void* mm_page, void* next_page)
{
	*reinterpret_cast<void**>(mm_page) = next_page;
}

#include <thread>
void lock_next_page(void* mm_page)
{
	while (get_next_page_lock(mm_page)->test_and_set(std::memory_order_acquire))
	{
		std::this_thread::yield();
	}
}

void unlock_next_page(void* mm_page)
{
	get_next_page_lock(mm_page)->clear(std::memory_order_release);
}

void* new_mm_page(uint64_t page_size, uint8_t page_type)
{
	// Allocate memory page
	void* mm_page = aligned_alloc(page_size, page_size);

	// Init next_page next_page_lock page_type remaining
	// Use placement new because atomic_init will be de deprecated in c++20
	*reinterpret_cast<void**>(mm_page) = nullptr;
	new (get_next_page_lock(mm_page)) std::atomic_flag(false);
	*reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(mm_page) | 0x9) = page_type;
	new (get_remaining(mm_page)) std::atomic<uint16_t>(block_count[page_type]);

	// Init bitmap
	atomic_uint* bitmap = get_bitmap(mm_page);
	for (int i = 0; i < bitmap_count[page_type] - 1; i++)
	{
		new (bitmap + i) atomic_uint(0xFFFFFFFF);
	}
	new (bitmap + (bitmap_count[page_type] - 1)) atomic_uint(0xFFFFFFFF >> bitmap_offset[page_type]);

	return mm_page;
}

void* allocate_block(void* mm_page)
{
	const uint8_t page_type = get_page_type(mm_page), bmc = bitmap_count[page_type], bs = block_size[page_type];
	atomic_uint* bitmap = get_bitmap(mm_page);
	uint32_t old_bitmap, new_bitmap, ffs;
	uint64_t block_base = reinterpret_cast<uint64_t>(get_block_base(mm_page)), allocated_block;
	for (int i = 0; i < bmc; i++)
	{
		do
		{
			old_bitmap = bitmap[i].load();
			/*!
			 * @todo use portable ffs function
			 */
			ffs = __builtin_ffs(old_bitmap);
			if (ffs == 0)
				break;
			allocated_block = block_base + (((i << 5) | (ffs - 1)) * bs);
			new_bitmap = old_bitmap & ~(1 << (ffs - 1));
		} while (!bitmap[i].compare_exchange_weak(old_bitmap, new_bitmap));
		if (ffs)
		{
			get_remaining(mm_page)->fetch_sub(1u);
			return reinterpret_cast<void*>(allocated_block);
		}
	}
	return nullptr;
}

void deallocate_block(void* mm_page, void* block)
{
	const uint8_t page_type = get_page_type(mm_page), bs = block_size[page_type];
	const uint16_t block_id = (reinterpret_cast<uint64_t>(block) - reinterpret_cast<uint64_t>(get_block_base(mm_page))) / bs;
	const uint8_t bitmap_idx = block_id >> 5, bitmap_bit = block_id & 31;
	atomic_uint* bitmap = get_bitmap(mm_page) + bitmap_idx;
	unsigned int old_bitmap, new_bitmap;
	do
	{
		old_bitmap = bitmap->load();
		new_bitmap = old_bitmap | (1 << bitmap_bit);
	} while (!bitmap->compare_exchange_weak(old_bitmap, new_bitmap));
	get_remaining(mm_page)->fetch_add(1u);
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