#include "BitmapFixedSizeAllocator.h"
using namespace SpaceGameEngine;
namespace SpaceGameEngine
{
	class NewSegregatedFitAllocator : public Uncopyable
	{
	public:
		explicit NewSegregatedFitAllocator();

		~NewSegregatedFitAllocator() = default;

		/*!
		 * @todo Add alignment support. Currently the allocated block is 8-byte aligned.
		 */
		void* Allocate(SizeType size, SizeType alignment);

		void Free(void* ptr, SizeType size, SizeType alignment);

	private:
		static UInt8 GetSizeLevel(UInt32 size);
		BitmapFixedSizeAllocator* allocatorHeaders[8];
	};
}