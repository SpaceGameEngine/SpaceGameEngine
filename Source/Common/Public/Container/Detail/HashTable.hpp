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
#include <initializer_list>
#include "MemoryManager.h"
#include "Utility/Utility.hpp"
#include "Utility/Hash.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	namespace Detail
	{
		template<typename V, typename Hasher = Hash<V>, typename EqualComparer = Equal<V>, typename Allocator = DefaultAllocator>
		class HashTable
		{
		public:
			using ValueType = V;
			using HasherType = Hasher;
			using EqualComparerType = EqualComparer;
			using AllocatorType = Allocator;

			template<typename _V, typename _Hasher, typename _EqualComparer, typename _Allocator>
			friend class HashTable;

			inline static constexpr const double sm_DefaultLoadFactor = 1.0;
			inline static constexpr const SizeType sm_DefaultBucketQuantity = 16;

		private:
			class Bucket
			{
			public:
				struct Node
				{
					HashType m_HashValue;
					ValueType m_Value;
					Node* m_pNext;
					Node* m_pPrevious;

					template<typename V2>
					inline explicit Node(V2&& val)
						: m_HashValue(0), m_Value(std::forward<V2>(val)), m_pNext(nullptr), m_pPrevious(nullptr)
					{
					}
				};

				template<typename T>
				friend class IteratorImpl;

				template<typename _V, typename _Hasher, typename _EqualComparer, typename _Allocator>
				friend class HashTable;

			public:
				inline Bucket()
					: m_pHead(nullptr)
				{
				}

				inline ~Bucket()
				{
					Node* pnow = m_pHead;
					while (pnow != nullptr)
					{
						Node* pb = pnow;
						pnow = pnow->m_pNext;
						Allocator::template Delete(pb);
					}
				}

				inline void Clear()
				{
					Node* pnow = m_pHead;
					while (pnow != nullptr)
					{
						Node* pb = pnow;
						pnow = pnow->m_pNext;
						Allocator::template Delete(pb);
					}
					m_pHead = nullptr;
				}

				inline Bucket(const Bucket& b)
				{
					const Node* pn = b.m_pHead;
					if (pn != nullptr)
					{
						m_pHead = Allocator::template New<Node>(pn->m_Value);
						m_pHead->m_HashValue = pn->m_HashValue;
						pn = pn->m_pNext;
						while (pn != nullptr)
						{
							Node* pb = Allocator::template New<Node>(pn->m_Value);
							pb->m_HashValue = pn->m_HashValue;
							pb->m_pNext = m_pHead;
							m_pHead->m_pPrevious = pb;
							m_pHead = pb;
							pn = pn->m_pNext;
						}
					}
					else
						m_pHead = nullptr;
				}

				inline Bucket(Bucket&& b)
					: m_pHead(b.m_pHead)
				{
					b.m_pHead = nullptr;
				}

				inline Bucket& operator=(const Bucket& b)
				{
					SGE_ASSERT(SelfAssignmentError, this, &b);
					Clear();
					const Node* pn = b.m_pHead;
					if (pn != nullptr)
					{
						m_pHead = Allocator::template New<Node>(pn->m_Value);
						m_pHead->m_HashValue = pn->m_HashValue;
						pn = pn->m_pNext;
						while (pn != nullptr)
						{
							Node* pb = Allocator::template New<Node>(pn->m_Value);
							pb->m_HashValue = pn->m_HashValue;
							pb->m_pNext = m_pHead;
							m_pHead->m_pPrevious = pb;
							m_pHead = pb;
							pn = pn->m_pNext;
						}
					}
					return *this;
				}

				inline Bucket& operator=(Bucket&& b)
				{
					SGE_ASSERT(SelfAssignmentError, this, &b);
					Clear();
					m_pHead = b.m_pHead;
					b.m_pHead = nullptr;
					return *this;
				}

				template<typename OtherBucket>
				inline Bucket(OtherBucket&& b)
				{
					if constexpr (!std::is_same_v<std::remove_reference_t<OtherBucket>, OtherBucket>)
					{
						const typename RemoveCVRef<OtherBucket>::Type::Node* pn = b.m_pHead;
						if (pn != nullptr)
						{
							m_pHead = Allocator::template New<Node>(pn->m_Value);
							m_pHead->m_HashValue = pn->m_HashValue;
							pn = pn->m_pNext;
							while (pn != nullptr)
							{
								Node* pb = Allocator::template New<Node>(pn->m_Value);
								pb->m_HashValue = pn->m_HashValue;
								pb->m_pNext = m_pHead;
								m_pHead->m_pPrevious = pb;
								m_pHead = pb;
								pn = pn->m_pNext;
							}
						}
						else
							m_pHead = nullptr;
					}
					else
					{
						typename RemoveCVRef<OtherBucket>::Type::Node* pn = b.m_pHead;
						if (pn != nullptr)
						{
							m_pHead = Allocator::template New<Node>(std::move(pn->m_Value));
							m_pHead->m_HashValue = pn->m_HashValue;
							pn = pn->m_pNext;
							while (pn != nullptr)
							{
								Node* pb = Allocator::template New<Node>(std::move(pn->m_Value));
								pb->m_HashValue = pn->m_HashValue;
								pb->m_pNext = m_pHead;
								m_pHead->m_pPrevious = pb;
								m_pHead = pb;
								pn = pn->m_pNext;
							}
						}
						else
							m_pHead = nullptr;
					}
				}

				template<typename OtherBucket>
				inline Bucket& operator=(OtherBucket&& b)
				{
					Clear();
					if constexpr (!std::is_same_v<std::remove_reference_t<OtherBucket>, OtherBucket>)
					{
						const typename RemoveCVRef<OtherBucket>::Type::Node* pn = b.m_pHead;
						if (pn != nullptr)
						{
							m_pHead = Allocator::template New<Node>(pn->m_Value);
							m_pHead->m_HashValue = pn->m_HashValue;
							pn = pn->m_pNext;
							while (pn != nullptr)
							{
								Node* pb = Allocator::template New<Node>(pn->m_Value);
								pb->m_HashValue = pn->m_HashValue;
								pb->m_pNext = m_pHead;
								m_pHead->m_pPrevious = pb;
								m_pHead = pb;
								pn = pn->m_pNext;
							}
						}
					}
					else
					{
						typename RemoveCVRef<OtherBucket>::Type::Node* pn = b.m_pHead;
						if (pn != nullptr)
						{
							m_pHead = Allocator::template New<Node>(std::move(pn->m_Value));
							m_pHead->m_HashValue = pn->m_HashValue;
							pn = pn->m_pNext;
							while (pn != nullptr)
							{
								Node* pb = Allocator::template New<Node>(std::move(pn->m_Value));
								pb->m_HashValue = pn->m_HashValue;
								pb->m_pNext = m_pHead;
								m_pHead->m_pPrevious = pb;
								m_pHead = pb;
								pn = pn->m_pNext;
							}
						}
					}
					return *this;
				}

				template<typename V2>
				inline Node* FindNode(HashType hash_value, const V2& val)
				{
					Node* pnow = m_pHead;
					while (pnow != nullptr)
					{
						if (pnow->m_HashValue == hash_value && EqualComparer::Compare(pnow->m_Value, val))
							return pnow;
						pnow = pnow->m_pNext;
					}
					return nullptr;
				}

				template<typename V2>
				inline const Node* FindNode(HashType hash_value, const V2& val) const
				{
					const Node* pnow = m_pHead;
					while (pnow != nullptr)
					{
						if (pnow->m_HashValue == hash_value && EqualComparer::Compare(pnow->m_Value, val))
							return pnow;
						pnow = pnow->m_pNext;
					}
					return nullptr;
				}

				template<typename V2>
				inline Pair<Node*, bool> Insert(HashType hash_value, V2&& val)
				{
					Node* p = m_pHead;
					while (p != nullptr)
					{
						if (p->m_HashValue == hash_value && EqualComparer::Compare(p->m_Value, val))
						{
							return Pair<Node*, bool>(p, false);
						}
						p = p->m_pNext;
					}
					p = Allocator::template New<Node>(std::forward<V2>(val));
					p->m_HashValue = hash_value;
					p->m_pNext = m_pHead;
					if (m_pHead)
						m_pHead->m_pPrevious = p;
					m_pHead = p;
					return Pair<Node*, bool>(p, true);
				}

				template<typename V2>
				inline Pair<Node*, bool> Upsert(HashType hash_value, V2&& val)
				{
					Node* p = m_pHead;
					while (p != nullptr)
					{
						if (p->m_HashValue == hash_value && EqualComparer::Compare(p->m_Value, val))
						{
							p->m_Value = std::forward<V2>(val);
							return Pair<Node*, bool>(p, false);
						}
						p = p->m_pNext;
					}
					p = Allocator::template New<Node>(std::forward<V2>(val));
					p->m_HashValue = hash_value;
					p->m_pNext = m_pHead;
					if (m_pHead)
						m_pHead->m_pPrevious = p;
					m_pHead = p;
					return Pair<Node*, bool>(p, true);
				}

				inline void RemoveNode(Node* p)
				{
					SGE_ASSERT(NullPointerError, p);
					if (p->m_pPrevious == nullptr)
					{
						m_pHead = p->m_pNext;
						if (m_pHead)
						{
							m_pHead->m_pPrevious = nullptr;
						}
					}
					else if (p->m_pNext == nullptr)
					{
						p->m_pPrevious->m_pNext = nullptr;
					}
					else
					{
						p->m_pPrevious->m_pNext = p->m_pNext;
						p->m_pNext->m_pPrevious = p->m_pPrevious;
					}
					Allocator::Delete(p);
				}

				template<typename V2>
				inline bool Remove(HashType hash_value, const V2& val)
				{
					Node* p = FindNode(hash_value, val);
					if (p != nullptr)
					{
						RemoveNode(p);
						return true;
					}
					else
						return false;
				}

				inline void Rehash(Bucket* nbs, SizeType nbs_size)
				{
					SGE_ASSERT(NullPointerError, nbs);
					SGE_ASSERT(InvalidValueError, nbs_size, 1, SGE_MAX_MEMORY_SIZE / sizeof(Bucket));
					Node* pnow = m_pHead;
					while (pnow != nullptr)
					{
						SizeType nidx = pnow->m_HashValue & (nbs_size - 1);
						Node* pb = pnow;
						pnow = pnow->m_pNext;

						if (nbs[nidx].m_pHead == nullptr)
						{
							nbs[nidx].m_pHead = pb;
							pb->m_pPrevious = nullptr;
							pb->m_pNext = nullptr;
						}
						else
						{
							pb->m_pNext = nbs[nidx].m_pHead;
							nbs[nidx].m_pHead->m_pPrevious = pb;
							pb->m_pPrevious = nullptr;
							nbs[nidx].m_pHead = pb;
						}
					}
					m_pHead = nullptr;
				}

			private:
				Node* m_pHead;
			};

			inline static constexpr const SizeType sm_MaxSize = SGE_MAX_MEMORY_SIZE / sizeof(typename Bucket::Node);
			inline static constexpr const SizeType sm_MaxBucketQuantity = SGE_MAX_MEMORY_SIZE / sizeof(Bucket);

			struct ZeroLoadFactorError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The load factor can not be zero.");

				inline static bool Judge(double load_factor)
				{
					return load_factor == 0;
				}
			};

			template<typename T>
			class IteratorImpl;

		public:
			inline HashTable()
				: m_LoadFactor(sm_DefaultLoadFactor), m_BucketQuantity(sm_DefaultBucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(0)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket();
			}

			inline ~HashTable()
			{
				if (m_pContent)
				{
					for (SizeType i = 0; i < m_BucketQuantity; ++i)
						m_pContent[i].~Bucket();

					Allocator::RawDelete(m_pContent);
				}
			}

			inline void Clear()
			{
				if (m_pContent)
				{
					for (SizeType i = 0; i < m_BucketQuantity; ++i)
						m_pContent[i].~Bucket();

					Allocator::RawDelete(m_pContent);
				}

				m_LoadFactor = sm_DefaultLoadFactor;
				m_BucketQuantity = sm_DefaultBucketQuantity;
				m_pContent = (Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
				m_Size = 0;

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket();
			}

			inline HashTable(double load_factor)
				: HashTable()
			{
				SGE_ASSERT(ZeroLoadFactorError, load_factor);
				m_LoadFactor = load_factor;
			}

			inline HashTable(std::initializer_list<V> ilist)
				: m_LoadFactor(sm_DefaultLoadFactor), m_BucketQuantity(GetCorrectBucketQuantity(m_LoadFactor, ilist.size())), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(ilist.size())
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket();

				for (auto i = ilist.begin(); i != ilist.end(); ++i)
				{
					HashType hash = Hasher::GetHash(*i);
					m_pContent[hash & (m_BucketQuantity - 1)].Insert(hash, *i);
				}
			}

			inline HashTable(const HashTable& hm)
				: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(*(hm.m_pContent + i));
			}

			inline HashTable(HashTable&& hm)
				: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent(hm.m_pContent), m_Size(hm.m_Size)
			{
				hm.m_pContent = nullptr;
			}

			inline HashTable& operator=(const HashTable& hm)
			{
				SGE_ASSERT(SelfAssignmentError, this, &hm);
				RawClear();

				m_LoadFactor = hm.m_LoadFactor;
				m_BucketQuantity = hm.m_BucketQuantity;
				m_pContent = (Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
				m_Size = hm.m_Size;

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(*(hm.m_pContent + i));

				return *this;
			}

			inline HashTable& operator=(HashTable&& hm)
			{
				SGE_ASSERT(SelfAssignmentError, this, &hm);
				RawClear();

				m_LoadFactor = hm.m_LoadFactor;
				m_BucketQuantity = hm.m_BucketQuantity;
				m_pContent = hm.m_pContent;
				m_Size = hm.m_Size;

				hm.m_pContent = nullptr;

				return *this;
			}

			template<typename OtherAllocator>
			inline HashTable(const HashTable<V, Hasher, EqualComparer, OtherAllocator>& hm)
				: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(*(hm.m_pContent + i));
			}

			template<typename OtherAllocator>
			inline HashTable(HashTable<V, Hasher, EqualComparer, OtherAllocator>&& hm)
				: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(std::move(*(hm.m_pContent + i)));
			}

			template<typename OtherAllocator>
			inline HashTable& operator=(const HashTable<V, Hasher, EqualComparer, OtherAllocator>& hm)
			{
				RawClear();

				m_LoadFactor = hm.m_LoadFactor;
				m_BucketQuantity = hm.m_BucketQuantity;
				m_pContent = (Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
				m_Size = hm.m_Size;

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(*(hm.m_pContent + i));

				return *this;
			}

			template<typename OtherAllocator>
			inline HashTable& operator=(HashTable<V, Hasher, EqualComparer, OtherAllocator>&& hm)
			{
				RawClear();

				m_LoadFactor = hm.m_LoadFactor;
				m_BucketQuantity = hm.m_BucketQuantity;
				m_pContent = (Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
				m_Size = hm.m_Size;

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					new (m_pContent + i) Bucket(std::move(*(hm.m_pContent + i)));

				return *this;
			}

			using Iterator = IteratorImpl<V>;
			using ConstIterator = IteratorImpl<const V>;

			template<typename IteratorType>
			struct IsHashTableIterator
			{
				inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>;
			};

			struct ExternalIteratorError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator does not belong to this HashTable.");

				template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value, void>>
				inline static bool Judge(const IteratorType& iter, const HashTable& hm)
				{
					return iter.m_pBucketEnd != hm.m_pContent + hm.m_BucketQuantity;
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

			inline static SizeType GetCorrectBucketQuantity(double load_factor, SizeType size)
			{
				SGE_ASSERT(ZeroLoadFactorError, load_factor);
				SGE_ASSERT(InvalidValueError, size, 0, sm_MaxSize);
				SizeType buf = (SizeType)std::round((double)size / load_factor);
				if (buf <= 1)
					return 1;
				else
				{
					SizeType re = 1;
					SizeType buf2 = (buf << 1) - 1;
					while (buf2 != 1)
					{
						buf2 >>= 1;
						re <<= 1;
					}
					return re;
				}
			}

			inline double GetLoadFactor() const
			{
				return m_LoadFactor;
			}

			inline void SetLoadFactor(double load_factor)
			{
				SGE_ASSERT(ZeroLoadFactorError, load_factor);
				m_LoadFactor = load_factor;
				Rehash(GetCorrectBucketQuantity(m_LoadFactor, m_Size));
			}

			inline SizeType GetSize() const
			{
				return m_Size;
			}

			inline SizeType GetBucketQuantity() const
			{
				return m_BucketQuantity;
			}

			template<typename V2>
			inline Pair<Iterator, bool> Insert(V2&& val)
			{
				SizeType new_bucket_size = GetCorrectBucketQuantity(m_LoadFactor, m_Size + 1);
				if (m_BucketQuantity < new_bucket_size)
					Rehash(new_bucket_size);

				HashType hash = Hasher::GetHash(val);
				SizeType idx = hash & (m_BucketQuantity - 1);
				auto re = m_pContent[idx].Insert(hash, std::forward<V2>(val));
				if (re.m_Second)
					m_Size += 1;
				return Pair<Iterator, bool>(Iterator(m_pContent + idx, re.m_First, m_pContent + m_BucketQuantity), re.m_Second);
			}

			template<typename V2>
			inline Pair<Iterator, bool> Upsert(V2&& val)
			{
				SizeType new_bucket_size = GetCorrectBucketQuantity(m_LoadFactor, m_Size + 1);
				if (m_BucketQuantity < new_bucket_size)
					Rehash(new_bucket_size);

				HashType hash = Hasher::GetHash(val);
				SizeType idx = hash & (m_BucketQuantity - 1);
				auto re = m_pContent[idx].Upsert(hash, std::forward<V2>(val));
				if (re.m_Second)
					m_Size += 1;
				return Pair<Iterator, bool>(Iterator(m_pContent + idx, re.m_First, m_pContent + m_BucketQuantity), re.m_Second);
			}

			template<typename V2>
			inline void Insert(std::initializer_list<V2> ilist)
			{
				SizeType new_bucket_size = GetCorrectBucketQuantity(m_LoadFactor, m_Size + ilist.size());
				if (m_BucketQuantity < new_bucket_size)
					Rehash(new_bucket_size);

				for (auto i = ilist.begin(); i != ilist.end(); ++i)
				{
					HashType hash = Hasher::GetHash(*i);
					if (m_pContent[hash & (m_BucketQuantity - 1)].Insert(hash, *i).m_Second)
						m_Size += 1;
				}
			}

			template<typename V2>
			inline void Upsert(std::initializer_list<V2> ilist)
			{
				SizeType new_bucket_size = GetCorrectBucketQuantity(m_LoadFactor, m_Size + ilist.size());
				if (m_BucketQuantity < new_bucket_size)
					Rehash(new_bucket_size);

				for (auto i = ilist.begin(); i != ilist.end(); ++i)
				{
					HashType hash = Hasher::GetHash(*i);
					if (m_pContent[hash & (m_BucketQuantity - 1)].Upsert(hash, *i).m_Second)
						m_Size += 1;
				}
			}

			template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value, void>>
			inline IteratorType Remove(const IteratorType& iter)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter);
				SGE_ASSERT(ExternalIteratorError, iter, *this);
				IteratorType re = iter;
				++re;
				m_pContent[(iter.m_pNode->m_HashValue) & (m_BucketQuantity - 1)].RemoveNode(iter.m_pNode);
				m_Size -= 1;
				return re;
			}

			template<typename V2>
			inline bool RemoveByKey(const V2& val)
			{
				HashType hash = Hasher::GetHash(val);
				m_Size -= 1;
				return m_pContent[hash & (m_BucketQuantity - 1)].Remove(hash, val);
			}

			inline void ShrinkToFit()
			{
				SizeType correct_bucket_quantity = GetCorrectBucketQuantity(m_LoadFactor, m_Size);
				if (m_BucketQuantity > correct_bucket_quantity)
					Rehash(correct_bucket_quantity);
			}

			template<typename V2>
			inline Iterator Find(const V2& val)
			{
				HashType hash = Hasher::GetHash(val);
				SizeType idx = hash & (m_BucketQuantity - 1);
				auto pnode = m_pContent[idx].FindNode(hash, val);
				if (pnode)
					return Iterator(m_pContent + idx, pnode, m_pContent + m_BucketQuantity);
				else
					return GetEnd();
			}

			template<typename V2>
			inline ConstIterator Find(const V2& val) const
			{
				HashType hash = Hasher::GetHash(val);
				SizeType idx = hash & (m_BucketQuantity - 1);
				auto pnode = m_pContent[idx].FindNode(hash, val);
				if (pnode)
					return ConstIterator(m_pContent + idx, pnode, m_pContent + m_BucketQuantity);
				else
					return GetConstEnd();
			}

			template<typename V2>
			inline V& Get(const V2& val)
			{
				Iterator iter = Find(val);
				SGE_CHECK(typename Iterator::OutOfRangeError, iter);
				return *iter;
			}

			template<typename V2>
			inline const V& Get(const V2& val) const
			{
				ConstIterator iter = Find(val);
				SGE_CHECK(typename ConstIterator::OutOfRangeError, iter);
				return *iter;
			}

			template<typename V2>
			inline V& operator[](V2&& val)
			{
				Iterator iter = Find(val);
				if (iter == GetEnd())
					iter = Insert(std::forward<V2>(val)).m_First;
				return *iter;
			}

			inline void Rehash(SizeType new_bucket_quantity)
			{
				SGE_ASSERT(InvalidValueError, new_bucket_quantity, 1, sm_MaxBucketQuantity);

				Bucket* pbuf = (Bucket*)Allocator::RawNew(new_bucket_quantity * sizeof(Bucket), alignof(Bucket));
				for (SizeType i = 0; i < new_bucket_quantity; ++i)
					new (pbuf + i) Bucket();

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					m_pContent[i].Rehash(pbuf, new_bucket_quantity);

				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					m_pContent[i].~Bucket();

				Allocator::RawDelete(m_pContent);

				m_pContent = pbuf;
				m_BucketQuantity = new_bucket_quantity;
			}

			inline bool operator==(const HashTable& hash_table) const
			{
				if (m_Size != hash_table.m_Size)
					return false;

				for (auto iter = GetConstBegin(); iter != GetConstEnd(); ++iter)
				{
					auto fiter = hash_table.Find(*iter);
					if (fiter == hash_table.GetConstEnd())
						return false;
					if (*iter != *fiter)
						return false;
				}

				return true;
			}

			template<typename OtherHasher, typename OtherAllocator>
			inline bool operator==(const HashTable<V, OtherHasher, EqualComparer, OtherAllocator>& hash_table) const
			{
				if (m_Size != hash_table.m_Size)
					return false;

				for (auto iter = GetConstBegin(); iter != GetConstEnd(); ++iter)
				{
					auto fiter = hash_table.Find(*iter);
					if (fiter == hash_table.GetConstEnd())
						return false;
					if (*iter != *fiter)
						return false;
				}

				return true;
			}

			inline bool operator!=(const HashTable& hash_table) const
			{
				if (m_Size != hash_table.m_Size)
					return true;

				for (auto iter = GetConstBegin(); iter != GetConstEnd(); ++iter)
				{
					auto fiter = hash_table.Find(*iter);
					if (fiter == hash_table.GetConstEnd())
						return true;
					if (*iter != *fiter)
						return true;
				}

				return false;
			}

			template<typename OtherHasher, typename OtherAllocator>
			inline bool operator!=(const HashTable<V, OtherHasher, EqualComparer, OtherAllocator>& hash_table) const
			{
				if (m_Size != hash_table.m_Size)
					return true;

				for (auto iter = GetConstBegin(); iter != GetConstEnd(); ++iter)
				{
					auto fiter = hash_table.Find(*iter);
					if (fiter == hash_table.GetConstEnd())
						return true;
					if (*iter != *fiter)
						return true;
				}

				return false;
			}

		private:
			inline void RawClear()
			{
				if (m_pContent)
				{
					for (SizeType i = 0; i < m_BucketQuantity; ++i)
						m_pContent[i].~Bucket();

					Allocator::RawDelete(m_pContent);
				}
			}

			template<typename T>
			class IteratorImpl
			{
			public:
				using ValueType = T;

				friend class HashTable<V, Hasher, EqualComparer, Allocator>;

				struct OutOfRangeError
				{
					inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
					inline static bool Judge(const IteratorImpl& iter)
					{
						return iter.m_pBucket == iter.m_pBucketEnd || iter.m_pNode == nullptr;
					}
				};

			public:
				inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<T>, const HashTable&, HashTable&> ht)
				{
					IteratorImpl re(ht.m_pContent, ht.m_pContent->m_pHead, ht.m_pContent + ht.m_BucketQuantity);

					while (re.m_pNode == nullptr && re.m_pBucket != re.m_pBucketEnd)
					{
						re.m_pBucket += 1;
						re.m_pNode = re.m_pBucket->m_pHead;
					}
					if (re.m_pBucket == re.m_pBucketEnd)
						re.m_pNode = nullptr;

					return re;
				}

				inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<T>, const HashTable&, HashTable&> ht)
				{
					return IteratorImpl(ht.m_pContent + ht.m_BucketQuantity, nullptr, ht.m_pContent + ht.m_BucketQuantity);
				}

				inline IteratorImpl(const IteratorImpl& iter)
					: m_pBucket(iter.m_pBucket), m_pNode(iter.m_pNode), m_pBucketEnd(iter.m_pBucketEnd)
				{
				}

				inline IteratorImpl& operator=(const IteratorImpl& iter)
				{
					SGE_ASSERT(SelfAssignmentError, this, &iter);
					m_pBucket = iter.m_pBucket;
					m_pNode = iter.m_pNode;
					m_pBucketEnd = iter.m_pBucketEnd;
					return *this;
				}

				template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
				inline IteratorImpl(const IteratorType& iter)
					: m_pBucket((BucketPointerType)iter.m_pBucket), m_pNode((NodePointerType)iter.m_pNode), m_pBucketEnd((BucketPointerType)iter.m_pBucketEnd)
				{
				}

				template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
				inline IteratorImpl& operator=(const IteratorType& iter)
				{
					m_pBucket = (BucketPointerType)iter.m_pBucket;
					m_pNode = (NodePointerType)iter.m_pNode;
					m_pBucketEnd = (BucketPointerType)iter.m_pBucketEnd;
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

				inline IteratorImpl& operator+=(SizeType i)
				{
					for (SizeType j = 0; j < i; ++j)
					{
						MoveForward();
					}
					return *this;
				}

				inline IteratorImpl operator+(SizeType i) const
				{
					IteratorImpl re(*this);
					re += i;
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
					return &(m_pNode->m_Value);
				}

				inline T& operator*() const
				{
					SGE_ASSERT(OutOfRangeError, *this);
					return m_pNode->m_Value;
				}

				inline bool operator==(const IteratorImpl& iter) const
				{
					return m_pBucket == iter.m_pBucket && m_pNode == iter.m_pNode && m_pBucketEnd == iter.m_pBucketEnd;
				}

				inline bool operator!=(const IteratorImpl& iter) const
				{
					return m_pBucket != iter.m_pBucket || m_pNode != iter.m_pNode || m_pBucketEnd != iter.m_pBucketEnd;
				}

				template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
				inline bool operator==(const IteratorType& iter) const
				{
					return m_pBucket == iter.m_pBucket && m_pNode == iter.m_pNode && m_pBucketEnd == iter.m_pBucketEnd;
				}

				template<typename IteratorType, typename = std::enable_if_t<IsHashTableIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<std::remove_const_t<typename IteratorType::ValueType>, std::remove_const_t<ValueType>>), void>>
				inline bool operator!=(const IteratorType& iter) const
				{
					return m_pBucket != iter.m_pBucket || m_pNode != iter.m_pNode || m_pBucketEnd != iter.m_pBucketEnd;
				}

				inline T* GetData() const
				{
					SGE_ASSERT(OutOfRangeError, *this);
					return &(m_pNode->m_Value);
				}

			private:
				using BucketPointerType = std::conditional_t<std::is_const_v<T>, const Bucket*, Bucket*>;
				using NodePointerType = std::conditional_t<std::is_const_v<T>, const typename Bucket::Node*, typename Bucket::Node*>;

				inline IteratorImpl(BucketPointerType pbucket, NodePointerType pnode, BucketPointerType pbucket_end)
				{
					SGE_ASSERT(NullPointerError, pbucket);
					// SGE_ASSERT(NullPointerError, pnode);	pnode can be nullptr as the end iterator
					SGE_ASSERT(NullPointerError, pbucket_end);
					m_pBucket = pbucket;
					m_pNode = pnode;
					m_pBucketEnd = pbucket_end;
				}

				inline void MoveForward()
				{
					if (m_pBucket != m_pBucketEnd)
					{
						m_pNode = m_pNode->m_pNext;
						while (m_pNode == nullptr && m_pBucket != m_pBucketEnd)
						{
							m_pBucket += 1;
							m_pNode = m_pBucket->m_pHead;
						}
						if (m_pBucket == m_pBucketEnd)
							m_pNode = nullptr;
					}
				}

			private:
				BucketPointerType m_pBucket;
				NodePointerType m_pNode;
				BucketPointerType m_pBucketEnd;
			};

		private:
			double m_LoadFactor;
			SizeType m_BucketQuantity;
			Bucket* m_pContent;
			SizeType m_Size;
		};
	}
}

/*!
@}
*/