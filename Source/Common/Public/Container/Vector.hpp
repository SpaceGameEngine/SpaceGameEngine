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
#include "ContainerConcept.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief Vector is a type of ordered container which is non-thread-safe.
	@note It uses a method called pre-allocating to allocate its memory which
	it needs.If it does not contain any object,it will pre-allocate a memory
	which can contain 4 objects.If you insert a new object,and the memory which
	the Vector needs after the inserting is larger than the memory it holds
	before at the same time,it will re-allocate another memory which as twice
	large as the previous one,and after that,it will copy the content of the
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

		static_assert(IsCopyable<T>::Result, "Vector does not support this type");

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

		inline T& operator[](SizeType index)
		{
			SGE_ASSERT(InvalidSizeError, index, 0, m_Size - 1);
			return GetObject(index);
		}

		inline const T& operator[](SizeType index) const
		{
			SGE_ASSERT(InvalidSizeError, index, 0, m_Size - 1);
			return GetObject(index);
		}

		class Iterator
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const Iterator& iter, T* begin, T* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

		public:
			friend OutOfRangeError;
			friend Vector;

			inline static Iterator GetBegin(Vector& v)
			{
				return v.GetBegin();
			}

			inline static Iterator GetEnd(Vector& v)
			{
				return v.GetEnd();
			}

			inline Iterator(const Iterator& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline Iterator& operator=(const Iterator& iter)
			{
				m_pContent = iter.m_pContent;
				return *this;
			}

			inline Iterator operator+(SizeType i) const
			{
				return Iterator(m_pContent + i);
			}

			inline Iterator& operator+=(SizeType i)
			{
				m_pContent += i;
				return *this;
			}

			inline Iterator operator-(SizeType i) const
			{
				return Iterator(m_pContent - i);
			}

			inline Iterator& operator-=(SizeType i)
			{
				m_pContent -= i;
				return *this;
			}

			inline SizeType operator-(const Iterator& iter) const
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

			inline bool operator==(const Iterator& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const Iterator& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

		private:
			inline explicit Iterator(T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			T* m_pContent;
		};

		class ConstIterator
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const ConstIterator& iter, T* begin, T* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

		public:
			friend OutOfRangeError;
			friend Vector;

			inline static ConstIterator GetBegin(const Vector& v)
			{
				return v.GetConstBegin();
			}

			inline static ConstIterator GetEnd(const Vector& v)
			{
				return v.GetConstEnd();
			}

			inline ConstIterator(const ConstIterator& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline ConstIterator& operator=(const ConstIterator& iter)
			{
				m_pContent = iter.m_pContent;
				return *this;
			}

			inline ConstIterator operator+(SizeType i) const
			{
				return ConstIterator(m_pContent + i);
			}

			inline ConstIterator& operator+=(SizeType i)
			{
				m_pContent += i;
				return *this;
			}

			inline ConstIterator operator-(SizeType i) const
			{
				return ConstIterator(m_pContent - i);
			}

			inline ConstIterator& operator-=(SizeType i)
			{
				m_pContent -= i;
				return *this;
			}

			inline SizeType operator-(const ConstIterator& iter) const
			{
				return ((AddressType)m_pContent - (AddressType)iter.m_pContent) / sizeof(T);
			}

			inline const T* operator->() const
			{
				return m_pContent;
			}

			inline const T& operator*() const
			{
				return *m_pContent;
			}

			inline bool operator==(const ConstIterator& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const ConstIterator& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

		private:
			inline explicit ConstIterator(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			const T* m_pContent;
		};

		inline Iterator GetBegin()
		{
			return Iterator(reinterpret_cast<T*>(m_pContent));
		}

		inline Iterator GetEnd()
		{
			return Iterator(reinterpret_cast<T*>(m_pContent) + m_Size);
		}

		inline ConstIterator GetConstBegin() const
		{
			return ConstIterator(reinterpret_cast<T*>(m_pContent));
		}

		inline ConstIterator GetConstEnd() const
		{
			return ConstIterator(reinterpret_cast<T*>(m_pContent) + m_Size);
		}

		inline T& PushBack(const T& val)
		{
			if (m_Size + 1 <= m_RealSize)
			{
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(val);
				m_Size += 1;
			}
			else
			{
				SetRealSize(2 * m_RealSize);
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(val);
				m_Size += 1;
			}
			return GetObject(m_Size - 1);
		}

		inline T& PushBack(T&& val)
		{
			if (m_Size + 1 <= m_RealSize)
			{
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(std::move(val));
				m_Size += 1;
			}
			else
			{
				SetRealSize(2 * m_RealSize);
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(std::move(val));
				m_Size += 1;
			}
			return GetObject(m_Size - 1);
		}

		template<typename... Args>
		inline T& EmplaceBack(Args&&... args)
		{
			if (m_Size + 1 <= m_RealSize)
			{
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(std::forward<Args>(args)...);
				m_Size += 1;
			}
			else
			{
				SetRealSize(2 * m_RealSize);
				new (reinterpret_cast<T*>(m_pContent) + m_Size) T(std::forward<Args>(args)...);
				m_Size += 1;
			}
			return GetObject(m_Size - 1);
		}
		/*!
		@brief check the type to make sure that it is one of the Vector's Iterator Types.
		@todo use concept
		*/
		template<typename U>
		struct IsVectorIterator
		{
		private:
			template<typename _U>
			inline static constexpr std::enable_if_t<
				IsError<typename _U::OutOfRangeError, const _U&, T*, T*>::Result &&
					std::is_same_v<decltype(_U::GetBegin(*(new Vector))), _U> &&
					std::is_same_v<decltype(_U::GetEnd(*(new Vector))), _U> &&
					std::is_same_v<decltype(new _U(std::declval<_U>())), _U*> &&
					std::is_same_v<decltype(std::declval<_U>() = std::declval<_U>()), _U&> &&
					std::is_same_v<decltype(std::declval<_U>() + std::declval<SizeType>()), _U> &&
					std::is_same_v<decltype(std::declval<_U>() += std::declval<SizeType>()), _U&> &&
					std::is_same_v<decltype(std::declval<_U>() - std::declval<SizeType>()), _U> &&
					std::is_same_v<decltype(std::declval<_U>() -= std::declval<SizeType>()), _U&> &&
					std::is_same_v<decltype(std::declval<_U>() - std::declval<_U>()), SizeType> &&
					(std::is_same_v<decltype(std::declval<_U>().operator->()), T*> || std::is_same_v<decltype(std::declval<_U>().operator->()), const T*>)&&(std::is_same_v<decltype(std::declval<_U>().operator*()), T&> || std::is_same_v<decltype(std::declval<_U>().operator*()), const T&>)&&std::is_same_v<decltype(std::declval<_U>() == std::declval<_U>()), bool> &&
					std::is_same_v<decltype(std::declval<_U>() != std::declval<_U>()), bool>,
				bool>
			Check(int)
			{
				return true;
			}

			template<typename _U>
			inline static constexpr bool Check(...)
			{
				return false;
			}

		public:
			inline static constexpr const bool Result = Check<std::remove_cv_t<U>>(0);
		};

		/*!
		@brief insert a value to the Vector before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Result, bool>, typename = std::enable_if_t<std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>, bool>>
		inline Iterator Insert(const IteratorType& iter, const T& val)
		{
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			SizeType index = iter - IteratorType::GetBegin(*this);
			if (index == m_Size)
			{
				PushBack(val);
				return Iterator(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
			}
			else
			{
				PushBack(std::move(GetObject(m_Size - 1)));
				for (SizeType i = m_Size - 2; i > index; i--)
				{
					GetObject(i) = std::move(GetObject(i - 1));
				}
				GetObject(index) = val;
				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
		}

		/*!
		@brief insert a value to the Vector before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Result, bool>, typename = std::enable_if_t<std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>, bool>>
		inline Iterator Insert(const IteratorType& iter, T&& val)
		{
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			SizeType index = iter - IteratorType::GetBegin(*this);
			if (index == m_Size)
			{
				PushBack(std::move(val));
				return Iterator(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
			}
			else
			{
				PushBack(std::move(GetObject(m_Size - 1)));
				for (SizeType i = m_Size - 2; i > index; i--)
				{
					GetObject(i) = std::move(GetObject(i - 1));
				}
				GetObject(index) = std::move(val);
				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
		}

		/*!
		@brief insert some same values to the Vector before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Result, bool>, typename = std::enable_if_t<std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>, bool>>
		inline Iterator Insert(const IteratorType& iter, SizeType size, const T& val)
		{
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			SGE_ASSERT(InvalidSizeError, m_Size + size, 0, sm_MaxSize);
			SizeType index = iter - IteratorType::GetBegin(*this);
			if (index == m_Size)
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				for (SizeType i = m_Size; i < m_Size + size; i++)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(val);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
			else
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
				}
				for (SizeType i = m_Size - 1; i >= index + size; i--)
				{
					GetObject(i) = std::move(GetObject(i - size));
				}
				for (SizeType i = index; i < std::min(index + size, m_Size); i++)
				{
					GetObject(i) = val;
				}
				for (SizeType i = m_Size; i < index + size; i++)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(val);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
		}

		/*!
		@brief insert some values to the Vector before the iterator by giving iterators.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@note use copy not move to insert elements.
		*/
		template<typename IteratorType, typename AnotherIteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Result, bool>, typename = std::enable_if_t<IsIterator<AnotherIteratorType, T>::Result, bool>, typename = std::enable_if_t<std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>, bool>>
		inline Iterator Insert(const IteratorType& iter, const AnotherIteratorType& begin, const AnotherIteratorType& end)
		{
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			SizeType size = end - begin;
			SGE_ASSERT(InvalidSizeError, m_Size + size, 0, sm_MaxSize);
			SizeType index = iter - IteratorType::GetBegin(*this);
			if (index == m_Size)
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				auto aiter = begin;
				for (SizeType i = m_Size; i < m_Size + size; i++, aiter += 1)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
			else
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
				}
				for (SizeType i = m_Size - 1; i >= index + size; i--)
				{
					GetObject(i) = std::move(GetObject(i - size));
				}
				auto aiter = begin;
				for (SizeType i = index; i < std::min(index + size, m_Size); i++, aiter += 1)
				{
					GetObject(i) = *aiter;
				}
				for (SizeType i = m_Size; i < index + size; i++, aiter += 1)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
		}

		/*!
		@brief insert some values to the Vector before the iterator by giving initializer_list.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@note use copy not move to insert elements.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Result, bool>, typename = std::enable_if_t<std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>, bool>>
		inline Iterator Insert(const IteratorType& iter, std::initializer_list<T> ilist)
		{
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			SizeType size = ilist.size();
			SGE_ASSERT(InvalidSizeError, m_Size + size, 0, sm_MaxSize);
			SizeType index = iter - IteratorType::GetBegin(*this);
			if (index == m_Size)
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				auto aiter = ilist.begin();
				for (SizeType i = m_Size; i < m_Size + size; i++, aiter += 1)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
			else
			{
				if (m_Size + size > m_RealSize)
				{
					SetRealSize(2 * (m_Size + size));
				}

				for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
				}
				for (SizeType i = m_Size - 1; i >= index + size; i--)
				{
					GetObject(i) = std::move(GetObject(i - size));
				}
				auto aiter = ilist.begin();
				for (SizeType i = index; i < std::min(index + size, m_Size); i++, aiter += 1)
				{
					GetObject(i) = *aiter;
				}
				for (SizeType i = m_Size; i < index + size; i++, aiter += 1)
				{
					new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
				}
				m_Size += size;

				return Iterator(reinterpret_cast<T*>(m_pContent) + index);
			}
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