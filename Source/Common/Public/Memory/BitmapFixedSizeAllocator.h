#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "TypeDefinition.hpp"
typedef std::atomic<uint32_t> atomic_uint;
namespace SpaceGameEngine
{
	class BitmapFixedSizeAllocator
	{
	public:
		BitmapFixedSizeAllocator(SizeType page_size, UInt8 page_type);
		void* Allocate();
		void Free(void* ptr);
	private:
		void* GetNextPage();
		std::atomic_flag* GetNextPageLock();
		UInt8 GetPageType();
		std::atomic<UInt16>* GetRemaining();
		atomic_uint* GetBitmap();
		void* GetBlockBase();
		void SetNextPage(void* next_page);
		void LockNextPage();
		void UnlockNextPage();

		void* mm_page;
	};
}