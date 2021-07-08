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

			template<typename... Args>
			inline Node(Args&&... args)
				: m_Content(std::forward<Args>(args)...), m_pNext(nullptr), m_pPrevious(nullptr)
			{
			}
		};

	public:
		inline List()
			: m_pHead(nullptr), m_pTail(nullptr), m_Size(0)
		{
		}

		inline ~List()
		{
			if (m_Size)
			{
				Node* pn = m_pHead;
				while (pn != nullptr)
				{
					Node* pb = pn;
					pn = pn->m_pNext;
					Allocator::template Delete(pb);
				}
			}
		}

		inline void Clear()
		{
			if (m_Size)
			{
				Node* pn = m_pHead;
				while (pn != nullptr)
				{
					Node* pb = pn;
					pn = pn->m_pNext;
					Allocator::template Delete(pb);
				}
			}

			m_Size = 0;
			m_pHead = nullptr;
			m_pTail = nullptr;
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

			template<typename __T>
			friend class ReverseIteratorImpl;

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

			inline _T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

			inline _T& operator*() const
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

			inline _T* GetData() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

		private:
			using NodePointerType = std::conditional_t<std::is_const_v<_T>, const Node*, Node*>;

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

		template<typename _T>
		class ReverseIteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter)
				{
					return iter.m_pNode == nullptr;
				}
			};

			using ValueType = _T;

			friend class List<T, Allocator>;

			template<typename IteratorType>
			friend class IteratorImpl;

		public:
			inline static ReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const List&, List&> l)
			{
				return ReverseIteratorImpl(l.m_pTail, l.m_pHead, l.m_pTail);
			}

			inline static ReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const List&, List&> l)
			{
				return ReverseIteratorImpl(nullptr, l.m_pHead, l.m_pTail);
			}

			inline ReverseIteratorImpl(const ReverseIteratorImpl& iter)
				: m_pNode(iter.m_pNode), m_pHead(iter.m_pHead), m_pTail(iter.m_pTail)
			{
			}

			inline ReverseIteratorImpl& operator=(const ReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pNode = iter.m_pNode;
				m_pHead = iter.m_pHead;
				m_pTail = iter.m_pTail;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl(const IteratorType& iter)
				: m_pNode((NodePointerType)iter.m_pNode), m_pHead((NodePointerType)iter.m_pHead), m_pTail((NodePointerType)iter.m_pTail)
			{
			}

			template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl& operator=(const IteratorType& iter)
			{
				m_pNode = (NodePointerType)iter.m_pNode;
				m_pHead = (NodePointerType)iter.m_pHead;
				m_pTail = (NodePointerType)iter.m_pTail;
				return *this;
			}

			inline ReverseIteratorImpl operator+(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				for (SizeType j = 0; j < i; ++j)
					re.MoveForward();
				return re;
			}

			inline ReverseIteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
					MoveForward();
				return *this;
			}

			inline ReverseIteratorImpl& operator++()
			{
				MoveForward();
				return *this;
			}

			inline const ReverseIteratorImpl operator++(int)
			{
				ReverseIteratorImpl re(*this);
				MoveForward();
				return re;
			}

			inline ReverseIteratorImpl operator-(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				for (SizeType j = 0; j < i; ++j)
					re.MoveBack();
				return re;
			}

			inline ReverseIteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
					MoveBack();
				return *this;
			}

			inline ReverseIteratorImpl& operator--()
			{
				MoveBack();
				return *this;
			}

			inline const ReverseIteratorImpl operator--(int)
			{
				ReverseIteratorImpl re(*this);
				MoveBack();
				return re;
			}

			inline SizeType operator-(const ReverseIteratorImpl& iter) const
			{
				SizeType re = 0;
				ReverseIteratorImpl i = iter;
				while (i != *this)
				{
					i.MoveForward();
					re += 1;
				}
				return re;
			}

			inline _T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

			inline _T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pNode->m_Content;
			}

			inline bool operator==(const ReverseIteratorImpl& iter) const
			{
				return m_pNode == iter.m_pNode && m_pHead == iter.m_pHead && m_pTail == iter.m_pTail;
			}

			inline bool operator!=(const ReverseIteratorImpl& iter) const
			{
				return m_pNode != iter.m_pNode || m_pHead != iter.m_pHead || m_pTail != iter.m_pTail;
			}

			inline _T* GetData() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pNode->m_Content);
			}

		private:
			using NodePointerType = std::conditional_t<std::is_const_v<_T>, const Node*, Node*>;

			inline ReverseIteratorImpl(NodePointerType pnode, NodePointerType phead, NodePointerType ptail)
				: m_pNode(pnode), m_pHead(phead), m_pTail(ptail)
			{
			}

			inline void MoveBack()
			{
				if (m_pNode)
					m_pNode = m_pNode->m_pNext;
				else
					m_pNode = m_pHead;
			}

			inline void MoveForward()
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

		using Iterator = IteratorImpl<T>;
		using ConstIterator = IteratorImpl<const T>;
		using ReverseIterator = ReverseIteratorImpl<T>;
		using ConstReverseIterator = ReverseIteratorImpl<const T>;

		template<typename IteratorType>
		struct IsListIterator
		{
			inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator> || std::is_same_v<IteratorType, ReverseIterator> || std::is_same_v<IteratorType, ConstReverseIterator>;
		};

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

		inline T& PushBack(const T& val)
		{
			Node* pn = Allocator::template New<Node>(val);
			if (m_Size)
			{
				pn->m_pPrevious = m_pTail;
				m_pTail->m_pNext = pn;
				m_pTail = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		inline T& PushBack(T&& val)
		{
			Node* pn = Allocator::template New<Node>(std::move(val));
			if (m_Size)
			{
				pn->m_pPrevious = m_pTail;
				m_pTail->m_pNext = pn;
				m_pTail = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		template<typename... Args>
		inline T& EmplaceBack(Args&&... args)
		{
			Node* pn = Allocator::template New<Node>(std::forward<Args>(args)...);
			if (m_Size)
			{
				pn->m_pPrevious = m_pTail;
				m_pTail->m_pNext = pn;
				m_pTail = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		inline T& PushFront(const T& val)
		{
			Node* pn = Allocator::template New<Node>(val);
			if (m_Size)
			{
				pn->m_pNext = m_pHead;
				m_pHead->m_pPrevious = pn;
				m_pHead = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		inline T& PushFront(T&& val)
		{
			Node* pn = Allocator::template New<Node>(std::move(val));
			if (m_Size)
			{
				pn->m_pNext = m_pHead;
				m_pHead->m_pPrevious = pn;
				m_pHead = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		template<typename... Args>
		inline T& EmplaceFront(Args&&... args)
		{
			Node* pn = Allocator::template New<Node>(std::forward<Args>(args)...);
			if (m_Size)
			{
				pn->m_pNext = m_pHead;
				m_pHead->m_pPrevious = pn;
				m_pHead = pn;
			}
			else
			{
				m_pHead = pn;
				m_pTail = pn;
			}
			m_Size += 1;
			return pn->m_Content;
		}

		template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const T& val)
		{
			Node* pn = Allocator::template New<Node>(val);
			m_Size += 1;
			IteratorType biter = iter - 1;
			Node* pfwd = nullptr;
			Node* pbck = nullptr;
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				pfwd = (Node*)iter.m_pNode;
				pbck = (Node*)biter.m_pNode;
			}
			else	//Reverse
			{
				pfwd = (Node*)biter.m_pNode;
				pbck = (Node*)iter.m_pNode;
			}
			pn->m_pNext = pfwd;
			pn->m_pPrevious = pbck;

			if (pfwd)
				pfwd->m_pPrevious = pn;
			else
				m_pTail = pn;

			if (pbck)
				pbck->m_pNext = pn;
			else
				m_pHead = pn;

			return IteratorType(pn, m_pHead, m_pTail);
		}

		template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, T&& val)
		{
			Node* pn = Allocator::template New<Node>(std::move(val));
			m_Size += 1;
			IteratorType biter = iter - 1;
			Node* pfwd = nullptr;
			Node* pbck = nullptr;
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				pfwd = (Node*)iter.m_pNode;
				pbck = (Node*)biter.m_pNode;
			}
			else	//Reverse
			{
				pfwd = (Node*)biter.m_pNode;
				pbck = (Node*)iter.m_pNode;
			}
			pn->m_pNext = pfwd;
			pn->m_pPrevious = pbck;

			if (pfwd)
				pfwd->m_pPrevious = pn;
			else
				m_pTail = pn;

			if (pbck)
				pbck->m_pNext = pn;
			else
				m_pHead = pn;

			return IteratorType(pn, m_pHead, m_pTail);
		}

		template<typename IteratorType, typename = std::enable_if_t<IsListIterator<IteratorType>::Value, bool>, typename... Args>
		inline IteratorType Emplace(const IteratorType& iter, Args&&... args)
		{
			Node* pn = Allocator::template New<Node>(std::forward<Args>(args)...);
			m_Size += 1;
			IteratorType biter = iter - 1;
			Node* pfwd = nullptr;
			Node* pbck = nullptr;
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				pfwd = (Node*)iter.m_pNode;
				pbck = (Node*)biter.m_pNode;
			}
			else	//Reverse
			{
				pfwd = (Node*)biter.m_pNode;
				pbck = (Node*)iter.m_pNode;
			}
			pn->m_pNext = pfwd;
			pn->m_pPrevious = pbck;

			if (pfwd)
				pfwd->m_pPrevious = pn;
			else
				m_pTail = pn;

			if (pbck)
				pbck->m_pNext = pn;
			else
				m_pHead = pn;

			return IteratorType(pn, m_pHead, m_pTail);
		}

	private:
		inline void RawClear()
		{
			if (m_Size)
			{
				Node* pn = m_pHead;
				while (pn != nullptr)
				{
					Node* pb = pn;
					pn = pn->m_pNext;
					Allocator::template Delete(pb);
				}
			}
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