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
#ifdef SGE_WINDOWS
#include <malloc.h>
#else
#include <mm_malloc.h>
#include <iostream>
#endif
#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#include "Error.h"
#include "Platform.hpp"
#include "Concurrent/Lock.h"

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
		static void RawDelete(void* ptr, SizeType size, SizeType alignment = 0);

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
			RawDelete(ptr, sizeof(T), alignof(T));
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
	@todo add mutexs for fixedsizeallocators
	*/
	class MemoryManager : public Uncopyable, public Singleton<MemoryManager>
	{
	public:


		/*!
		@brief the allocator which can only allocate a fixed size memory while the size of memory it
		can allocate must be set by calling LockedFixedSizeAllocator::Init method
		@attention must call LockedFixedSizeAllocator::Init method after instancing before using
		*/

	public:
		friend Singleton<MemoryManager>;

		~MemoryManager();

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
		inline static const SizeType sm_MemoryPageSize = 8192;

		LockedFixedSizeAllocator* m_FixedSizeAllocators[sm_MaxFixedSizeAllocatorQuantity];
	};

	struct MemoryManagerAllocator
	{
		static void* RawNew(SizeType size, SizeType alignment = 0);
		static void RawDelete(void* ptr, SizeType size, SizeType alignment = 0);

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
			RawDelete(ptr, sizeof(T), alignof(T));
		}
	};

	using DefaultAllocator = MemoryManagerAllocator;

	/*!
	@}
	*/
}