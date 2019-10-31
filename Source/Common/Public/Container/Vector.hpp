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
#include "TypeDefinition.hpp"
#include "MemoryManager.h"
#include "Error.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Allocator = DefaultAllocator>
	class Vector
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;

		inline static const constexpr SizeType sm_MaxSize = SGE_MAX_MEMORY_SIZE / sizeof(T);

		inline Vector()
		{
			m_RealSize = 4;
			m_Size = 0;
			m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
		}

		inline ~Vector()
		{
			if (m_pContent)
				Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
		}

		inline Vector(const Vector& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>(v.m_pContent + i * sizeof(T)));
			}
		}
		inline Vector(Vector&& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
		}
		inline Vector& operator=(const Vector& v)
		{
			for (SizeType i = 0; i < m_Size; i++)
			{
				reinterpret_cast<T*>(m_pContent + i * sizeof(T))->~T();
			}

			if (v.m_Size > m_RealSize)
			{
				Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
			}
			else
			{
				m_Size = v.m_Size;
			}
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>(v.m_pContent + i * sizeof(T)));
			}
			return *this;
		}
		inline Vector& operator=(Vector&& v)
		{
			Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
			return *this;
		}

		template<typename OtherAllocator>
		inline Vector(const Vector<T, OtherAllocator>& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>(v.m_pContent + i * sizeof(T)));
			}
		}
		template<typename OtherAllocator>
		inline Vector(Vector<T, OtherAllocator>&& v)
		{
			//todo
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
		}
		template<typename OtherAllocator>
		inline Vector& operator=(const Vector<T, OtherAllocator>& v)
		{
			for (SizeType i = 0; i < m_Size; i++)
			{
				reinterpret_cast<T*>(m_pContent + i * sizeof(T))->~T();
			}

			if (v.m_Size > m_RealSize)
			{
				Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
			}
			else
			{
				m_Size = v.m_Size;
			}
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>(v.m_pContent + i * sizeof(T)));
			}
			return *this;
		}
		template<typename OtherAllocator>
		inline Vector& operator=(Vector<T, OtherAllocator>&& v)
		{
			//todo
			Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
			return *this;
		}

		/*!
		@brief set the real size which the vector allocates.
		@warning the size you set can not be less than the vector's size.
		@note unless the size you set is equal to the vector's previous real size,this method will make the vector reallocate the memory.
		*/
		inline void SetRealSize(SizeType size)
		{
			SGE_ASSERT(InvalidSizeError, size, m_Size, sm_MaxSize);
			if (m_RealSize == size)
				return;
			else
			{
				auto pbuffer = m_pContent;
				m_pContent = Allocator::template RawNew(size * sizeof(T), alignof(T));
				memcpy(m_pContent, pbuffer, m_Size * sizeof(T));
				Allocator::template RawDelete(pbuffer, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = size;
			}
		}

		inline SizeType GetSize()
		{
			return m_Size;
		}
		inline SizeType GetRealSize()
		{
			return m_RealSize;
		}

	private:
		void* m_pContent;
		SizeType m_RealSize;
		SizeType m_Size;
	};
	/*!
	@}
	*/
}