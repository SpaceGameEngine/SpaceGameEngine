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
	1. You'd better not make a pointer which associates to the object in the Vector
	which will be modified and re-allocate its memory frequently.That's because
	the old memory address you get will be invalid after the Vector re-allocating
	its memory.
	2. The moving operation may not as fast as you thought when you try to move
	a Vector with a type of Allocator to another Vector with another type of Allocator.
	Under this circumstance,the Vector can not just copy another Vector's m_pContent,
	it will call the T's moving constructor by using the objects in another Vector
	one by one.
	*/
	template<typename T, typename Allocator = DefaultAllocator>
	class Vector
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;

		inline static const constexpr SizeType sm_MaxSize = SGE_MAX_MEMORY_SIZE / sizeof(T);

		/*!
		@brief Default constructor of Vector.
		@note By default,the Vector will pre-allocate a memory which can contain four
		T object.
		*/
		inline Vector()
		{
			m_RealSize = 4;
			m_Size = 0;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
		}

		/*!
		@brief Destructor of Vector.
		@note If the Vector has not been moved to another Vector,its m_pContent will never
		been nullptr,so we need to call the objects' destructors and release the memory.
		*/
		inline ~Vector()
		{
			if (m_pContent)
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			}
		}

		/*!
		@brief Copy constructor of Vector.
		@note The Vector first allocates the needed memory and then calls the every object's
		copy constructor by using another Vector's objects.	
		*/
		inline Vector(const Vector& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (&GetObject(i)) T(v.GetObject(i));
			}
		}
		/*!
		@brief Move constructor of Vector.
		@note Because of using the same type of Allocator,so the Vector can just copy the
		another Vector's m_pContent simply,and then set another Vector's m_pContent to
		nullptr to avoid the twice releases of the memory.
		*/
		inline Vector(Vector&& v)
		{
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
		}
		/*!
		@brief Copy assignment of Vector.
		@note There are two situations when copy assignment is called.If the Vector's m_Size
		is larger than another Vector's,the Vector will release the redundant objects to fit
		another Vector's size and call the others' copy assignment by giving another Vector's
		objects.
		*/
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

		inline T& GetObject(SizeType index)
		{
			SGE_ASSERT(InvalidSizeError, index, 0, m_Size - 1);
			return *reinterpret_cast<T*>((AddressType)(m_pContent) + index * sizeof(T));
		}

		inline const T& GetObject(SizeType index) const
		{
			SGE_ASSERT(InvalidSizeError, index, 0, m_Size - 1);
			return *reinterpret_cast<T*>((AddressType)(m_pContent) + index * sizeof(T));
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