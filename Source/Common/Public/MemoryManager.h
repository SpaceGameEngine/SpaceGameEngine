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

	/*!
	@file
	@todo add Allocator as a concept when c++20 can be used
	@todo think about whether Allocator need to change the interface to support aligned allocating which using the customed alignment
	*/

	struct StdAllocator
	{
		static void* RawNew(SizeType size);
		static void RawDelete(void* ptr);

		template<typename T,typename... Args>
		static T* New(Args&&... arg)
		{
			return new (RawNew(sizeof(T))) T(std::forward<Args>(arg)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			ptr->~T();
			RawDelete(ptr);
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
	@todo think about whether using Stack Allocator to allocate large size memory or just use new/delete
	@todo rewrite document
	@todo think about how to manage allocators which using different alignments
	*/
	class MemoryManager :public Uncopyable
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
			a memory block in the memory page or not
			*/
			MemoryBlockHeader* GetFirstMemoryBlock();

			MemoryPageHeader* m_pNext = nullptr;
		};

		/*!
		@brief the allocator which can only allocate a fixed size memory while the size of memory it
		can allocate must be set by calling FixedSizeAllocator::Init method
		@attention must call FixedSizeAllocator::Init method after instancing before using
		@todo add implement&unittest
		*/
		class FixedSizeAllocator :public Uncopyable
		{
		public:
			struct FixedSizeAllocatorNotInitializedError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("FixedSizeAllocator has not been initialized");
				static bool Judge(bool is_init);
			};
		public:
			FixedSizeAllocator();
			~FixedSizeAllocator();

			void Init(SizeType data_mem_size, SizeType page_mem_size, SizeType alignment);

			void* Allocate();
			void Free(void* ptr);
			
			/*!
			@brief get the memory address of the next memory block by giving the memory address of the current memory block
			@note the result is calculated by the current allocator's constant memory block size
			*/
			MemoryBlockHeader* GetNextMemoryBlock(MemoryBlockHeader* ptr);
		private:
			bool m_IsInitialized;
		};
	public:

	private:

	};

	/*!
	@}
	*/
}