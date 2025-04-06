/*
Copyright 2025 creatorlxd

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
	template<typename T, typename LessComparer = Less<T>, typename Allocator = DefaultAllocator>
	class Set : public Detail::RedBlackTree<T, LessComparer, Equal<T>, Allocator>
	{
	public:
		using ValueType = T;
		using LessComparerType = LessComparer;
		using AllocatorType = Allocator;

	public:
		template<typename... Args>
		inline Set(Args&&... args)
			: Detail::RedBlackTree<T, LessComparer, Equal<T>, Allocator>(std::forward<Args>(args)...)
		{
		}

		template<typename T2>
		inline Set(std::initializer_list<T2> ilist)
			: Detail::RedBlackTree<T, LessComparer, Equal<T>, Allocator>(ilist)
		{
		}

		template<typename Arg>
		inline Set& operator=(Arg&& arg)
		{
			Detail::RedBlackTree<T, LessComparer, Equal<T>, Allocator>::operator=(std::forward<Arg>(arg));

			return *this;
		}
	};
}

/*!
@}
*/