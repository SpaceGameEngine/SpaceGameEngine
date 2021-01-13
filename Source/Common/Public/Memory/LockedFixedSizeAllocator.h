#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#include "Concurrent/Lock.h"

namespace SpaceGameEngine
{

#ifdef SGE_X86
#define SGE_MAX_MEMORY_SIZE UINT32_MAX
#elif defined(SGE_X64)
#define SGE_MAX_MEMORY_SIZE UINT64_MAX
#endif

	/*!
	@brief make the memory size or memory address aligned using the alignment
	*/
#define SGE_MEMORY_ALIGN(value, alignment) (((value) + ((alignment)-1)) & ~((alignment)-1))

	/*!
	@brief the header of a memory block which contain the information of the memory block
	@note the memory block's information which is stored in MemoryBlockHeader is only used in the allocator,
	so when the memory block is allocated,its header will be unuseful,to avoid waste,the memory of the header will also
	be a part of the memory which is allocated.There is a formula of memory size which need to be allocated:
	`ActualAlllocatedMemorySize=max(MemorySizeWhichUserNeed,sizeof(MemoryBlockHeader))`
	*/
	struct MemoryBlockHeader
	{
		MemoryBlockHeader* m_pNext = nullptr;
	};

	/*!
	@brief the header of a memory page which contain the information of the memory page
	@note Be different with the MemoryBlockHeader,the MemoryPageHeader will not be erased,and it is
	always the header of the memory page for the allocator to use it to manage the memory page.The
	memory blocks will be made of the memory after the MemoryPageHeader.
	*/
	struct MemoryPageHeader
	{
		/*!
		@brief get the memory address of the first memory block in the memory page whether there is
		a memory block in the memory page or not.
		*/
		MemoryBlockHeader* GetFirstMemoryBlock();

		/*!
		@brief the memory offset which is arisen when using aligned allocation.
		*/
		SizeType m_Offset = 0;
		MemoryPageHeader* m_pNext = nullptr;
	};

	class LockedFixedSizeAllocator : public Uncopyable
	{
	public:
		/*!
		@attention the alignment argument can not be -1.
		*/
		explicit LockedFixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment);
		~LockedFixedSizeAllocator();

		void* Allocate();
		void Free(void* ptr);

	private:
		/*!
		@brief get the memory address of the next memory block by giving the memory address of the current memory block
		@note the result is calculated by the current allocator's constant memory block size
		*/
		Mutex mutex;
		MemoryBlockHeader* GetNextMemoryBlock(MemoryBlockHeader* ptr);

	private:
		MemoryBlockHeader* m_pFreeMemoryBlocks;
		SizeType m_FreeMemoryBlockQuantity;

		MemoryPageHeader* m_pMemoryPages;
		SizeType m_MemoryPageQuantity;

		SizeType m_MemoryBlockSize;
		SizeType m_MemoryPageSize;
		SizeType m_Alignment;
	};
}