/*
Copyright 2026 creatorlxd

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
#include "MemoryManager.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename T, IsAllocator Allocator = DefaultAllocator>
	class HeapObject
	{
	public:
		template<typename... Args>
			requires requires {
				T(std::declval<Args>()...);
			}
		inline HeapObject(Args&&... args)
		{
			m_pContent = Allocator::template New<T>(std::forward<Args>(args)...);
		}

		inline ~HeapObject()
		{
			Allocator::Delete(static_cast<T*>(m_pContent));
		}

		inline HeapObject(const HeapObject& obj)
		{
			m_pContent = Allocator::template New<T>(obj.Get());
		}

		inline HeapObject(HeapObject&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj.Get()));
		}

		inline HeapObject& operator=(const HeapObject& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			Get() = obj.Get();
			return *this;
		}

		inline HeapObject& operator=(HeapObject&& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			Get() = std::move(obj.Get());
			return *this;
		}

		template<IsAllocator OtherAllocator>
		inline HeapObject(const HeapObject<T, OtherAllocator>& obj)
		{
			m_pContent = Allocator::template New<T>(obj.Get());
		}

		template<IsAllocator OtherAllocator>
		inline HeapObject(HeapObject<T, OtherAllocator>&& obj)
		{
			m_pContent = Allocator::template New<T>(std::move(obj.Get()));
		}

		template<IsAllocator OtherAllocator>
		inline HeapObject& operator=(const HeapObject<T, OtherAllocator>& obj)
		{
			Get() = obj.Get();
			return *this;
		}

		template<IsAllocator OtherAllocator>
		inline HeapObject& operator=(HeapObject<T, OtherAllocator>&& obj)
		{
			Get() = std::move(obj.Get());
			return *this;
		}

		inline T& Get()
		{
			SGE_ASSERT(NullPointerError, m_pContent);
			return *static_cast<T*>(m_pContent);
		}

		inline const T& Get() const
		{
			SGE_ASSERT(NullPointerError, m_pContent);
			return *static_cast<const T*>(m_pContent);
		}

		inline operator T&()
		{
			return Get();
		}

		inline operator const T&() const
		{
			return Get();
		}

	private:
		void* m_pContent = nullptr;
	};
}

/*!
@}
*/