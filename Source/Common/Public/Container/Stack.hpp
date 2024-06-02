/*
Copyright 2024 creatorlxd

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
#include "List.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	template<typename T, typename Allocator = DefaultAllocator, template<typename, typename> class Implement = List>
	class Stack
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;
		using ImplementType = Implement<T, Allocator>;

		struct EmptyStackError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Stack is empty");
			inline static bool Judge(SizeType size)
			{
				return size == 0;
			}
		};

		template<typename _T, typename _Allocator, template<typename, typename> class _Implement>
		friend class Stack;

	public:
		inline Stack()
			: m_Implement()
		{
		}

		inline Stack(const Stack& s)
			: m_Implement(s.m_Implement)
		{
		}

		inline Stack(Stack&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		inline Stack& operator=(const Stack& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = s.m_Implement;
			return *this;
		}

		inline Stack& operator=(Stack&& s)
		{
			SGE_ASSERT(SelfAssignmentError, this, &s);
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		template<typename OtherAllocator>
		inline Stack(const Stack<T, OtherAllocator, Implement>& s)
			: m_Implement(s.m_Implement)
		{
		}

		template<typename OtherAllocator>
		inline Stack(Stack<T, OtherAllocator, Implement>&& s)
			: m_Implement(std::move(s.m_Implement))
		{
		}

		template<typename OtherAllocator>
		inline Stack& operator=(const Stack<T, OtherAllocator, Implement>& s)
		{
			m_Implement = s.m_Implement;
			return *this;
		}

		template<typename OtherAllocator>
		inline Stack& operator=(Stack<T, OtherAllocator, Implement>&& s)
		{
			m_Implement = std::move(s.m_Implement);
			return *this;
		}

		inline Stack(SizeType size, const T& val)
			: m_Implement(size, val)
		{
		}

		template<typename AnotherIteratorType, typename = std::enable_if_t<IsSequentialIterator<AnotherIteratorType>, bool>>
		inline Stack(const AnotherIteratorType& begin, const AnotherIteratorType& end)
			: m_Implement(begin, end)
		{
		}

		inline Stack(std::initializer_list<T> ilist)
			: m_Implement(ilist)
		{
		}

		inline Stack(const ImplementType& c)
			: m_Implement(c)
		{
		}

		inline Stack(ImplementType&& c)
			: m_Implement(std::move(c))
		{
		}

		template<typename OtherAllocator>
		inline Stack(const Implement<T, OtherAllocator>& c)
			: m_Implement(c)
		{
		}

		template<typename OtherAllocator>
		inline Stack(Implement<T, OtherAllocator>&& c)
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

		template<typename... Args>
		inline void Emplace(Args&&... args)
		{
			m_Implement.EmplaceBack(std::forward<Args>(args)...);
		}

		inline void Pop()
		{
			SGE_ASSERT(EmptyStackError, m_Implement.GetSize());
			m_Implement.PopBack();
		}

		inline T& GetTop()
		{
			SGE_ASSERT(EmptyStackError, m_Implement.GetSize());
			return *(m_Implement.GetReverseBegin());
		}

		inline const T& GetTop() const
		{
			SGE_ASSERT(EmptyStackError, m_Implement.GetSize());
			return *(m_Implement.GetConstReverseBegin());
		}

		inline bool operator==(const Stack& stack) const
		{
			return m_Implement == stack.m_Implement;
		}

		template<typename OtherAllocator, template<typename, typename> class OtherImplement>
		inline bool operator==(const Stack<T, OtherAllocator, OtherImplement>& stack) const
		{
			return m_Implement == stack.m_Implement;
		}

		inline bool operator!=(const Stack& stack) const
		{
			return m_Implement != stack.m_Implement;
		}

		template<typename OtherAllocator, template<typename, typename> class OtherImplement>
		inline bool operator!=(const Stack<T, OtherAllocator, OtherImplement>& stack) const
		{
			return m_Implement != stack.m_Implement;
		}

	private:
		ImplementType m_Implement;
	};

}

/*!
@}
*/