/*
Copyright 2026 creatorlxd

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
	template<typename K, typename V, typename LessComparer = Less<K>, IsAllocator Allocator = DefaultAllocator>
	class Map : public Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>
	{
	public:
		using KeyType = const K;
		using ValueType = V;
		using LessComparerType = LessComparer;
		using AllocatorType = Allocator;

	public:
		template<typename... Args>
		inline Map(Args&&... args)
			: Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>(std::forward<Args>(args)...)
		{
		}

		template<IsPair P = Pair<K, V>>	   // not Pair<const K, V> to make key movable
		inline Map(std::initializer_list<P> ilist)
			: Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>(ilist)
		{
		}

		template<typename Arg>
		inline Map& operator=(Arg&& arg)
		{
			Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::operator=(std::forward<Arg>(arg));

			return *this;
		}

		using Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Insert;

		using Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Upsert;

		template<typename K2, typename V2>
		inline Pair<typename Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Iterator,
					bool>
		Insert(K2&& key, V2&& val)
		{
			return Insert(MakePair(std::forward<K2>(key), std::forward<V2>(val)));
		}

		template<typename K2, typename V2>
		inline Pair<typename Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Iterator,
					bool>
		Upsert(K2&& key, V2&& val)
		{
			return Upsert(MakePair(std::forward<K2>(key), std::forward<V2>(val)));
		}

		inline bool RemoveByKey(const K& key)
		{
			return Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::RemoveByValue(key);
		}

		template<typename K2>
		inline V& Get(const K2& key)
		{
			return Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Get(key)
				.m_Second;
		}

		template<typename K2>
		inline const V& Get(const K2& key) const
		{
			return Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Get(key)
				.m_Second;
		}

		template<typename K2>
		inline V& operator[](K2&& key)
		{
			typename Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Iterator iter = Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::Find(key);
			if (iter == Detail::RedBlackTree<Detail::KeyValuePair<K, V>, KeyComparer<LessComparer>, KeyComparer<Equal<K>>, Allocator>::GetEnd())
				iter = Insert(std::forward<K2>(key), V()).m_First;
			return iter->m_Second;
		}

	private:
		inline void RemoveByValue()	   // used to ban the red_black_tree's RemoveByValue
		{
		}
	};
}

/*!
@}
*/