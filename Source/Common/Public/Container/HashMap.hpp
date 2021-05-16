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

				inline Node()
					: m_HashValue(0), m_KeyValuePair(K(), V()), m_pNext(nullptr)
				{
				}
				template<typename K2, typename V2>
				inline Node(K2&& key, V2&& val)
					: m_HashValue(0), m_KeyValuePair(std::forward<K2>(key), std::forward<V2>(val)), m_pNext(nullptr)
				{
				}

				template<typename P>
				inline explicit Node(P&& p)
					: m_HashValue(0), m_KeyValuePair(std::forward<P>(p)), m_pNext(nullptr)
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

			inline Node* Find(HashType hash_value, const K& key)
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

			inline Node* Find(HashType hash_value, const K& key) const
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
				p = Allocator::template New(std::forward<K2>(key), std::forward<V2>(val));
				p->m_HashValue = hash_value;
				p->m_pNext = m_pHead;
				m_pHead = p;
				return Pair<Node*, bool>(p, true);
			}

		private:
			Node* m_pHead;
		};

	public:
		inline HashMap()
			: m_LoadFactor(sm_DefaultLoadFactor), m_BucketQuantity(sm_DefaultBucketQuantity), m_pContent((Bucket*)Allocator::RawNew(m_BucketQuantity * sizeof(Bucket), alignof(Bucket))), m_Size(0)
		{
		}

		inline ~HashMap()
		{
			Allocator::RawDelete(m_pContent, m_BucketQuantity * sizeof(Bucket), alignof(Bucket));
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