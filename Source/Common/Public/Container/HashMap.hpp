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
#include "MemoryManager.h"
#include "Utility/Utility.hpp"
#include "Map.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	using HashType = UInt64;

	template<typename T>
	struct Hash
	{
		inline static HashType GetHash(const T& val)
		{
			return std::hash<T>()(val);
		}
	};

	template<typename K, typename V, typename Hasher = Hash<K>, typename Allocator = DefaultAllocator>
	class HashMap
	{
	public:
		using KeyType = const K;
		using ValueType = V;
		using HasherType = Hasher;
		using AllocatorType = Allocator;

		inline static constexpr const float sm_DefaultLoadFactor = 1.0f;
		inline static constexpr const SizeType sm_DefaultBucketQuantity = 16;

	private:
		class Bucket
		{
		public:
			struct Node
			{
				HashType m_HashValue;
				Pair<const K, V> m_KeyValuePair;
				Node* m_pNext;
				Node* m_pPrevious;

				inline Node()
					: m_HashValue(0), m_KeyValuePair(K(), V()), m_pNext(nullptr), m_pPrevious(nullptr)
				{
				}
				template<typename K2, typename V2>
				inline Node(K2&& key, V2&& val)
					: m_HashValue(0), m_KeyValuePair(std::forward<K2>(key), std::forward<V2>(val)), m_pNext(nullptr), m_pPrevious(nullptr)
				{
				}

				template<typename P>
				inline explicit Node(P&& p)
					: m_HashValue(0), m_KeyValuePair(std::forward<P>(p)), m_pNext(nullptr), m_pPrevious(nullptr)
				{
				}
			};

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
					m_pHead = Allocator::template New<Node>(pn->m_KeyValuePair);
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(pn->m_KeyValuePair);
						pb->m_HashValue = pn->m_HashValue;
						pb->m_pNext = m_pHead;
						m_pHead->m_pPrevious = pb;
						m_pHead = pb;
						pn = pn->m_pNext;
					}
				}
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
					m_pHead = Allocator::template New<Node>(pn->m_KeyValuePair);
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(pn->m_KeyValuePair);
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

			template<typename OtherAllocator>
			inline Bucket(const typename HashMap<K, V, Hasher, OtherAllocator>::Bucket& b)
			{
				const typename HashMap<K, V, Hasher, OtherAllocator>::Bucket::Node* pn = b.m_pHead;
				if (pn != nullptr)
				{
					m_pHead = Allocator::template New<Node>(pn->m_KeyValuePair);
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(pn->m_KeyValuePair);
						pb->m_HashValue = pn->m_HashValue;
						pb->m_pNext = m_pHead;
						m_pHead->m_pPrevious = pb;
						m_pHead = pb;
						pn = pn->m_pNext;
					}
				}
			}

			template<typename OtherAllocator>
			inline Bucket(typename HashMap<K, V, Hasher, OtherAllocator>::Bucket&& b)
			{
				typename HashMap<K, V, Hasher, OtherAllocator>::Bucket::Node* pn = b.m_pHead;
				if (pn != nullptr)
				{
					m_pHead = Allocator::template New<Node>(std::move(pn->m_KeyValuePair));
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(std::move(pn->m_KeyValuePair));
						pb->m_HashValue = pn->m_HashValue;
						pb->m_pNext = m_pHead;
						m_pHead->m_pPrevious = pb;
						m_pHead = pb;
						pn = pn->m_pNext;
					}
				}
			}

			template<typename OtherAllocator>
			inline Bucket& operator=(const typename HashMap<K, V, Hasher, OtherAllocator>::Bucket& b)
			{
				Clear();
				const typename HashMap<K, V, Hasher, OtherAllocator>::Bucket::Node* pn = b.m_pHead;
				if (pn != nullptr)
				{
					m_pHead = Allocator::template New<Node>(pn->m_KeyValuePair);
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(pn->m_KeyValuePair);
						pb->m_HashValue = pn->m_HashValue;
						pb->m_pNext = m_pHead;
						m_pHead->m_pPrevious = pb;
						m_pHead = pb;
						pn = pn->m_pNext;
					}
				}
				return *this;
			}

			template<typename OtherAllocator>
			inline Bucket& operator=(typename HashMap<K, V, Hasher, OtherAllocator>::Bucket&& b)
			{
				Clear();
				typename HashMap<K, V, Hasher, OtherAllocator>::Bucket::Node* pn = b.m_pHead;
				if (pn != nullptr)
				{
					m_pHead = Allocator::template New<Node>(std::move(pn->m_KeyValuePair));
					m_pHead->m_HashValue = pn->m_HashValue;
					pn = pn->m_pNext;
					while (pn != nullptr)
					{
						Node* pb = Allocator::template New<Node>(std::move(pn->m_KeyValuePair));
						pb->m_HashValue = pn->m_HashValue;
						pb->m_pNext = m_pHead;
						m_pHead->m_pPrevious = pb;
						m_pHead = pb;
						pn = pn->m_pNext;
					}
				}
				return *this;
			}

			inline Node* FindNode(HashType hash_value, const K& key)
			{
				Node* pnow = m_pHead;
				while (pnow != nullptr)
				{
					if (pnow->m_HashValue == hash_value && pnow->m_KeyValuePair.m_First == key)
						return pnow;
					pnow = pnow->m_pNext;
				}
				return nullptr;
			}

			inline const Node* FindNode(HashType hash_value, const K& key) const
			{
				const Node* pnow = m_pHead;
				while (pnow != nullptr)
				{
					if (pnow->m_HashValue == hash_value && pnow->m_KeyValuePair.m_First == key)
						return pnow;
					pnow = pnow->m_pNext;
				}
				return nullptr;
			}

			template<typename K2, typename V2>
			inline Pair<Node*, bool> Insert(HashType hash_value, K2&& key, V2&& val)
			{
				Node* p = m_pHead;
				while (p != nullptr)
				{
					if (p->m_HashValue == hash_value && p->m_KeyValuePair.m_First == key)
					{
						p->m_KeyValuePair.m_Second = std::forward<V2>(val);
						return Pair<Node*, bool>(p, false);
					}
					p = p->m_pNext;
				}
				p = Allocator::template New<Node>(std::forward<K2>(key), std::forward<V2>(val));
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

			inline bool RemoveByKey(HashType hash_value, const K& key)
			{
				Node* p = FindNode(hash_value, key);
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
				SGE_ASSERT(InvalidSizeError, nbs_size, 1, SGE_MAX_MEMORY_SIZE / sizeof(Bucket));
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

		struct ZeroLoadFactorError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The load factor can not be zero.");

			inline static bool Judge(float load_factor)
			{
				return load_factor == 0;
			}
		};

	public:
		inline HashMap()
			: m_LoadFactor(sm_DefaultLoadFactor), m_BucketQuantity(sm_DefaultBucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(0)
		{
			for (SizeType i = 0; i < m_BucketQuantity; ++i)
				new (m_pContent + i) Bucket();
		}

		inline ~HashMap()
		{
			if (m_pContent)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					m_pContent[i].~Bucket();

				Allocator::RawDelete(m_pContent, m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
			}
		}

		inline HashMap(float load_factor)
			: HashMap()
		{
			SGE_ASSERT(ZeroLoadFactorError, load_factor);
			m_LoadFactor = load_factor;
		}

		inline HashMap(const HashMap& hm)
			: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
		{
			for (SizeType i = 0; i < m_BucketQuantity; ++i)
				new (m_pContent + i) Bucket(*(hm.m_pContent + i));
		}

		inline HashMap(HashMap&& hm)
			: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent(hm.m_pContent), m_Size(hm.m_Size)
		{
			hm.m_pContent = nullptr;
			hm.m_BucketQuantity = 0;
			hm.m_Size = 0;
		}

		inline HashMap& operator=(const HashMap& hm)
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

		inline HashMap& operator=(HashMap&& hm)
		{
			SGE_ASSERT(SelfAssignmentError, this, &hm);
			RawClear();

			m_LoadFactor = hm.m_LoadFactor;
			m_BucketQuantity = hm.m_BucketQuantity;
			m_pContent = hm.m_pContent;
			m_Size = hm.m_Size;

			hm.m_pContent = nullptr;
			hm.m_BucketQuantity = 0;
			hm.m_Size = 0;

			return *this;
		}

		template<typename OtherAllocator>
		inline HashMap(const HashMap<K, V, Hasher, OtherAllocator>& hm)
			: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
		{
			for (SizeType i = 0; i < m_BucketQuantity; ++i)
				new (m_pContent + i) Bucket(*(hm.m_pContent + i));
		}

		template<typename OtherAllocator>
		inline HashMap(HashMap<K, V, Hasher, OtherAllocator>&& hm)
			: m_LoadFactor(hm.m_LoadFactor), m_BucketQuantity(hm.m_BucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(hm.m_Size)
		{
			for (SizeType i = 0; i < m_BucketQuantity; ++i)
				new (m_pContent + i) Bucket(std::move(*(hm.m_pContent + i)));
		}

		template<typename OtherAllocator>
		inline HashMap& operator=(const HashMap<K, V, Hasher, OtherAllocator>& hm)
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
		inline HashMap& operator=(HashMap<K, V, Hasher, OtherAllocator>&& hm)
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

	private:
		inline void RawClear()
		{
			if (m_pContent)
			{
				for (SizeType i = 0; i < m_BucketQuantity; ++i)
					m_pContent[i].~Bucket();

				Allocator::RawDelete(m_pContent, m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
			}
			m_pContent = nullptr;
			m_BucketQuantity = 0;
			m_Size = 0;
		}

	private:
		float m_LoadFactor;
		SizeType m_BucketQuantity;
		Bucket* m_pContent;
		SizeType m_Size;
	};

	/*!
	@}
	*/
}