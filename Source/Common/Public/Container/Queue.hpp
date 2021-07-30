/*
Copyright 2021 creatorlxd

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
#include "List.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Allocator = DefaultAllocator, template<typename, typename> class Implement = List>
	class Queue
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;
		using ImplementType = Implement<T, Allocator>;

		struct EmptyQueueError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The Queue is empty");
			inline static bool Judge(SizeType size)
			{
				return size == 0;
			}
		};

		template<typename _T, typename _Allocator, template<typename, typename> class _Implement>
		friend class Queue;

	public:
		inline Queue()
			: m_Implement()
		{
		}

		inline Queue(const Queue& s)
			: m_Implement(s.m_Implement)
		{
		}

		inline Queue(Queue&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		inline Queue& operator=(const Queue& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = s.m_Implement;
			return *this;
		}

		inline Queue& operator=(Queue&& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		template<typename OtherAllocator>
		inline Queue(const Queue<T, OtherAllocator, Implement>& s)
			: m_Implement(s.m_Implement)
		{
		}

		template<typename OtherAllocator>
		inline Queue(Queue<T, OtherAllocator, Implement>&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		template<typename OtherAllocator>
		inline Queue& operator=(const Queue<T, OtherAllocator, Implement>& s)
		{
			m_Implement = s.m_Implement;
			return *this;
		}

		template<typename OtherAllocator>
		inline Queue& operator=(Queue<T, OtherAllocator, Implement>&& s)
		{
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		inline Queue(SizeType size, const T& val)
			: m_Implement(size, val)
		{
		}

		template<typename AnotherIteratorType, typename = std::enable_if_t<IsSequentialIterator<AnotherIteratorType>::Value, bool>>
		inline Queue(const AnotherIteratorType& begin, const AnotherIteratorType& end)
			: m_Implement(begin, end)
		{
		}

		inline Queue(std::initializer_list<T> ilist)
			: m_Implement(ilist)
		{
		}

		inline Queue(const ImplementType& c)
			: m_Implement(c)
		{
		}

		inline Queue(ImplementType&& c)
			: m_Implement(std::move(c))
		{
		}

		template<typename OtherAllocator>
		inline Queue(const Implement<T, OtherAllocator>& c)
			: m_Implement(c)
		{
		}

		template<typename OtherAllocator>
		inline Queue(Implement<T, OtherAllocator>&& c)
			: m_Implement(std::move(c))
		{
		}

		inline SizeType GetSize() const
		{
			return m_Implement.GetSize();
		}

		inline ImplementType& GetImplement()
		{
			return m_Implement;
		}

		inline const ImplementType& GetImplement() const
		{
			return m_Implement;
		}

		inline void Push(const T& val)
		{
			m_Implement.PushBack(val);
		}

		inline void Push(T&& val)
		{
			m_Implement.PushBack(std::move(val));
		}

		inline void Pop()
		{
			SGE_ASSERT(EmptyQueueError, m_Implement.GetSize());
			m_Implement.PopFront();
		}

		inline T& GetFront()
		{
			SGE_ASSERT(EmptyQueueError, m_Implement.GetSize());
			return *(m_Implement.GetBegin());
		}

		inline const T& GetFront() const
		{
			SGE_ASSERT(EmptyQueueError, m_Implement.GetSize());
			return *(m_Implement.GetConstBegin());
		}

	private:
		ImplementType m_Implement;
	};

	/*!
	@}
	*/
}