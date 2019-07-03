﻿/*
Copyright 2019 creatorlxd

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
#pragma once
#include "TypeDefination.hpp"
#include "Utility.hpp"
#include "Error.h"
#include "Platform.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef SGE_X86
#define SGE_MAX_MEMORY_SIZE UINT32_MAX
#elif defined(SGE_X64)
#define SGE_MAX_MEMORY_SIZE UINT64_MAX
#endif

	struct InvalidAlignmentError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The alignment is invalid");
		/*!
		@note only the alignment which is 0 or 2^n can pass the judgment.
		*/
		static bool Judge(SizeType alignment);
	};

	/*!
	@brief get default alignment by giving memory size
	@note the default alignment depends on the allocated
	memory size,when the size >= 16,the alignment is 16,or it will be 4.
	*/
	SizeType GetDefaultAlignment(SizeType size);

	/*!
	@file
	@todo add Allocator as a concept when c++20 can be used
	*/

	struct StdAllocator
	{
		static void* RawNew(SizeType size, SizeType alignment = 0);
		static void RawDelete(void* ptr, SizeType size);

		template<typename T,typename... Args>
		static T* New(Args&&... arg)
		{
			return new (RawNew(sizeof(T),alignof(T))) T(std::forward<Args>(arg)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			ptr->~T();
			RawDelete(ptr, sizeof(T));
		}
	};

	/*!@todo change the std allocator to my allocator*/
	using DefaultAllocator = StdAllocator;

	/*!
	@brief make the memory size or memory address aligned using the alignment
	*/
#define SGE_MEMORY_ALIGN(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))

	/*!
	@brief the memory manager for the engine to use
	@note The MemoryManager is just a common memory manager using the different allocators which represent
	the different memory allocation strategies.It will choose the proper allocator to manage the memory depend
	on the current condition.
	@todo add interface&implement&unittest
	*/
	class MemoryManager :public Uncopyable, public Singleton<MemoryManager>
	{
	public:
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

		/*!
		@brief the allocator which can only allocate a fixed size memory while the size of memory it
		can allocate must be set by calling FixedSizeAllocator::Init method
		@attention must call FixedSizeAllocator::Init method after instancing before using
		@todo add mutex
		*/
		class FixedSizeAllocator :public Uncopyable
		{
		public:
			/*!
			@attention the alignment argument can not be 0.
			*/
			explicit FixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment);
			~FixedSizeAllocator();

			void* Allocate();
			void Free(void* ptr);
			
		private:
			/*!
			@brief get the memory address of the next memory block by giving the memory address of the current memory block
			@note the result is calculated by the current allocator's constant memory block size
			*/
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
	public:
		friend Singleton<MemoryManager>;

		/*!
		@attention the alignment can not be larger than 128.
		*/
		void* Allocate(SizeType size, SizeType alignment);
		void Free(void* ptr, SizeType size);
	private:
		MemoryManager();
		
		/*!
		@attention the first is size,the second is alignment.
		*/
		using RequestInformation = Pair<SizeType, SizeType>;

		/*!
		@attention the request_info's size must be less than or equal to 2048,
		the alignment of it can not be larger than 128.
		*/
		UInt32 RequestInformationToIndex(const RequestInformation& request_info);
		/*!
		@attention the index can not be larger than 526336.
		*/
		RequestInformation IndexToRequestInformation(UInt32 index);
	private:
		inline static const SizeType sm_MaxFixedSizeAllocatorQuantity = 526337;
		inline static const SizeType sm_MemoryBlockSizes[] = {
			// 4-increments
			4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
			52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,
			// 32-increments
			128, 160, 192, 224, 256, 288, 320, 352, 384,
			416, 448, 480, 512, 544, 576, 608, 640,
			// 64-increments
			704, 768, 832, 896, 960, 1024
		};
		inline static const SizeType sm_MaxMemoryBlockSize = 1024;
		inline static const SizeType sm_MemoryPageSize = 8192;

		FixedSizeAllocator* m_FixedSizeAllocators[sm_MaxFixedSizeAllocatorQuantity];
		SizeType m_MemoryBlockSizeMap[sm_MaxMemoryBlockSize + 1];
	};

	/*!
	@}
	*/
}