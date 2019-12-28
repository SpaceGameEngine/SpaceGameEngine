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
#include <initializer_list>
#include "TypeDefinition.hpp"
#include "MemoryManager.h"
#include "Error.h"
#include "Iterator.hpp"

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
		@note There are three situations when copy assignment is called.If the Vector's m_Size
		is larger than or equal to another Vector's,the Vector will release the redundant
		objects to fit another Vector's size and call the others' copy assignment by giving
		another Vector's objects.Or if the Vector's m_RealSize is larger than or equal to
		another Vector's size,the Vector will call its old objects' copy assignment and then
		use copy construction to construct new objects to make it be equal to another Vector.
		At last,if the Vector's m_RealSize is less than another Vector's size,the Vector will
		call its objects' destructor,then re-allocate its memory to fit another Vector's size
		and call its new objects' copy constructor using another Vector's objects.
		*/
		inline Vector& operator=(const Vector& v)
		{
			if (m_Size >= v.m_Size)
			{
				for (SizeType i = v.m_Size; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				m_Size = v.m_Size;
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = v.GetObject(i);
				}
			}
			else if (m_RealSize >= v.m_Size)
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = v.GetObject(i);
				}
				SizeType buffer = m_Size;
				m_Size = v.m_Size;
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new (&GetObject(i)) T(v.GetObject(i));
				}
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(v.GetObject(i));
				}
			}
			return *this;
		}
		/*!
		@brief Move assignment of Vector.
		@note The Vector will invoke its objects' destructor,then release its memory
		and just copy another Vector's m_pContent and set it to the nullptr after the
		operation.
		*/
		inline Vector& operator=(Vector&& v)
		{
			for (SizeType i = 0; i < m_Size; i++)
			{
				GetObject(i).~T();
			}
			Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			m_RealSize = v.m_RealSize;
			m_Size = v.m_Size;
			m_pContent = v.m_pContent;
			v.m_pContent = nullptr;
			return *this;
		}

		/*!
		@brief Copy Constructor of Vector which accepts Vector with different Allocators as the argument.
		@note Same as the normal version.
		*/
		template<typename OtherAllocator>
		inline Vector(const Vector<T, OtherAllocator>& v)
		{
			m_RealSize = v.GetRealSize();
			m_Size = v.GetSize();
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (&GetObject(i)) T(v.GetObject(i));
			}
		}
		/*!
		@brief Move Constructor of Vector which accepts Vector with different Allocators as the argument.
		@note Because of the different Allocators,the Vector can not just copy another Vector's m_pContent.
		Instead,the Vector will allocate needed memory and then call the every object's move constructor
		by giving another Vector's objects.
		*/
		template<typename OtherAllocator>
		inline Vector(Vector<T, OtherAllocator>&& v)
		{
			m_RealSize = v.GetRealSize();
			m_Size = v.GetSize();
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (&GetObject(i)) T(std::move(v.GetObject(i)));
			}
		}
		/*!
		@brief Copy assignment of Vector which accepts Vector with different Allocators as the argument.
		@note Same as the normal version.
		*/
		template<typename OtherAllocator>
		inline Vector& operator=(const Vector<T, OtherAllocator>& v)
		{
			if (m_Size >= v.GetSize())
			{
				for (SizeType i = v.GetSize(); i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				m_Size = v.GetSize();
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = v.GetObject(i);
				}
			}
			else if (m_RealSize >= v.GetSize())
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = v.GetObject(i);
				}
				SizeType buffer = m_Size;
				m_Size = v.GetSize();
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new (&GetObject(i)) T(v.GetObject(i));
				}
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.GetRealSize();
				m_Size = v.GetSize();
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(v.GetObject(i));
				}
			}
			return *this;
		}
		/*!
		@brief Move assignment of Vector which accepts Vector with different Allocators as the argument.
		@note Because of the different Allocators,the Vector can not just copy another Vector's m_pContent.
		There are three situations when the move assignment is called.If the Vector's m_Size
		is larger than or equal to another Vector's,the Vector will release the redundant
		objects to fit another Vector's size and call the others' move assignment by giving
		another Vector's objects.Or if the Vector's m_RealSize is larger than or equal to
		another Vector's size,the Vector will call its old objects' move assignment and then
		use move construction to construct new objects to make it be equal to another Vector.
		At last,if the Vector's m_RealSize is less than another Vector's size,the Vector will
		call its objects' destructor,then re-allocate its memory to fit another Vector's size
		and call its new objects' move constructor using another Vector's objects.
		*/
		template<typename OtherAllocator>
		inline Vector& operator=(Vector<T, OtherAllocator>&& v)
		{
			if (m_Size >= v.GetSize())
			{
				for (SizeType i = v.GetSize(); i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				m_Size = v.GetSize();
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = std::move(v.GetObject(i));
				}
			}
			else if (m_RealSize >= v.GetSize())
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i) = std::move(v.GetObject(i));
				}
				SizeType buffer = m_Size;
				m_Size = v.GetSize();
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new (&GetObject(i)) T(std::move(v.GetObject(i)));
				}
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = v.GetRealSize();
				m_Size = v.GetSize();
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(std::move(v.GetObject(i)));
				}
			}
			return *this;
		}

		inline Vector(std::initializer_list<T> list)
		{
			m_Size = list.size();
			m_RealSize = list.size() * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (&GetObject(i)) T(*(list.begin() + i));
			}
		}

		template<typename STLContainer,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<std::decay_t<STLContainer>>().size()), typename std::decay_t<STLContainer>::size_type>, bool>,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<std::decay_t<STLContainer>>().begin()), typename std::decay_t<STLContainer>::iterator>, bool>,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<typename std::decay_t<STLContainer>::iterator>() + 1), typename std::decay_t<STLContainer>::iterator>, bool>>
		inline Vector(STLContainer&& stl_container)
		{
			m_Size = stl_container.size();
			m_RealSize = stl_container.size() * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			if constexpr (std::is_same_v<std::decay_t<STLContainer>, STLContainer>)
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(std::move(*(stl_container.begin() + i)));
				}
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(*(stl_container.begin() + i));
				}
			}
		}

		inline Vector(SizeType size, const T& val)
		{
			SGE_ASSERT(InvalidSizeError, size, 0, sm_MaxSize);
			m_Size = size;
			m_RealSize = size * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new (&GetObject(i)) T(val);
			}
		}

		/*!
		@brief set the real size which the vector allocates.
		@warning the size you set can not be less than the vector's size.
		@note unless the size you set is equal to the vector's previous real size,this method
		will make the vector re-allocate the memory.However,it is not just copying the memory
		simply.The Vector will allocate new memory first and use the old objects
		to construct new objects using move construction,and then call the old objects'
		destructor,finally,release the old memory.
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
				for (SizeType i = 0; i < m_Size; i++)
				{
					new (&GetObject(i)) T(std::move(*reinterpret_cast<T*>((AddressType)(pbuffer) + sizeof(T) * i)));
				}
				for (SizeType i = 0; i < m_Size; i++)
				{
					reinterpret_cast<T*>((AddressType)(pbuffer) + sizeof(T) * i)->~T();
				}
				Allocator::RawDelete(pbuffer, m_RealSize * sizeof(T), alignof(T));
				m_RealSize = size;
			}
		}

		/*!
		@brief clear all elements in the Vector and set size to zero.
		@note also makes real size to the default value.
		*/
		inline void Clear()
		{
			for (SizeType i = 0; i < m_Size; i++)
			{
				GetObject(i).~T();
			}
			Allocator::RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
			m_Size = 0;
			m_RealSize = 4;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
		}

		/*!
		@brief set the Vector's size.
		@param val the default value of the new elements.
		@note There are three situations when the function is called.When
		the size we set is less than the Vector's old size,the Vector
		will release its spare elements.When the size we set is larger than
		the Vector's old size but less than its real size,the Vector will
		use the val argument to construct the new elements by calling T's
		copy constructor.Finally,if the size is larger than the real size,
		it will cause the memory's re-allocation,the old elements will be
		moved to the new location and the new elements will be constructed
		by the val argument.
		*/
		inline void SetSize(SizeType size, const T& val)
		{
			if (size == m_Size)
				return;
			else if (size < m_Size)
			{
				for (SizeType i = size; i < m_Size; i++)
				{
					GetObject(i).~T();
				}
				m_Size = size;
			}
			else if (size <= m_RealSize)
			{
				SizeType buffer = m_Size;
				m_Size = size;
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new (&GetObject(i)) T(val);
				}
			}
			else
			{
				SetRealSize(2 * size);
				SizeType buffer = m_Size;
				m_Size = size;
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new (&GetObject(i)) T(val);
				}
			}
		}

		inline T* GetData()
		{
			return reinterpret_cast<T*>(m_pContent);
		}

		inline const T* GetData() const
		{
			return reinterpret_cast<T*>(m_pContent);
		}

		inline SizeType GetSize() const
		{
			return m_Size;
		}
		inline SizeType GetRealSize() const
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

		/*!
		@brief get the begin iterator of the Vector.
		@note it can just accept the iterator type.If you want to define your own iterator type,
		you need to specialize this template method to make Vector support your iterator type.
		*/
		template<template<template<typename...> class, typename> class IteratorType>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetBegin()
		{
			static_assert(false, "Can not use this type to store iterator");
		}

		/*!
		@brief get the const begin iterator of the Vector.
		@note it can just accept the iterator type.If you want to define your own iterator type,
		you need to specialize this template method to make Vector support your iterator type.
		*/
		template<template<template<typename...> class, typename> class IteratorType>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetBegin() const
		{
			static_assert(false, "Can not use this type to store const iterator");
		}

		/*!
		@brief get the end iterator of the Vector.
		@note it can just accept the iterator type.If you want to define your own iterator type,
		you need to specialize this template method to make Vector support your iterator type.
		*/
		template<template<template<typename...> class, typename> class IteratorType>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetEnd()
		{
			static_assert(false, "Can not use this type to store iterator");
		}

		/*!
		@brief get the const end iterator of the Vector.
		@note it can just accept the iterator type.If you want to define your own iterator type,
		you need to specialize this template method to make Vector support your iterator type.
		*/
		template<template<template<typename...> class, typename> class IteratorType>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetEnd() const
		{
			static_assert(false, "Can not use this type to store const iterator");
		}

	public:
		template<>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetBegin<Iterator>()
		{
			return typename GetIteratorTypeInstance<Iterator, Vector, T>::Result(reinterpret_cast<T*>(m_pContent));
		}

		template<>
		inline typename GetIteratorTypeInstance<Iterator, Vector, T>::Result GetEnd<Iterator>()
		{
			return typename GetIteratorTypeInstance<Iterator, Vector, T>::Result(reinterpret_cast<T*>(m_pContent) + m_Size);
		}

	private:
		void* m_pContent;
		SizeType m_RealSize;
		SizeType m_Size;
	};

	template<typename T>
	class Iterator<Vector, T>
	{
	public:
		struct OutOfRangeError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
			inline static bool Judge(const Iterator<Vector, T>& iter, T* begin, T* end)
			{
				return !(iter.m_pContent >= begin && iter.m_pContent <= end);
			}
		};

	public:
		friend OutOfRangeError;

		template<typename _T, typename Allocator>
		friend class Vector;

		inline Iterator<Vector, T>(const Iterator<Vector, T>& iter)
		{
			m_pContent = iter.m_pContent;
		}

		inline Iterator<Vector, T>& operator=(const Iterator<Vector, T>& iter)
		{
			m_pContent = iter.m_pContent;
			return *this;
		}

		inline Iterator<Vector, T> operator+(SizeType i) const
		{
			return Iterator<Vector, T>(m_pContent + i);
		}

		inline Iterator<Vector, T>& operator+=(SizeType i)
		{
			m_pContent += i;
			return *this;
		}

		inline Iterator<Vector, T> operator-(SizeType i) const
		{
			return Iterator<Vector, T>(m_pContent - i);
		}

		inline Iterator<Vector, T>& operator-=(SizeType i)
		{
			m_pContent -= i;
			return *this;
		}

		inline SizeType operator-(const Iterator<Vector, T>& iter) const
		{
			return ((AddressType)m_pContent - (AddressType)iter.m_pContent) / sizeof(T);
		}

		inline T* operator->() const
		{
			return m_pContent;
		}

		inline T& operator*() const
		{
			return *m_pContent;
		}

		inline bool operator==(const Iterator<Vector, T>& iter) const
		{
			return m_pContent == iter.m_pContent;
		}

		inline bool operator!=(const Iterator<Vector, T>& iter) const
		{
			return m_pContent != iter.m_pContent;
		}

	private:
		inline explicit Iterator<Vector, T>(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			m_pContent = ptr;
		}

	private:
		T* m_pContent;
	};
	/*!
	@}
	*/
}