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
#include "Container/Detail/RedBlackTree.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename K, typename V, typename LessComparer = Less<K>, typename Allocator = DefaultAllocator>
	class Map
	{
	public:
		using KeyType = const K;
		using ValueType = V;
		using AllocatorType = Allocator;
		using LessComparerType = LessComparer;

		template<typename _K, typename _V, typename _LessComparer, typename _Allocator>
		friend class Map;

		inline Map()
			: m_Tree()
		{
		}

		inline ~Map()
		{
		}

		inline Map(const Map& m)
			: m_Tree(m.m_Tree)
		{
		}

		inline Map(Map&& m)
			: m_Tree(std::move(m.m_Tree))
		{
		}

		inline Map& operator=(const Map& m)
		{
			SGE_ASSERT(SelfAssignmentError, this, &m);
			m_Tree = m.m_Tree;
			return *this;
		}

		inline Map& operator=(Map&& m)
		{
			SGE_ASSERT(SelfAssignmentError, this, &m);
			m_Tree = std::move(m.m_Tree);
			return *this;
		}

		template<typename OtherAllocator>
		inline Map(const Map<K, V, LessComparer, OtherAllocator>& m)
			: m_Tree(m.m_Tree)
		{
		}

		template<typename OtherAllocator>
		inline Map(Map<K, V, LessComparer, OtherAllocator>&& m)
			: m_Tree(std::move(m.m_Tree))
		{
		}

		template<typename OtherAllocator>
		inline Map& operator=(const Map<K, V, LessComparer, OtherAllocator>& m)
		{
			m_Tree = m.m_Tree;
			return *this;
		}

		template<typename OtherAllocator>
		inline Map& operator=(Map<K, V, LessComparer, OtherAllocator>&& m)
		{
			m_Tree = std::move(m.m_Tree);
			return *this;
		}

		inline Map(std::initializer_list<Pair<const K, V>> ilist)
		{
			for (auto i = ilist.begin(); i != ilist.end(); ++i)
			{
				m_Tree.InternalInsert(i->m_First, i->m_Second);
			}
		}

		inline void Clear()
		{
			m_Tree.Clear();
		}

		inline SizeType GetSize() const
		{
			return m_Tree.GetSize();
		}

		template<typename IteratorType>
		struct IsMapIterator;

		template<typename T>
		class IteratorImpl
		{
		public:
			using ValueType = T;

			friend class Map<K, V, LessComparer, Allocator>;

			struct OutOfRangeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter)
				{
					return iter.m_pContent == &(iter.m_pTree->m_NilNode);
				}
			};

		public:
			inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				if (m.m_Tree.m_Size)
					return IteratorImpl(m.m_Tree.GetMinimumNode(m.m_Tree.m_pRoot), &(m.m_Tree));
				else
					return IteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				return IteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline IteratorImpl(const IteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
			}

			inline IteratorImpl& operator=(const IteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
				return *this;
			}

			inline IteratorImpl& operator++()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const IteratorImpl operator++(int)
			{
				IteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline IteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator++();
				}
				return *this;
			}

			inline IteratorImpl operator+(SizeType i) const
			{
				IteratorImpl re(*this);
				re += i;
				return re;
			}

			inline IteratorImpl& operator--()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const IteratorImpl operator--(int)
			{
				IteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline IteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator--();
				}
				return *this;
			}

			inline IteratorImpl operator-(SizeType i) const
			{
				IteratorImpl re(*this);
				re -= i;
				return re;
			}

			inline SizeType operator-(const IteratorImpl& iter) const
			{
				SizeType re = 0;
				IteratorImpl i = iter;
				while (i != *this)
				{
					++i;
					re += 1;
				}
				return re;
			}

			inline T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pContent->m_KeyValuePair);
			}

			inline T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pContent->m_KeyValuePair;
			}

			inline bool operator==(const IteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			inline bool operator!=(const IteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			inline T* GetData() const
			{
				return &(m_pContent->m_KeyValuePair);
			}

		private:
			using InternalPointerType = typename std::conditional_t<std::is_const_v<T>, const typename Detail::RedBlackTree<K, V, LessComparer, Allocator>::Node*, typename Detail::RedBlackTree<K, V, LessComparer, Allocator>::Node*>;
			using InternalRedBlackTreePointerType = typename std::conditional_t<std::is_const_v<T>, const Detail::RedBlackTree<K, V, LessComparer, Allocator>*, Detail::RedBlackTree<K, V, LessComparer, Allocator>*>;
			inline IteratorImpl(InternalPointerType ptr, InternalRedBlackTreePointerType ptree)
			{
				SGE_ASSERT(NullPointerError, ptr);
				SGE_ASSERT(NullPointerError, ptree);
				m_pContent = ptr;
				m_pTree = ptree;
			}

		private:
			InternalPointerType m_pContent;
			InternalRedBlackTreePointerType m_pTree;
		};

		template<typename T>
		class ReverseIteratorImpl
		{
		public:
			using ValueType = T;

			friend class Map<K, V, LessComparer, Allocator>;

			struct OutOfRangeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter)
				{
					return iter.m_pContent == &(iter.m_pTree->m_NilNode);
				}
			};

		public:
			inline static ReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				if (m.m_Tree.m_Size)
					return ReverseIteratorImpl(m.m_Tree.GetMaximumNode(m.m_Tree.m_pRoot), &(m.m_Tree));
				else
					return ReverseIteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline static ReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				return ReverseIteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline ReverseIteratorImpl(const ReverseIteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
			}

			inline ReverseIteratorImpl& operator=(const ReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
				return *this;
			}

			inline ReverseIteratorImpl& operator++()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator++(int)
			{
				ReverseIteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline ReverseIteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator++();
				}
				return *this;
			}

			inline ReverseIteratorImpl operator+(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				re += i;
				return re;
			}

			inline ReverseIteratorImpl& operator--()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator--(int)
			{
				ReverseIteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline ReverseIteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator--();
				}
				return *this;
			}

			inline ReverseIteratorImpl operator-(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				re -= i;
				return re;
			}

			inline SizeType operator-(const ReverseIteratorImpl& iter) const
			{
				SizeType re = 0;
				ReverseIteratorImpl i = iter;
				while (i != *this)
				{
					++i;
					re += 1;
				}
				return re;
			}

			inline T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pContent->m_KeyValuePair);
			}

			inline T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pContent->m_KeyValuePair;
			}

			inline bool operator==(const ReverseIteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			inline bool operator!=(const ReverseIteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			inline T* GetData() const
			{
				return &(m_pContent->m_KeyValuePair);
			}

		private:
			using InternalPointerType = typename std::conditional_t<std::is_const_v<T>, const typename Detail::RedBlackTree<K, V, LessComparer, Allocator>::Node*, typename Detail::RedBlackTree<K, V, LessComparer, Allocator>::Node*>;
			using InternalRedBlackTreePointerType = typename std::conditional_t<std::is_const_v<T>, const Detail::RedBlackTree<K, V, LessComparer, Allocator>*, Detail::RedBlackTree<K, V, LessComparer, Allocator>*>;
			inline ReverseIteratorImpl(InternalPointerType ptr, InternalRedBlackTreePointerType ptree)
			{
				SGE_ASSERT(NullPointerError, ptr);
				SGE_ASSERT(NullPointerError, ptree);
				m_pContent = ptr;
				m_pTree = ptree;
			}

		private:
			InternalPointerType m_pContent;
			InternalRedBlackTreePointerType m_pTree;
		};

		using Iterator = IteratorImpl<Pair<const K, V>>;
		using ConstIterator = IteratorImpl<const Pair<const K, V>>;
		using ReverseIterator = ReverseIteratorImpl<Pair<const K, V>>;
		using ConstReverseIterator = ReverseIteratorImpl<const Pair<const K, V>>;

		template<typename IteratorType>
		struct IsMapIterator
		{
			inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator> || std::is_same_v<IteratorType, ReverseIterator> || std::is_same_v<IteratorType, ConstReverseIterator>;
		};

		struct ExternalIteratorError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator does not belong to this Map.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const Map& m)
			{
				return iter.m_pTree != &(m.m_Tree);
			}
		};

		struct KeyNotFoundError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The key can not be found in this Map.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const IteratorType& end)
			{
				return iter == end;
			}
		};

		struct NilNodeIteratorError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator which pointing to the nil node is invalid.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const IteratorType& end)
			{
				return iter == end;
			}
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

		template<typename K2, typename V2>
		inline Pair<Iterator, bool> Insert(K2&& key, V2&& val)
		{
			auto re = m_Tree.InternalInsert(std::forward<K2>(key), std::forward<V2>(val));
			return Pair<Iterator, bool>(Iterator(re.m_First, &m_Tree), re.m_Second);
		}

		inline void Insert(std::initializer_list<Pair<const K, V>> ilist)
		{
			for (auto i = ilist.begin(); i != ilist.end(); ++i)
			{
				m_Tree.InternalInsert(i->m_First, i->m_Second);
			}
		}

		template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
		inline IteratorType Remove(const IteratorType& iter)
		{
			SGE_ASSERT(ExternalIteratorError, iter, *this);
			SGE_ASSERT(NilNodeIteratorError, iter, IteratorType::GetEnd(*this));
			IteratorType re = iter;
			++re;
			m_Tree.RemoveNode((typename Detail::RedBlackTree<K, V, LessComparer, Allocator>::Node*)iter.m_pContent);
			return re;
		}

		inline bool RemoveByKey(const K& key)
		{
			return m_Tree.RemoveByKey(key);
		}

		inline Iterator Find(const K& key)
		{
			return Iterator(m_Tree.FindNode(key), &m_Tree);
		}

		inline ConstIterator Find(const K& key) const
		{
			return ConstIterator(m_Tree.FindNode(key), &m_Tree);
		}

		inline V& Get(const K& key)
		{
			Iterator iter = Find(key);
			SGE_CHECK(KeyNotFoundError, iter, GetEnd());
			return iter->m_Second;
		}

		inline const V& Get(const K& key) const
		{
			ConstIterator iter = Find(key);
			SGE_CHECK(KeyNotFoundError, iter, GetConstEnd());
			return iter->m_Second;
		}

		template<typename K2>
		inline V& operator[](K2&& key)
		{
			Iterator iter = Find(key);
			if (iter == GetEnd())
				iter = Insert(std::forward<K2>(key), V()).m_First;
			return iter->m_Second;
		}

		inline bool operator==(const Map& map) const
		{
			if (GetSize() != map.GetSize())
				return false;

			auto iter = GetConstBegin();
			auto oiter = map.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return false;
				++iter;
				++oiter;
			}

			return true;
		}

		template<typename OtherLessComparer, typename OtherAllocator>
		inline bool operator==(const Map<K, V, OtherLessComparer, OtherAllocator>& map) const
		{
			if (GetSize() != map.GetSize())
				return false;

			auto iter = GetConstBegin();
			auto oiter = map.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return false;
				++iter;
				++oiter;
			}

			return true;
		}

		inline bool operator!=(const Map& map) const
		{
			if (GetSize() != map.GetSize())
				return true;

			auto iter = GetConstBegin();
			auto oiter = map.GetConstBegin();
			while (iter != GetConstEnd())
			{
				if (*iter != *oiter)
					return true;
				++iter;
				++oiter;
			}

			return false;
		}

		template<typename OtherLessComparer, typename OtherAllocator>
		inline bool operator!=(const Map<K, V, OtherLessComparer, OtherAllocator>& map) const
		{
			if (GetSize() != map.GetSize())
				return true;

			auto iter = GetConstBegin();
			auto oiter = map.GetConstBegin();
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
		Detail::RedBlackTree<K, V, LessComparer, Allocator> m_Tree;
	};

}

/*!
@}
*/