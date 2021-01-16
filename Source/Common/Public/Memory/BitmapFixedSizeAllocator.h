#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#define PAGE_SIZE 4096
typedef std::atomic<uint32_t> atomic_uint;
namespace SpaceGameEngine
{
	class NewSegregatedFitAllocator;
	class BitmapFixedSizeAllocator : public Uncopyable
	{
	public:
		explicit BitmapFixedSizeAllocator(UInt8 page_type);
		~BitmapFixedSizeAllocator();
		void* Allocate();
		void Free(void* ptr);
		bool TryFree(void* ptr);
		friend NewSegregatedFitAllocator;

	private:
		BitmapFixedSizeAllocator* GetNextPage();
		std::atomic<bool>* GetNextPageLock();
		UInt8 GetPageType();
		std::atomic<UInt16>* GetRemaining();
		atomic_uint* GetBitmap();
		void* GetBlockBase();
		void SetNextPage(void* next_page);
		void LockNextPage();
		void UnlockNextPage();

		Byte mm_page[PAGE_SIZE];
	};
}