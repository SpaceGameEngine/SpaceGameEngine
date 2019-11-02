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

	/*!
	@brief Vector is a type of ordered container which is non-thread-safe.
	@note It uses a method called pre-allocating to allocate its memory which
	it need.If it does not contain any object,it will pre-allocate a memory
	which can contain four objects.If you insert a new object,and the memory
	the Vector needs after the inserting is larger than the memory it holds
	before at the same time,it will re-allocate another memory which as twice
	large as the previous one,and after that,it will copy the contents of the
	old memory and paste them into the new one.Finally the old memory will be
	released safely,and the free part of the new memory will be regard as the
	new pre-allocated memory.
	@attention
	1. you'd better not make a pointer which associates to the object in the Vector
	which will be modified and re-allocate its memory frequently.That's because
	the old memory address you get will be invalid after the Vector re-allocating
	its memory.
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
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
		}

		inline ~Vector()
		{
			if (m_pContent)
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					reinterpret_cast<T*>((AddressType)m_pContent + i * sizeof(T))->~T();
				}
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			}
		}

		inline Vector(const Vector& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new ((AddressType)m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>((AddressType)v.m_pContent + i * sizeof(T)));
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
				reinterpret_cast<T*>((AddressType)m_pContent + i * sizeof(T))->~T();
			}

			if (v.m_Size > m_RealSize)
			{
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			}
			else
			{
				m_Size = v.m_Size;
			}
			for (SizeType i = 0; i < m_Size; i++)
			{
				new ((AddressType)m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>((AddressType)v.m_pContent + i * sizeof(T)));
			}
			return *this;
		}
		inline Vector& operator=(Vector&& v)
		{
			Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
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
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new ((AddressType)m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>((AddressType)v.m_pContent + i * sizeof(T)));
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
				reinterpret_cast<T*>((AddressType)m_pContent + i * sizeof(T))->~T();
			}

			if (v.m_Size > m_RealSize)
			{
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			}
			else
			{
				m_Size = v.m_Size;
			}
			for (SizeType i = 0; i < m_Size; i++)
			{
				new ((AddressType)m_pContent + i * sizeof(T)) T(*reinterpret_cast<T*>((AddressType)v.m_pContent + i * sizeof(T)));
			}
			return *this;
		}
		template<typename OtherAllocator>
		inline Vector& operator=(Vector<T, OtherAllocator>&& v)
		{
			//todo
			Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
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
				m_pContent = Allocator::RawNew(size * sizeof(T), alignof(T));
				memcpy(m_pContent, pbuffer, m_Size * sizeof(T));
				Allocator::RawDelete(pbuffer, m_RealSize * sizeof(T), alignof(T));
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