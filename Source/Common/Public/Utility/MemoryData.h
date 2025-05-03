/*
Copyright 2025 creatorlxd

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
#include "CommonAPI.h"
#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#include "MemoryManager.h"
#include <cstring>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	/*!
	@brief MemoryData is a class that manages memory data and it is not copyable but movable.
	@details It is used to encapsulate a pointer to memory and its size, along with a function to free the memory.
	@note You'd better use the `MakeMemoryData`/`NewMemoryData`/`ReferenceMemoryData` function to create a `MemoryData` object.
	*/
	class COMMON_API MemoryData : public Uncopyable
	{
	public:
		using FreeFunction = void (*)(void*, SizeType);

	public:
		MemoryData() = default;
		~MemoryData();

		MemoryData(void* ptr, SizeType size, FreeFunction free_func);

		MemoryData(MemoryData&& md);
		MemoryData& operator=(MemoryData&& md);

		void Clear();

		void* GetData();
		const void* GetData() const;
		SizeType GetSize() const;

	private:
		void RawClear();

	private:
		void* m_pContent = nullptr;
		SizeType m_Size = 0;
		FreeFunction m_pFreeFunction = nullptr;
	};

	/*!
	@brief Create a MemoryData object from a pointer and size.
	@details The function allocates memory using the specified allocator and copies the data from the source pointer to the new memory.
	*/
	template<IsAllocator Allocator = DefaultAllocator>
	inline MemoryData MakeMemoryData(const void* src, SizeType size)
	{
		SGE_ASSERT(NullPointerError, src);
		SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
		auto ptr = Allocator::RawNew(size);
		memcpy(ptr, src, size);
		return MemoryData(ptr, size, [](void* ptr, SizeType size) {
			Allocator::RawDelete(ptr);
		});
	}

	/*!
	@brief Create a MemoryData object from a size.
	@details The function allocates memory using the specified allocator and returns a MemoryData object with the allocated memory.
	*/
	template<IsAllocator Allocator = DefaultAllocator>
	inline MemoryData NewMemoryData(SizeType size)
	{
		SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
		return MemoryData(Allocator::RawNew(size), size, [](void* ptr, SizeType size) {
			Allocator::RawDelete(ptr);
		});
	}

	/*!
	@brief Create a MemoryData object from a pointer and size.
	@details The function creates a MemoryData object that references the specified pointer and size without maintain its lifetime.
	*/
	inline MemoryData ReferenceMemoryData(void* ptr, SizeType size)
	{
		SGE_ASSERT(NullPointerError, ptr);
		SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
		return MemoryData(ptr, size, [](void* ptr, SizeType size) {
			// do nothing
		});
	}
}

/*!
@}
*/