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
#include "TypeDefinition.hpp"
#include "SGEString.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	using HashType = UInt64;

	template<typename T>
	struct Hash
	{
		inline static HashType GetHash(const T& val)
		{
			return std::hash<T>()(val);
		}
	};

	template<typename T, typename Trait, typename Allocator>
	struct Hash<StringCore<T, Trait, Allocator>>
	{
		inline static HashType GetHash(const StringCore<T, Trait, Allocator>& val)
		{
			HashType re = 0;
			HashType seed = 131;
			const T* str = val.GetData();
			while (*str)
			{
				re = re * seed + *(str++);
			}
			return re;
		}
	};
}

/*!
@}
*/