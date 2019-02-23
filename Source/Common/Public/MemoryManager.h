/*
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

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef _WIN32
#define SGE_MAX_MEMORY_SIZE UINT32_MAX
#else
#define SGE_MAX_MEMORY_SIZE UINT64_MAX
#endif

	struct StdAllocator
	{
		static void* RawNew(SizeType size);
		static void RawDelete(void* ptr);

		template<typename T,typename... Arg>
		static T* New(Arg&&... arg)
		{
			return new (RawNew(sizeof(T))) T(std::forward<Arg>(arg)...);
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
	@brief make the memory size aligned using the alignment
	*/
#define SGE_MEMORY_ALIGN(size, alignment) (((size) + ((alignment) - 1)) & ~((alignment) - 1))

	/*!
	@brief the memory manager for the engine to use
	*/
	class MemoryManager :public Uncopyable
	{
	public:
		/*!
		@brief the header of a memory block which contain the information of the memory block
		*/
		struct MemoryBlockHeader
		{
			MemoryBlockHeader* m_pNext = nullptr;
		};

		/*!
		@brief the header of a memory page which contain the information of the memory page
		*/
		struct MemoryPageHeader
		{
			/*!
			@brief get the memory address of the first memory block in the memory page whether there is a memory block in the memory page or not
			*/
			MemoryBlockHeader* GetFirstMemoryBlock();

			MemoryPageHeader* m_pNext = nullptr;
		};

		/*!
		@brief the simple allocator that only allocate a sort of memory block
		*/
		class SimpleAllocator :public Uncopyable
		{
		public:
			friend MemoryManager;

			SimpleAllocator();
			~SimpleAllocator();
		private:
			void Set(SizeType data_mem_size, SizeType page_mem_size, SizeType alignment);
			void Clear();

			void* Allocate();
			void Free(void* ptr);
			
			/*!
			@brief get the memory address of the next memory block by giving the memory address of the current memory block
			@note the result is calculated by the current allocator's constant memory block size
			*/
			MemoryBlockHeader* GetNextMemoryBlock(MemoryBlockHeader* ptr);
		private:
			
		};
	public:

	private:

	};

	/*!
	@}
	*/
}