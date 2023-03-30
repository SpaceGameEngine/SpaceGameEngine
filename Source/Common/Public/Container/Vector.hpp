/*
Copyright 2023 creatorlxd

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
#include <cstring>
#include "TypeDefinition.hpp"
#include "MemoryManager.h"
#include "Error.h"
#include "ContainerConcept.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

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

		struct EmptyVectorError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Vector is empty");
			inline static bool Judge(SizeType size)
			{
				return size == 0;
			}
		};

		template<typename _T, typename _Allocator>
		friend class Vector;

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
				if constexpr (IsTrivial<T>)
				{
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						GetObject(i).~T();
					}
				}
				Allocator::RawDelete(m_pContent);
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
			if constexpr (IsTrivial<T>)
			{
				std::memcpy(m_pContent, v.m_pContent, m_Size * sizeof(T));
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					new ((T*)(m_pContent) + i) T(v.GetObject(i));
				}
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
			SGE_ASSERT(SelfAssignmentError, this, &v);
			if (m_pContent)
			{
				if (m_Size >= v.m_Size)
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.m_Size;
						std::memcpy(m_pContent, v.m_pContent, m_Size * sizeof(T));
					}
					else
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
				}
				else if (m_RealSize >= v.m_Size)
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.m_Size;
						std::memcpy(m_pContent, v.m_pContent, m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i) = v.GetObject(i);
						}
						SizeType buffer = m_Size;
						m_Size = v.m_Size;
						for (SizeType i = buffer; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(v.GetObject(i));
						}
					}
				}
				else
				{
					if constexpr (IsTrivial<T>)
					{
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.m_RealSize;
						m_Size = v.m_Size;
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						std::memcpy(m_pContent, v.m_pContent, m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i).~T();
						}
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.m_RealSize;
						m_Size = v.m_Size;
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						for (SizeType i = 0; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(v.GetObject(i));
						}
					}
				}
			}
			else
			{
				m_RealSize = v.m_RealSize;
				m_Size = v.m_Size;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
				if constexpr (IsTrivial<T>)
				{
					std::memcpy(m_pContent, v.m_pContent, m_Size * sizeof(T));
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						new ((T*)(m_pContent) + i) T(v.GetObject(i));
					}
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
			SGE_ASSERT(SelfAssignmentError, this, &v);
			if (m_pContent)
			{
				if constexpr (IsTrivial<T>)
				{
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						GetObject(i).~T();
					}
				}

				Allocator::RawDelete(m_pContent);
			}

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

			if constexpr (IsTrivial<T>)
			{
				std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					new ((T*)(m_pContent) + i) T(v.GetObject(i));
				}
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

			if constexpr (IsTrivial<T>)
			{
				std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
			}
			else
			{
				for (SizeType i = 0; i < m_Size; i++)
				{
					new ((T*)(m_pContent) + i) T(std::move(v.GetObject(i)));
				}
			}
		}
		/*!
		@brief Copy assignment of Vector which accepts Vector with different Allocators as the argument.
		@note Same as the normal version.
		*/
		template<typename OtherAllocator>
		inline Vector& operator=(const Vector<T, OtherAllocator>& v)
		{
			if (m_pContent)
			{
				if (m_Size >= v.GetSize())
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.GetSize();
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
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
				}
				else if (m_RealSize >= v.GetSize())
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.GetSize();
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i) = v.GetObject(i);
						}
						SizeType buffer = m_Size;
						m_Size = v.GetSize();
						for (SizeType i = buffer; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(v.GetObject(i));
						}
					}
				}
				else
				{
					if constexpr (IsTrivial<T>)
					{
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.GetRealSize();
						m_Size = v.GetSize();
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i).~T();
						}
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.GetRealSize();
						m_Size = v.GetSize();
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						for (SizeType i = 0; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(v.GetObject(i));
						}
					}
				}
			}
			else
			{
				m_RealSize = v.GetRealSize();
				m_Size = v.GetSize();
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));

				if constexpr (IsTrivial<T>)
				{
					std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						new ((T*)(m_pContent) + i) T(v.GetObject(i));
					}
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
			if (m_pContent)
			{
				if (m_Size >= v.GetSize())
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.GetSize();
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
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
				}
				else if (m_RealSize >= v.GetSize())
				{
					if constexpr (IsTrivial<T>)
					{
						m_Size = v.GetSize();
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i) = std::move(v.GetObject(i));
						}
						SizeType buffer = m_Size;
						m_Size = v.GetSize();
						for (SizeType i = buffer; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(std::move(v.GetObject(i)));
						}
					}
				}
				else
				{
					if constexpr (IsTrivial<T>)
					{
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.GetRealSize();
						m_Size = v.GetSize();
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
					}
					else
					{
						for (SizeType i = 0; i < m_Size; i++)
						{
							GetObject(i).~T();
						}
						Allocator::RawDelete(m_pContent);
						m_RealSize = v.GetRealSize();
						m_Size = v.GetSize();
						m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
						for (SizeType i = 0; i < m_Size; i++)
						{
							new ((T*)(m_pContent) + i) T(std::move(v.GetObject(i)));
						}
					}
				}
			}
			else
			{
				m_RealSize = v.GetRealSize();
				m_Size = v.GetSize();
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));

				if constexpr (IsTrivial<T>)
				{
					std::memcpy(m_pContent, v.GetData(), m_Size * sizeof(T));
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						new ((T*)(m_pContent) + i) T(std::move(v.GetObject(i)));
					}
				}
			}
			return *this;
		}

		inline Vector(std::initializer_list<T> ilist)
		{
			SGE_ASSERT(InvalidValueError, ilist.size(), 0, sm_MaxSize);

			if (ilist.size())
			{
				m_Size = ilist.size();
				m_RealSize = ilist.size() * 2;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
				if constexpr (IsTrivial<T>)
				{
					std::memcpy(m_pContent, ilist.begin(), m_Size * sizeof(T));
				}
				else
				{
					auto iter = ilist.begin();
					for (SizeType i = 0; i < m_Size; i++, ++iter)
					{
						new ((T*)(m_pContent) + i) T(*iter);
					}
				}
			}
			else
			{
				m_RealSize = 4;
				m_Size = 0;
				m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			}
		}

		template<typename STLContainer,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<std::remove_cv_t<std::remove_reference_t<STLContainer>>>().size()), typename std::remove_cv_t<std::remove_reference_t<STLContainer>>::size_type>, bool>,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<std::remove_cv_t<std::remove_reference_t<STLContainer>>>().begin()), typename std::remove_cv_t<std::remove_reference_t<STLContainer>>::iterator>, bool>,
				 typename = std::enable_if_t<std::is_same_v<decltype(std::declval<typename std::remove_cv_t<std::remove_reference_t<STLContainer>>::iterator>() + 1), typename std::remove_cv_t<std::remove_reference_t<STLContainer>>::iterator>, bool>>
		inline Vector(STLContainer&& stl_container)
		{
			m_Size = stl_container.size();
			m_RealSize = stl_container.size() * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<STLContainer>>, STLContainer>)
			{
				auto iter = stl_container.begin();
				for (SizeType i = 0; i < m_Size && iter != stl_container.end(); i++, iter++)
				{
					new ((T*)(m_pContent) + i) T(std::move(*iter));
				}
			}
			else
			{
				auto iter = stl_container.begin();
				for (SizeType i = 0; i < m_Size && iter != stl_container.end(); i++, iter++)
				{
					new ((T*)(m_pContent) + i) T(*iter);
				}
			}
		}

		inline Vector(SizeType size, const T& val)
		{
			SGE_ASSERT(InvalidValueError, size, 0, sm_MaxSize);
			m_Size = size;
			m_RealSize = size * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			for (SizeType i = 0; i < m_Size; i++)
			{
				new ((T*)(m_pContent) + i) T(val);
			}
		}

		template<typename IteratorType, typename = std::enable_if_t<IsSequentialIterator<IteratorType>, void>>
		inline Vector(const IteratorType& begin, const IteratorType& end)
		{
			SizeType size = end - begin;
			SGE_ASSERT(InvalidValueError, size, 0, sm_MaxSize);
			m_Size = size;
			m_RealSize = size * 2;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
			auto iter = begin;
			for (SizeType i = 0; i < m_Size && iter != end; i++, iter++)
			{
				new ((T*)(m_pContent) + i) T(*iter);
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
			SGE_ASSERT(InvalidValueError, size, m_Size, sm_MaxSize);
			if (m_RealSize == size)
				return;
			else
			{
				auto pbuffer = m_pContent;
				m_pContent = Allocator::RawNew(size * sizeof(T), alignof(T));
				if constexpr (IsTrivial<T>)
				{
					std::memcpy(m_pContent, pbuffer, m_Size * sizeof(T));
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						new ((T*)(m_pContent) + i) T(std::move(*reinterpret_cast<T*>((AddressType)(pbuffer) + sizeof(T) * i)));
					}
					for (SizeType i = 0; i < m_Size; i++)
					{
						reinterpret_cast<T*>((AddressType)(pbuffer) + sizeof(T) * i)->~T();
					}
				}
				Allocator::RawDelete(pbuffer);
				m_RealSize = size;
			}
		}

		/*!
		@brief clear all elements in the Vector and set size to zero.
		@note also makes real size to the default value.
		*/
		inline void Clear()
		{
			if (m_pContent)
			{
				if constexpr (IsTrivial<T>)
				{
				}
				else
				{
					for (SizeType i = 0; i < m_Size; i++)
					{
						GetObject(i).~T();
					}
				}
				Allocator::RawDelete(m_pContent);
			}

			m_Size = 0;
			m_RealSize = 4;
			m_pContent = Allocator::RawNew(m_RealSize * sizeof(T), alignof(T));
		}

		/*!
		@brief set the Vector's size.
		@param val the default value of the new elements.
		@warning the value can not be one of the elements of this Vector.
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
				if constexpr (IsTrivial<T>)
				{
				}
				else
				{
					for (SizeType i = size; i < m_Size; i++)
					{
						GetObject(i).~T();
					}
				}
				m_Size = size;
			}
			else if (size <= m_RealSize)
			{
				SizeType buffer = m_Size;
				m_Size = size;
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new ((T*)(m_pContent) + i) T(val);
				}
			}
			else
			{
				SetRealSize(2 * size);
				SizeType buffer = m_Size;
				m_Size = size;
				for (SizeType i = buffer; i < m_Size; i++)
				{
					new ((T*)(m_pContent) + i) T(val);
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
			SGE_ASSERT(InvalidValueError, index, 0, m_Size - 1);
			return *reinterpret_cast<T*>((AddressType)(m_pContent) + index * sizeof(T));
		}

		inline const T& GetObject(SizeType index) const
		{
			SGE_ASSERT(InvalidValueError, index, 0, m_Size - 1);
			return *reinterpret_cast<const T*>((AddressType)(m_pContent) + index * sizeof(T));
		}

		inline T& operator[](SizeType index)
		{
			SGE_ASSERT(InvalidValueError, index, 0, m_Size - 1);
			return GetObject(index);
		}

		inline const T& operator[](SizeType index) const
		{
			SGE_ASSERT(InvalidValueError, index, 0, m_Size - 1);
			return GetObject(index);
		}

		template<typename IteratorType>
		struct IsVectorIterator;

		template<typename _T>
		class IteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter, _T* begin, _T* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = _T;

			friend OutOfRangeError;
			friend Vector;
			template<typename __T>
			friend class ReverseIteratorImpl;

		public:
			inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const Vector&, Vector&> v)
			{
				return IteratorImpl(reinterpret_cast<_T*>(v.m_pContent));
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const Vector&, Vector&> v)
			{
				return IteratorImpl(reinterpret_cast<_T*>(v.m_pContent) + v.m_Size);
			}

			inline IteratorImpl(const IteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline IteratorImpl& operator=(const IteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl(const IteratorType& iter)
			{
				m_pContent = (_T*)iter.GetData();
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (_T*)iter.GetData();
				return *this;
			}

			inline IteratorImpl operator+(SizeType i) const
			{
				return IteratorImpl(m_pContent + i);
			}

			inline IteratorImpl& operator+=(SizeType i)
			{
				m_pContent += i;
				return *this;
			}

			inline IteratorImpl& operator++()
			{
				m_pContent += 1;
				return *this;
			}

			inline const IteratorImpl operator++(int)
			{
				IteratorImpl re(*this);
				m_pContent += 1;
				return re;
			}

			inline IteratorImpl operator-(SizeType i) const
			{
				return IteratorImpl(m_pContent - i);
			}

			inline IteratorImpl& operator-=(SizeType i)
			{
				m_pContent -= i;
				return *this;
			}

			inline IteratorImpl& operator--()
			{
				m_pContent -= 1;
				return *this;
			}

			inline const IteratorImpl operator--(int)
			{
				IteratorImpl re(*this);
				m_pContent -= 1;
				return re;
			}

			inline SizeType operator-(const IteratorImpl& iter) const
			{
				return ((AddressType)m_pContent - (AddressType)iter.m_pContent) / sizeof(_T);
			}

			inline _T* operator->() const
			{
				return m_pContent;
			}

			inline _T& operator*() const
			{
				return *m_pContent;
			}

			inline bool operator==(const IteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const IteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			inline ValueType* GetData() const
			{
				return m_pContent;
			}

		private:
			inline explicit IteratorImpl(_T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			_T* m_pContent;
		};

		template<typename _T>
		class ReverseIteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter, _T* begin, _T* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = _T;

			friend OutOfRangeError;
			friend Vector;
			template<typename __T>
			friend class IteratorImpl;

		public:
			inline static ReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const Vector&, Vector&> v)
			{
				return ReverseIteratorImpl(reinterpret_cast<_T*>(v.m_pContent) + v.m_Size - 1);
			}

			inline static ReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const Vector&, Vector&> v)
			{
				return ReverseIteratorImpl(reinterpret_cast<_T*>(v.m_pContent) - 1);
			}

			inline ReverseIteratorImpl(const ReverseIteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline ReverseIteratorImpl& operator=(const ReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl(const IteratorType& iter)
			{
				m_pContent = (_T*)iter.GetData();
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (_T*)iter.GetData();
				return *this;
			}

			inline ReverseIteratorImpl operator+(SizeType i) const
			{
				return ReverseIteratorImpl(m_pContent - i);
			}

			inline ReverseIteratorImpl& operator+=(SizeType i)
			{
				m_pContent -= i;
				return *this;
			}

			inline ReverseIteratorImpl& operator++()
			{
				m_pContent -= 1;
				return *this;
			}

			inline const ReverseIteratorImpl operator++(int)
			{
				ReverseIteratorImpl re(*this);
				m_pContent -= 1;
				return re;
			}

			inline ReverseIteratorImpl operator-(SizeType i) const
			{
				return ReverseIteratorImpl(m_pContent + i);
			}

			inline ReverseIteratorImpl& operator-=(SizeType i)
			{
				m_pContent += i;
				return *this;
			}

			inline ReverseIteratorImpl& operator--()
			{
				m_pContent += 1;
				return *this;
			}

			inline const ReverseIteratorImpl operator--(int)
			{
				ReverseIteratorImpl re(*this);
				m_pContent += 1;
				return re;
			}

			inline SizeType operator-(const ReverseIteratorImpl& iter) const
			{
				return ((AddressType)iter.m_pContent - (AddressType)m_pContent) / sizeof(_T);
			}

			inline _T* operator->() const
			{
				return m_pContent;
			}

			inline _T& operator*() const
			{
				return *m_pContent;
			}

			inline bool operator==(const ReverseIteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const ReverseIteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			inline ValueType* GetData() const
			{
				return m_pContent;
			}

		private:
			inline explicit ReverseIteratorImpl(_T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			_T* m_pContent;
		};

		using Iterator = IteratorImpl<T>;
		using ConstIterator = IteratorImpl<const T>;
		using ReverseIterator = ReverseIteratorImpl<T>;
		using ConstReverseIterator = ReverseIteratorImpl<const T>;

		inline Iterator GetBegin()
		{
			return Iterator::GetBegin(*this);
		}

		inline Iterator GetEnd()
		{
			return Iterator::GetEnd(*this);
		}

		inline ConstIterator GetConstBegin() const
		{
			return ConstIterator::GetBegin(*this);
		}

		inline ConstIterator GetConstEnd() const
		{
			return ConstIterator::GetEnd(*this);
		}

		inline ReverseIterator GetReverseBegin()
		{
			return ReverseIterator::GetBegin(*this);
		}

		inline ReverseIterator GetReverseEnd()
		{
			return ReverseIterator::GetEnd(*this);
		}

		inline ConstReverseIterator GetConstReverseBegin() const
		{
			return ConstReverseIterator::GetBegin(*this);
		}

		inline ConstReverseIterator GetConstReverseEnd() const
		{
			return ConstReverseIterator::GetEnd(*this);
		}

		/*!
		@warning the value can not be one of the elements of this Vector.
		*/
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

		/*!
		@warning the value can not be one of the elements of this Vector.
		*/
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

		/*!
		@warning the value can not be one of the elements of this Vector.
		*/
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
		*/
		template<typename IteratorType>
		struct IsVectorIterator
		{
			inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator> || std::is_same_v<IteratorType, ReverseIterator> || std::is_same_v<IteratorType, ConstReverseIterator>;
		};

		/*!
		@brief insert a value to the Vector before the iterator.
		@warning only support sequential iterator.
		@warning the value can not be one of the elements of this Vector.
		@return Iterator pointing to the inserted value.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const T& val)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == m_Size)
				{
					PushBack(val);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + index + 1, (T*)(m_pContent) + index, (m_Size - 2 - index) * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size - 2; i > index; i--)
						{
							GetObject(i) = std::move(GetObject(i - 1));
						}
					}
					GetObject(index) = val;
					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == 0)
				{
					PushBack(val);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + m_Size - index, (T*)(m_pContent) + m_Size - index - 1, (index - 1) * sizeof(T));
					}
					else
					{
						for (SizeType i = 1; i < index; i++)
						{
							GetObject(m_Size - 1 - i) = std::move(GetObject(m_Size - 2 - i));
						}
					}
					GetObject(m_Size - 1 - index) = val;
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1 - index);
				}
			}
		}

		/*!
		@brief insert a value to the Vector before the iterator.
		@warning only support sequential iterator.
		@warning the value can not be one of the elements of this Vector.
		@return Iterator pointing to the inserted value.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, T&& val)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == m_Size)
				{
					PushBack(std::move(val));
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + index + 1, (T*)(m_pContent) + index, (m_Size - 2 - index) * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size - 2; i > index; i--)
						{
							GetObject(i) = std::move(GetObject(i - 1));
						}
					}
					GetObject(index) = std::move(val);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == 0)
				{
					PushBack(std::move(val));
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + m_Size - index, (T*)(m_pContent) + m_Size - index - 1, (index - 1) * sizeof(T));
					}
					else
					{
						for (SizeType i = 1; i < index; i++)
						{
							GetObject(m_Size - 1 - i) = std::move(GetObject(m_Size - 2 - i));
						}
					}
					GetObject(m_Size - 1 - index) = std::move(val);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1 - index);
				}
			}
		}

		/*!
		@warning the value can not be one of the elements of this Vector.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>, typename... Args>
		inline IteratorType Emplace(const IteratorType& iter, Args&&... args)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == m_Size)
				{
					EmplaceBack(std::forward<Args>(args)...);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + index + 1, (T*)(m_pContent) + index, (m_Size - 2 - index) * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size - 2; i > index; i--)
						{
							GetObject(i) = std::move(GetObject(i - 1));
						}
					}
					GetObject(index) = T(std::forward<Args>(args)...);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == 0)
				{
					EmplaceBack(std::forward<Args>(args)...);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					T tbuf(std::move(GetObject(m_Size - 1)));
					PushBack(std::move(tbuf));
					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + m_Size - index, (T*)(m_pContent) + m_Size - index - 1, (index - 1) * sizeof(T));
					}
					else
					{
						for (SizeType i = 1; i < index; i++)
						{
							GetObject(m_Size - 1 - i) = std::move(GetObject(m_Size - 2 - i));
						}
					}
					GetObject(m_Size - 1 - index) = T(std::forward<Args>(args)...);
					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1 - index);
				}
			}
		}

		/*!
		@brief insert some same values to the Vector before the iterator.
		@warning only support sequential iterator.
		@warning the size can not be zero.
		@warning the value can not be one of the elements of this Vector.
		@return Iterator pointing to the first inserted value.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, SizeType size, const T& val)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SGE_ASSERT(InvalidValueError, m_Size + size, m_Size + 1, sm_MaxSize);
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

					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
				else
				{
					if (m_Size + size > m_RealSize)
					{
						SetRealSize(2 * (m_Size + size));
					}

					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + index + size, (T*)(m_pContent) + index, (m_Size - index) * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
						}
						for (SizeType i = m_Size - 1; i >= index + size; i--)
						{
							GetObject(i) = std::move(GetObject(i - size));
						}
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

					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SGE_ASSERT(InvalidValueError, m_Size + size, m_Size + 1, sm_MaxSize);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == 0)
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

					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					if (m_Size + size > m_RealSize)
					{
						SetRealSize(2 * (m_Size + size));
					}

					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + m_Size - index + size, (T*)(m_pContent) + m_Size - index, index * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, m_Size - index + size); i--)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
						}
						for (SizeType i = m_Size - 1; i >= m_Size - index + size; i--)
						{
							GetObject(i) = std::move(GetObject(i - size));
						}
					}
					for (SizeType i = m_Size - index; i < std::min(m_Size - index + size, m_Size); i++)
					{
						GetObject(i) = val;
					}
					for (SizeType i = m_Size; i < m_Size - index + size; i++)
					{
						new (reinterpret_cast<T*>(m_pContent) + i) T(val);
					}
					m_Size += size;

					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - index - 1);
				}
			}
		}

		/*!
		@brief insert some values to the Vector before the iterator by giving iterators.
		@warning only support sequential iterator.
		@warning this function can not check whether the given two arguments(begin&end) are out of their range or not.
		@warning the begin&end's Container can not be the current Vector.
		@warning the (end-begin) can not be zero.
		@return Iterator pointing to the first inserted value.
		@note use copy not move to insert elements.
		*/
		template<typename IteratorType, typename AnotherIteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>, typename = std::enable_if_t<IsSequentialIterator<AnotherIteratorType>, bool>>
		inline IteratorType Insert(const IteratorType& iter, const AnotherIteratorType& begin, const AnotherIteratorType& end)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SizeType size = end - begin;
				SGE_ASSERT(InvalidValueError, m_Size + size, m_Size + 1, sm_MaxSize);
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

					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
				else
				{
					if (m_Size + size > m_RealSize)
					{
						SetRealSize(2 * (m_Size + size));
					}

					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + index + size, (T*)(m_pContent) + index, (m_Size - index) * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
						}
						for (SizeType i = m_Size - 1; i >= index + size; i--)
						{
							GetObject(i) = std::move(GetObject(i - size));
						}
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

					return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
				}
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SizeType size = end - begin;
				SGE_ASSERT(InvalidValueError, m_Size + size, m_Size + 1, sm_MaxSize);
				SizeType index = iter - IteratorType::GetBegin(*this);
				if (index == 0)
				{
					if (m_Size + size > m_RealSize)
					{
						SetRealSize(2 * (m_Size + size));
					}

					auto aiter = begin;
					for (SizeType i = m_Size + size - 1; i >= m_Size; i--, aiter += 1)
					{
						new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
						if (i == 0)
							break;
					}
					m_Size += size;

					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				}
				else
				{
					if (m_Size + size > m_RealSize)
					{
						SetRealSize(2 * (m_Size + size));
					}

					if constexpr (IsTrivial<T>)
					{
						memmove((T*)(m_pContent) + m_Size - index + size, (T*)(m_pContent) + m_Size - index, index * sizeof(T));
					}
					else
					{
						for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, m_Size - index + size); i--)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
						}
						for (SizeType i = m_Size - 1; i >= m_Size - index + size; i--)
						{
							GetObject(i) = std::move(GetObject(i - size));
						}
					}
					auto aiter = begin;
					for (SizeType i = m_Size - index + size - 1; i >= m_Size; i--, aiter += 1)
					{
						new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
						if (i == 0)
							break;
					}
					for (SizeType i = std::min(m_Size - index + size, m_Size) - 1; i >= m_Size - index; i--, aiter += 1)
					{
						GetObject(i) = *aiter;
						if (i == 0)
							break;
					}
					m_Size += size;

					return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - index - 1);
				}
			}
		}

		/*!
		@brief insert some values to the Vector before the iterator by giving initializer_list.
		@warning only support sequential iterator.
		@warning the size of initializer_list can not be zero.
		@warning the value can not be one of the elements of this Vector.
		@return Iterator pointing to the first inserted value.
		@note use copy not move to insert elements.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, std::initializer_list<T> ilist)
		{
			SizeType size = ilist.size();
			SGE_ASSERT(InvalidValueError, m_Size + size, m_Size, sm_MaxSize);

			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				if (size)
				{
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

						return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
					}
					else
					{
						if (m_Size + size > m_RealSize)
						{
							SetRealSize(2 * (m_Size + size));
						}

						if constexpr (IsTrivial<T>)
						{
							memmove((T*)(m_pContent) + index + size, (T*)(m_pContent) + index, (m_Size - index) * sizeof(T));
						}
						else
						{
							for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, index + size); i--)
							{
								new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
							}
							for (SizeType i = m_Size - 1; i >= index + size; i--)
							{
								GetObject(i) = std::move(GetObject(i - size));
							}
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

						return IteratorType(reinterpret_cast<T*>(m_pContent) + index);
					}
				}
				else
					return iter;
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				if (size)
				{
					SizeType index = iter - IteratorType::GetBegin(*this);
					if (index == 0)
					{
						if (m_Size + size > m_RealSize)
						{
							SetRealSize(2 * (m_Size + size));
						}

						auto aiter = ilist.begin();
						for (SizeType i = m_Size + size - 1; i >= m_Size; i--, aiter += 1)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
							if (i == 0)
								break;
						}
						m_Size += size;

						return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - 1);
					}
					else
					{
						if (m_Size + size > m_RealSize)
						{
							SetRealSize(2 * (m_Size + size));
						}

						if constexpr (IsTrivial<T>)
						{
							memmove((T*)(m_pContent) + m_Size - index + size, (T*)(m_pContent) + m_Size - index, index * sizeof(T));
						}
						else
						{
							for (SizeType i = m_Size + size - 1; i >= std::max(m_Size, m_Size - index + size); i--)
							{
								new (reinterpret_cast<T*>(m_pContent) + i) T(std::move(GetObject(i - size)));
							}
							for (SizeType i = m_Size - 1; i >= m_Size - index + size; i--)
							{
								GetObject(i) = std::move(GetObject(i - size));
							}
						}
						auto aiter = ilist.begin();
						for (SizeType i = m_Size - index + size - 1; i >= m_Size; i--, aiter += 1)
						{
							new (reinterpret_cast<T*>(m_pContent) + i) T(*aiter);
							if (i == 0)
								break;
						}
						for (SizeType i = std::min(m_Size - index + size, m_Size) - 1; i >= m_Size - index; i--, aiter += 1)
						{
							GetObject(i) = *aiter;
							if (i == 0)
								break;
						}
						m_Size += size;

						return IteratorType(reinterpret_cast<T*>(m_pContent) + m_Size - index - 1);
					}
				}
				else
					return iter;
			}
		}

		/*!
		@brief remove the last element in the Vector.
		*/
		inline void PopBack()
		{
			SGE_ASSERT(EmptyVectorError, m_Size);

			GetObject(m_Size - 1).~T();

			m_Size -= 1;
		}

		/*!
		@brief remove a element in Vector by giving a iterator.
		@warning only support sequential iterator.
		@return the iterator which points to the next element after the removing has been done.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Remove(const IteratorType& iter)
		{
			SGE_ASSERT(EmptyVectorError, m_Size);
			SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size - 1);

			if constexpr (IsTrivial<T>)
			{
			}
			else
			{
				iter->~T();
			}

			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				if constexpr (IsTrivial<T>)
				{
					memmove((T*)(iter.GetData()), (T*)(iter.GetData()) + 1, ((IteratorType::GetEnd(*this) - iter) - 1) * sizeof(T));
				}
				else
				{
					if (iter != IteratorType::GetEnd(*this) - 1)
					{
						new ((T*)(iter.GetData())) T(std::move(const_cast<T&>(*(iter + 1))));
						for (auto i = iter + 1; i != IteratorType::GetEnd(*this) - 1; i += 1)
						{
							const_cast<T&>(*i) = std::move(const_cast<T&>(*(i + 1)));
						}
					}
				}
				m_Size -= 1;
				return iter;
			}
			else	// reverse
			{
				if constexpr (IsTrivial<T>)
				{
					memmove((T*)(iter.GetData()), (T*)(iter.GetData()) + 1, (iter - IteratorType::GetBegin(*this)) * sizeof(T));
				}
				else
				{
					if (iter != IteratorType::GetBegin(*this))
					{
						new ((T*)(iter.GetData())) T(std::move(const_cast<T&>(*(iter - 1))));
						for (auto i = iter - 1; i != IteratorType::GetBegin(*this); i -= 1)
						{
							const_cast<T&>(*i) = std::move(const_cast<T&>(*(i - 1)));
						}
					}
				}
				m_Size -= 1;
				return iter + 1;
			}
		}

		/*!
		@brief remove some elements in Vector by giving the iterators.
		@warning only support sequential iterator.
		@warning the begin&end's Container must be current Vector.
		@return the iterator which points to the next element after the removing has been done.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsVectorIterator<IteratorType>::Value, bool>>
		inline IteratorType Remove(const IteratorType& begin, const IteratorType& end)
		{
			SGE_ASSERT(EmptyVectorError, m_Size);
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, begin, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
				SGE_ASSERT(typename IteratorType::OutOfRangeError, end, reinterpret_cast<T*>(m_pContent), reinterpret_cast<T*>(m_pContent) + m_Size);
			}
			else	// reverse
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, begin, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
				SGE_ASSERT(typename IteratorType::OutOfRangeError, end, reinterpret_cast<T*>(m_pContent) - 1, reinterpret_cast<T*>(m_pContent) + m_Size - 1);
			}
			SizeType size = end - begin;
			SGE_ASSERT(InvalidValueError, size, 1, m_Size);

			if constexpr (IsTrivial<T>)
			{
			}
			else
			{
				for (auto i = begin; i != end; i += 1)
				{
					const_cast<T&>(*i).~T();
				}
			}

			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				if constexpr (IsTrivial<T>)
				{
					memmove((T*)(begin.GetData()), (T*)(end.GetData()), (IteratorType::GetEnd(*this) - end) * sizeof(T));
				}
				else
				{
					auto iter = begin;
					for (; iter != end && iter + size != IteratorType::GetEnd(*this); iter += 1)
					{
						new ((T*)(iter.GetData())) T(std::move(const_cast<T&>(*(iter + size))));
					}
					if (iter + size != IteratorType::GetEnd(*this))
					{
						for (; iter + size != IteratorType::GetEnd(*this); iter += 1)
						{
							const_cast<T&>(*iter) = std::move(const_cast<T&>(*(iter + size)));
						}
					}
				}
				m_Size -= size;
				return begin;
			}
			else	// reverse
			{
				if constexpr (IsTrivial<T>)
				{
					memmove((T*)(end.GetData()) + 1, (T*)(begin.GetData()) + 1, (begin - IteratorType::GetBegin(*this)) * sizeof(T));
				}
				else
				{
					auto iter = end - 1;
					for (; iter != begin - 1 && iter - size != IteratorType::GetBegin(*this) - 1; iter -= 1)
					{
						new ((T*)(iter.GetData())) T(std::move(const_cast<T&>(*(iter - size))));
					}
					if (iter - size != IteratorType::GetBegin(*this) - 1)
					{
						for (; iter - size != IteratorType::GetBegin(*this) - 1; iter -= 1)
						{
							const_cast<T&>(*iter) = std::move(const_cast<T&>(*(iter - size)));
						}
					}
				}
				m_Size -= size;
				return end;
			}
		}

		/*!
		@brief find a element in this Vector which its value is equal to the given value.
		@note use normal O(n) algorithm.
		@return the iterator which points to the first matching element, return end iterator if no matching.
		*/
		inline Iterator Find(const T& val)
		{
			for (Iterator iter = GetBegin(); iter != GetEnd(); iter += 1)
			{
				if (*iter == val)
				{
					return iter;
				}
			}
			return GetEnd();
		}

		/*!
		@brief find a element in this Vector which its value is equal to the given value.
		@note use normal O(n) algorithm.
		@return the iterator which points to the first matching element, return end iterator if no matching.
		*/
		inline ConstIterator Find(const T& val) const
		{
			for (ConstIterator iter = GetConstBegin(); iter != GetConstEnd(); iter += 1)
			{
				if (*iter == val)
				{
					return iter;
				}
			}
			return GetConstEnd();
		}

		/*!
		@brief find a element in this Vector when invoking the given judge function by using it as the argument can get result true.
		@note use normal O(n) algorithm.
		@return the iterator which points to the first matching element, return end iterator if no matching.
		*/
		template<typename Callable>
		inline Iterator FindByFunction(Callable&& judge_func)
		{
			for (Iterator iter = GetBegin(); iter != GetEnd(); iter += 1)
			{
				if (judge_func(*iter))
				{
					return iter;
				}
			}
			return GetEnd();
		}

		/*!
		@brief find a element in this Vector when invoking the given judge function by using it as the argument can get result true.
		@note use normal O(n) algorithm.
		@return the iterator which points to the first matching element, return end iterator if no matching.
		*/
		template<typename Callable>
		inline ConstIterator FindByFunction(Callable&& judge_func) const
		{
			for (ConstIterator iter = GetConstBegin(); iter != GetConstEnd(); iter += 1)
			{
				if (judge_func(*iter))
				{
					return iter;
				}
			}
			return GetConstEnd();
		}

		/*!
		@brief find elements in this Vector which their value are equal to the given value and invoke the processing function with the each element as the argument.
		@note use normal O(n) algorithm.
		*/
		template<typename Callable>
		inline void FindAll(const T& val, Callable&& process_func)
		{
			for (Iterator iter = GetBegin(); iter != GetEnd(); iter += 1)
			{
				if (*iter == val)
				{
					process_func(*iter);
				}
			}
		}

		/*!
		@brief find elements in this Vector which their value are equal to the given value and invoke the processing function with the each element as the argument.
		@note use normal O(n) algorithm.
		*/
		template<typename Callable>
		inline void FindAll(const T& val, Callable&& process_func) const
		{
			for (ConstIterator iter = GetConstBegin(); iter != GetConstEnd(); iter += 1)
			{
				if (*iter == val)
				{
					process_func(*iter);
				}
			}
		}

		/*!
		@brief find elements in this Vector when invoking the given judge function by using them as the argument can get result true and then invoke the processing function with the each element as the argument.
		@note use normal O(n) algorithm.
		*/
		template<typename Callable1, typename Callable2>
		inline void FindAllByFunction(Callable1&& judge_func, Callable2&& process_func)
		{
			for (Iterator iter = GetBegin(); iter != GetEnd(); iter += 1)
			{
				if (judge_func(*iter))
				{
					process_func(*iter);
				}
			}
		}

		/*!
		@brief find elements in this Vector when invoking the given judge function by using them as the argument can get result true and then invoke the processing function with the each element as the argument.
		@note use normal O(n) algorithm.
		*/
		template<typename Callable1, typename Callable2>
		inline void FindAllByFunction(Callable1&& judge_func, Callable2&& process_func) const
		{
			for (ConstIterator iter = GetConstBegin(); iter != GetConstEnd(); iter += 1)
			{
				if (judge_func(*iter))
				{
					process_func(*iter);
				}
			}
		}

		inline bool operator==(const Vector& vector) const
		{
			if (m_Size != vector.m_Size)
				return false;

			auto iter = GetConstBegin();
			auto oiter = vector.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return false;
				++iter;
				++oiter;
			}

			return true;
		}

		template<typename OtherAllocator>
		inline bool operator==(const Vector<T, OtherAllocator>& vector) const
		{
			if (m_Size != vector.m_Size)
				return false;

			auto iter = GetConstBegin();
			auto oiter = vector.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return false;
				++iter;
				++oiter;
			}

			return true;
		}

		inline bool operator!=(const Vector& vector) const
		{
			if (m_Size != vector.m_Size)
				return true;

			auto iter = GetConstBegin();
			auto oiter = vector.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return true;
				++iter;
				++oiter;
			}

			return false;
		}

		template<typename OtherAllocator>
		inline bool operator!=(const Vector<T, OtherAllocator>& vector) const
		{
			if (m_Size != vector.m_Size)
				return true;

			auto iter = GetConstBegin();
			auto oiter = vector.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return true;
				++iter;
				++oiter;
			}

			return false;
		}

	private:
		void* m_pContent;
		SizeType m_RealSize;
		SizeType m_Size;
	};

}

/*!
@}
*/