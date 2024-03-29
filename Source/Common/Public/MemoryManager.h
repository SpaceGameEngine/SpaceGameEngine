﻿/*
Copyright 2022 creatorlxd

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
#include "TypeDefinition.hpp"
#include "Error.h"
#include "Platform.hpp"
#include "Concurrent/Thread.h"
#include "Concurrent/Lock.h"
#include "Concurrent/Atomic.hpp"
#include "CommonAPI.h"

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
		static COMMON_API bool Judge(SizeType alignment);
	};

	/*!
	@brief get default alignment by giving memory size
	@note the default alignment depends on the allocated
	memory size,when the size >= 16,the alignment is 16,or it will be 4.
	*/
	COMMON_API SizeType GetDefaultAlignment(SizeType size);

	struct COMMON_API StdAllocator
	{
		static void* RawNew(SizeType size, SizeType alignment = 0);
		static void RawDelete(void* ptr);

		template<typename T, typename... Args>
		static T* New(Args&&... args)
		{
			return new (RawNew(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			ptr->~T();
			RawDelete(ptr);
		}
	};

	/*!
	@brief make the memory size or memory address aligned using the alignment
	*/
#define SGE_MEMORY_ALIGN(value, alignment) (((value) + ((alignment)-1)) & ~((alignment)-1))

	/*!
	@brief the memory manager for the engine to use
	@note The MemoryManager is just a common memory manager using the different allocators which represent
	the different memory allocation strategies.It will choose the proper allocator to manage the memory depend
	on the current condition.
	*/
	class COMMON_API MemoryManager : public UncopyableAndUnmovable
	{
	public:
		/*!
		@brief the header of a memory block which contain the information of the memory block
		@note the memory block's information which is stored in MemoryBlockHeader is only used in the allocator,
		so when the memory block is allocated,its header will be unuseful,to avoid waste,the memory of the header will also
		be a part of the memory which is allocated.There is a formula of memory size which need to be allocated:
		`ActualAlllocatedMemorySize=max(MemorySizeWhichUserNeed,sizeof(MemoryBlockHeader))`
		*/
		struct COMMON_API MemoryBlockHeader
		{
			MemoryBlockHeader* m_pNext = nullptr;
		};

		/*!
		@brief the header of a memory page which contain the information of the memory page
		@note Be different with the MemoryBlockHeader,the MemoryPageHeader will not be erased,and it is
		always the header of the memory page for the allocator to use it to manage the memory page.The
		memory blocks will be made of the memory after the MemoryPageHeader.
		*/
		struct COMMON_API MemoryPageHeader
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
		*/
		class COMMON_API FixedSizeAllocator : public UncopyableAndUnmovable
		{
		public:
			/*!
			@attention the alignment argument can not be 0.
			*/
			FixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment);
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

			Mutex m_Mutex;
		};

		/*
		class MultiThreadBufferedFixedSizeAllocator : public FixedSizeAllocator
		{
		public:
			MultiThreadBufferedFixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment);
			~MultiThreadBufferedFixedSizeAllocator();

			void* Allocate();
			void Free(void* ptr);

		private:
			inline static const SizeType sm_PreAllocationBufferQuantity = 2 * (Thread::QueryHardwareConcurrency() != 0 ? Thread::QueryHardwareConcurrency() : 1);
			inline static const SizeType sm_FreeBufferQuantity = 8 * (Thread::QueryHardwareConcurrency() != 0 ? Thread::QueryHardwareConcurrency() : 1);

			OnceFlag m_OnceFlag;
			Atomic<void*>* m_pPreAllocationBuffer;
			Atomic<void*>* m_pFreeBuffer;
		};
		*/

	public:
		~MemoryManager();

		friend struct StdAllocator;

		static MemoryManager& GetSingleton();

		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void* Allocate(SizeType size, SizeType alignment);
		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void Free(void* ptr, SizeType size, SizeType alignment);

	private:
		MemoryManager();

		/*!
		@attention the first is size,the second is alignment.
		*/
		using RequestInformation = Pair<SizeType, SizeType>;

		struct InvalidRequestInformationError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The RequestInformation is invalid");
			/*!
			@note the request_info's size must be less than or equal to 1024,
			the alignment of it can not be larger than 128.
			*/
			static bool Judge(const RequestInformation& request_info);
		};

		/*!
		@attention the request_info's size must be less than or equal to 1024,
		the alignment of it can not be larger than 128.
		*/
		UInt32 RequestInformationToIndex(const RequestInformation& request_info);

	private:
		/*!
		@note 262273=((1024<<8)|128)+1
		*/
		inline static const SizeType sm_MaxFixedSizeAllocatorQuantity = 262273;
		inline static const SizeType sm_MaxMemoryBlockSize = 1024;
		inline static const SizeType sm_MemoryPageSize = 65536;

		FixedSizeAllocator* m_FixedSizeAllocators[sm_MaxFixedSizeAllocatorQuantity];
	};

	struct COMMON_API MemoryManagerAllocator
	{
		static void* RawNew(SizeType size, SizeType alignment = 0);
		static void RawDelete(void* ptr);

		template<typename T, typename... Args>
		static T* New(Args&&... args)
		{
			return new (RawNew(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			ptr->~T();
			RawDelete(ptr);
		}
	};

	using DefaultAllocator = MemoryManagerAllocator;

	/*!
	@}
	*/
}