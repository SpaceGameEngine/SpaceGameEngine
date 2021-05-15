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

	template<typename T>
	struct Hash
	{
		inline static SizeType GetHash(const T& val)
		{
			return std::hash<T>()(val);
		}
	};

	template<typename K, typename V, typename Hasher = Hash<K>, typename LessComparer = Less<K>, typename Allocator = DefaultAllocator>
	class HashMap
	{
	public:
		using KeyType = const K;
		using ValueType = V;
		using AllocatorType = Allocator;
		using HasherType = Hasher;
		using LessComparerType = LessComparer;

		inline static constexpr const float sm_DefaultLoadFactor = 1.0f;
		inline static constexpr const SizeType sm_DefaultArraySize = 16;
		inline static constexpr const SizeType sm_RedBlackTreeListSizeBound = 8;
		inline static constexpr const SizeType sm_RedBlackTreeArraySizeBound = 64;

	private:
		struct Bucket
		{
		};

	public:
		inline HashMap()
			: m_LoadFactor(sm_DefaultLoadFactor), m_ArraySize(sm_DefaultArraySize), m_pContent(Allocator::RawNew(m_ArraySize * sizeof(Bucket), alignof(Bucket))), m_Size(0)
		{
		}

		inline ~HashMap()
		{
			Allocator::RawDelete(m_pContent, m_ArraySize * sizeof(Bucket), alignof(Bucket));
		}

	private:
		float m_LoadFactor;
		SizeType m_ArraySize;
		Bucket m_pContent;
		SizeType m_Size;
	};

	/*!
	@}
	*/
}