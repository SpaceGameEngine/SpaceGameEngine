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
#include <initializer_list>
#include "TypeDefinition.hpp"
#include "MemoryManager.h"
#include "Error.h"
#include "ReverseSequentialIterator.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Allocator = DefaultAllocator>
	class List
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;

	private:
		struct Node
		{
			T m_Content;
			Node* m_pNext;
			Node* m_pPrevious;

			template<typename U>
			inline Node(U&& content)
				: m_Content(std::forward<U>(content)), m_pNext(nullptr), m_pPrevious(nullptr)
			{
			}
		};

	public:
		inline List()
			: m_pHead(nullptr), m_pTail(nullptr), m_Size(0)
		{
		}

		inline SizeType GetSize() const
		{
			return m_Size;
		}

		template<typename IteratorType>
		struct IsListIterator;

		template<typename _T>
		class IteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter)
				{
					return iter.m_pNode == nullptr;
				}
			};

			using ValueType = _T;

			friend class List<T, Allocator>;

			template<typename IteratorType>
			friend class ReverseSequentialIterator;

			template<typename IteratorType>
			friend class ListReverseIteratorImpl;

		public:
			inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const List&, List&> l)
			{
				return IteratorImpl(l.m_pHead, l.m_pHead, l.m_pTail);
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const List&, List&> l)
			{
				return IteratorImpl(nullptr, l.m_pHead, l.m_pTail);
			}

			inline IteratorImpl(const IteratorImpl& iter)
				: m_pNode(iter.m_pNode), m_pHead(iter.m_pHead), m_pTail(iter.m_pTail)
			{
			}

			inline IteratorImpl& operator=(const IteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pNode = iter.m_pNode;
				m_pHead = iter.m_pHead;
				m_pTail = iter.m_pTail;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl(const IteratorType& iter)
				: m_pNode((NodePointerType)iter.m_pNode), m_pHead((NodePointerType)iter.m_pHead), m_pTail((NodePointerType)iter.m_pTail)
			{
			}

			template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl& operator=(const IteratorType& iter)
			{
				m_pNode = (NodePointerType)iter.m_pNode;
				m_pHead = (NodePointerType)iter.m_pHead;
				m_pTail = (NodePointerType)iter.m_pTail;
				return *this;
			}

			inline IteratorImpl operator+(SizeType i) const
			{
				IteratorImpl re(*this);
				for (SizeType j = 0; j < i; ++j)
					re.MoveForward();
				return re;
			}

			inline IteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
					MoveForward();
				return *this;
			}

			inline IteratorImpl& operator++()
			{
				MoveForward();
				return *this;
			}

			inline const IteratorImpl operator++(int)
			{
				IteratorImpl re(*this);
				MoveForward();
				return re;
			}

			inline IteratorImpl operator-(SizeType i) const
			{
				IteratorImpl re(*this);
				for (SizeType j = 0; j < i; ++j)
					re.MoveBack();
				return re;
			}

			inline IteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
					MoveBack();
				return *this;
			}

			inline IteratorImpl& operator--()
			{
				MoveBack();
				return *this;
			}

			inline const IteratorImpl operator--(int)
			{
				IteratorImpl re(*this);
				MoveBack();
				return re;
			}

			inline SizeType operator-(const IteratorImpl& iter) const
			{
				SizeType re = 0;
				IteratorImpl i = iter;
				while (i != *this)
				{
					i.MoveForward();
					re += 1;
				}
				return re;
			}

			inline T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

			inline T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pNode->m_Content;
			}

			inline bool operator==(const IteratorImpl& iter) const
			{
				return m_pNode == iter.m_pNode && m_pHead == iter.m_pHead && m_pTail == iter.m_pTail;
			}

			inline bool operator!=(const IteratorImpl& iter) const
			{
				return m_pNode != iter.m_pNode || m_pHead != iter.m_pHead || m_pTail != iter.m_pTail;
			}

			inline T* GetData() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

		private:
			using NodePointerType = std::conditional_t<std::is_const_v<T>, const Node*, Node*>;

			inline IteratorImpl(NodePointerType pnode, NodePointerType phead, NodePointerType ptail)
				: m_pNode(pnode), m_pHead(phead), m_pTail(ptail)
			{
			}

			inline void MoveForward()
			{
				if (m_pNode)
					m_pNode = m_pNode->m_pNext;
				else
					m_pNode = m_pHead;
			}

			inline void MoveBack()
			{
				if (m_pNode)
					m_pNode = m_pNode->m_pPrevious;
				else
					m_pNode = m_pTail;
			}

		private:
			NodePointerType m_pNode;
			NodePointerType m_pHead;
			NodePointerType m_pTail;
		};

		template<typename IteratorType>
		class ListReverseIteratorImpl : public ReverseSequentialIterator<IteratorType>
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const ListReverseIteratorImpl& iter)
				{
					return iter.GetContent().m_pNode == nullptr;
				}
			};

			using ValueType = typename IteratorType::ValueType;

		public:
			inline static ListReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<ValueType>, const List&, List&> l)
			{
				return ListReverseIteratorImpl(l.m_pTail, l.m_pHead, l.m_pTail);
			}

			inline static ListReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<ValueType>, const List&, List&> l)
			{
				return ListReverseIteratorImpl(nullptr, l.m_pHead, l.m_pTail);
			}

			inline ListReverseIteratorImpl(const ListReverseIteratorImpl& iter)
				: ReverseSequentialIterator<IteratorType>(iter)
			{
			}

			inline ListReverseIteratorImpl& operator=(const ListReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				ReverseSequentialIterator<IteratorType>::operator=(iter);
				return *this;
			}

			template<typename _IteratorType, typename = std::enable_if_t<IsListIterator<_IteratorType>::Value && (std::is_same_v<typename _IteratorType::ValueType, ValueType> || std::is_same_v<typename _IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ListReverseIteratorImpl(const _IteratorType& iter)
				: ReverseSequentialIterator<IteratorType>(iter)
			{
			}

			template<typename _IteratorType, typename = std::enable_if_t<IsListIterator<_IteratorType>::Value && (std::is_same_v<typename _IteratorType::ValueType, ValueType> || std::is_same_v<typename _IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ListReverseIteratorImpl& operator=(const _IteratorType& iter)
			{
				ReverseSequentialIterator<IteratorType>::operator=(iter);
				return *this;
			}

			inline ListReverseIteratorImpl operator+(SizeType i) const
			{
				return ReverseSequentialIterator<IteratorType>::operator+(i);
			}

			inline ListReverseIteratorImpl& operator+=(SizeType i)
			{
				ReverseSequentialIterator<IteratorType>::operator+=(i);
				return *this;
			}

			inline ListReverseIteratorImpl& operator++()
			{
				ReverseSequentialIterator<IteratorType>::operator++();
				return *this;
			}

			inline const ListReverseIteratorImpl operator++(int)
			{
				return ReverseSequentialIterator<IteratorType>::operator++(0);
			}

			inline ListReverseIteratorImpl operator-(SizeType i) const
			{
				return ReverseSequentialIterator<IteratorType>::operator-(i);
			}

			inline ListReverseIteratorImpl& operator-=(SizeType i)
			{
				ReverseSequentialIterator<IteratorType>::operator-=(i);
				return *this;
			}

			inline ListReverseIteratorImpl& operator--()
			{
				ReverseSequentialIterator<IteratorType>::operator--();
				return *this;
			}

			inline const ListReverseIteratorImpl operator--(int)
			{
				return ReverseSequentialIterator<IteratorType>::operator--(0);
			}

			inline SizeType operator-(const ListReverseIteratorImpl& iter) const
			{
				return ReverseSequentialIterator<IteratorType>::operator-(iter);
			}

			inline bool operator==(const ListReverseIteratorImpl& iter) const
			{
				return ReverseSequentialIterator<IteratorType>::operator==(iter);
			}

			inline bool operator!=(const ListReverseIteratorImpl& iter) const
			{
				return ReverseSequentialIterator<IteratorType>::operator!=(iter);
			}

		private:
			using NodePointerType = typename IteratorType::NodePointerType;

			inline ListReverseIteratorImpl(NodePointerType pnode, NodePointerType phead, NodePointerType ptail)
				: ReverseSequentialIterator<IteratorType>(pnode, phead, ptail)
			{
			}

			inline ListReverseIteratorImpl(const ReverseSequentialIterator<IteratorType>& iter)
				: ReverseSequentialIterator<IteratorType>(iter)
			{
			}
		};

		using Iterator = IteratorImpl<T>;
		using ConstIterator = IteratorImpl<const T>;
		using ReverseIterator = ListReverseIteratorImpl<IteratorImpl<T>>;
		using ConstReverseIterator = ListReverseIteratorImpl<IteratorImpl<const T>>;

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

	private:
		Node* m_pHead;
		Node* m_pTail;
		SizeType m_Size;
	};
	/*!
	@}
	*/
}