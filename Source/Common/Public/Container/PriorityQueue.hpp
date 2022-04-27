/*
Copyright 2022 creatorlxd

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
#include "Vector.hpp"
#include "Utility/Utility.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Comparer = Less<T>, typename Allocator = DefaultAllocator, template<typename, typename> class Implement = Vector>
	class PriorityQueue
	{
	public:
		using ValueType = T;
		using ComparerType = Comparer;
		using AllocatorType = Allocator;
		using ImplementType = Implement<T, Allocator>;

		struct EmptyPriorityQueueError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The PriorityQueue is empty");
			inline static bool Judge(SizeType size)
			{
				return size == 0;
			}
		};

		template<typename _T, typename _Comparer, typename _Allocator, template<typename, typename> class _Implement>
		friend class PriorityQueue;

	public:
		inline PriorityQueue()
			: m_Implement()
		{
		}

		inline PriorityQueue(const PriorityQueue& s)
			: m_Implement(s.m_Implement)
		{
		}

		inline PriorityQueue(PriorityQueue&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		inline PriorityQueue& operator=(const PriorityQueue& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = s.m_Implement;
			return *this;
		}

		inline PriorityQueue& operator=(PriorityQueue&& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		template<typename OtherAllocator>
		inline PriorityQueue(const PriorityQueue<T, Comparer, OtherAllocator, Implement>& s)
			: m_Implement(s.m_Implement)
		{
		}

		template<typename OtherAllocator>
		inline PriorityQueue(PriorityQueue<T, Comparer, OtherAllocator, Implement>&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		template<typename OtherAllocator>
		inline PriorityQueue& operator=(const PriorityQueue<T, Comparer, OtherAllocator, Implement>& s)
		{
			m_Implement = s.m_Implement;
			return *this;
		}

		template<typename OtherAllocator>
		inline PriorityQueue& operator=(PriorityQueue<T, Comparer, OtherAllocator, Implement>&& s)
		{
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		inline PriorityQueue(SizeType size, const T& val)
			: m_Implement(size, val)
		{
		}

		template<typename AnotherIteratorType, typename = std::enable_if_t<IsSequentialIterator<AnotherIteratorType>, bool>>
		inline PriorityQueue(const AnotherIteratorType& begin, const AnotherIteratorType& end)
			: m_Implement(begin, end)
		{
			BuildHeap();
		}

		inline PriorityQueue(std::initializer_list<T> ilist)
			: m_Implement(ilist)
		{
			BuildHeap();
		}

		inline PriorityQueue(const ImplementType& c)
			: m_Implement(c)
		{
			BuildHeap();
		}

		inline PriorityQueue(ImplementType&& c)
			: m_Implement(std::move(c))
		{
			BuildHeap();
		}

		template<typename OtherAllocator>
		inline PriorityQueue(const Implement<T, OtherAllocator>& c)
			: m_Implement(c)
		{
			BuildHeap();
		}

		template<typename OtherAllocator>
		inline PriorityQueue(Implement<T, OtherAllocator>&& c)
			: m_Implement(std::move(c))
		{
			BuildHeap();
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
			UpdateFromBottom();
		}

		inline void Push(T&& val)
		{
			m_Implement.PushBack(std::move(val));
			UpdateFromBottom();
		}

		inline void Pop()
		{
			SGE_ASSERT(EmptyPriorityQueueError, m_Implement.GetSize());
			if (m_Implement.GetSize() == 1)
				m_Implement.PopBack();
			else
			{
				std::swap(m_Implement[0], m_Implement[m_Implement.GetSize() - 1]);
				m_Implement.PopBack();
				UpdateFromTop();
			}
		}

		inline T& GetTop()
		{
			SGE_ASSERT(EmptyPriorityQueueError, m_Implement.GetSize());
			return m_Implement[0];
		}

		inline const T& GetTop() const
		{
			SGE_ASSERT(EmptyPriorityQueueError, m_Implement.GetSize());
			return m_Implement[0];
		}

		inline bool operator==(const PriorityQueue& priority_queue) const
		{
			return m_Implement == priority_queue.m_Implement;
		}

		template<typename OtherComparer, typename OtherAllocator, template<typename, typename> class OtherImplement>
		inline bool operator==(const PriorityQueue<T, OtherComparer, OtherAllocator, OtherImplement>& priority_queue) const
		{
			return m_Implement == priority_queue.m_Implement;
		}

		inline bool operator!=(const PriorityQueue& priority_queue) const
		{
			return m_Implement != priority_queue.m_Implement;
		}

		template<typename OtherComparer, typename OtherAllocator, template<typename, typename> class OtherImplement>
		inline bool operator!=(const PriorityQueue<T, OtherComparer, OtherAllocator, OtherImplement>& priority_queue) const
		{
			return m_Implement != priority_queue.m_Implement;
		}

	private:
		inline void UpdateFromBottom()
		{
			SizeType cur = m_Implement.GetSize() - 1;
			while (cur != 0)
			{
				SizeType fa = (cur - 1) / 2;
				if (Comparer::Compare(m_Implement[cur], m_Implement[fa]))
					std::swap(m_Implement[cur], m_Implement[fa]);
				cur = fa;
			}
		}

		inline void UpdateFromTop()
		{
			SizeType cur = 0;
			while ((2 * cur + 1) < m_Implement.GetSize())
			{
				SizeType next = 2 * cur + 1;
				if ((2 * cur + 2) < m_Implement.GetSize())
				{
					if (Comparer::Compare(m_Implement[2 * cur + 2], m_Implement[2 * cur + 1]))
						next = 2 * cur + 2;
				}
				if (Comparer::Compare(m_Implement[next], m_Implement[cur]))
				{
					std::swap(m_Implement[next], m_Implement[cur]);
					cur = next;
				}
				else
					break;
			}
		}

		inline void BuildHeap()
		{
			SizeType cnt = 0;
			do
			{
				cnt = 0;
				for (SizeType i = 0; i < m_Implement.GetSize(); ++i)
				{
					SizeType child = 2 * i + 1;
					if (child >= m_Implement.GetSize())
						break;
					if (Comparer::Compare(m_Implement[child], m_Implement[i]))
					{
						std::swap(m_Implement[child], m_Implement[i]);
						++cnt;
					}
					if ((child += 1) < m_Implement.GetSize())
					{
						if (Comparer::Compare(m_Implement[child], m_Implement[i]))
						{
							std::swap(m_Implement[child], m_Implement[i]);
							++cnt;
						}
					}
				}
			} while (cnt != 0);
		}

	private:
		ImplementType m_Implement;
	};

	/*!
	@}
	*/
}