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
#include <type_traits>
#include "Meta/Trait.hpp"
#include "Meta/Concept.hpp"
#include "TypeDefinition.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	template<typename IteratorType>
	struct GetIteratorValueType
	{
	private:
		template<typename T>
		inline static constexpr TypeWrapper<typename T::ValueType> Check(int)
		{
			return TypeWrapper<typename T::ValueType>();
		}

		template<typename T>
		inline static constexpr TypeWrapper<typename T::value_type> Check(long)
		{
			return TypeWrapper<typename T::value_type>();
		}

		template<typename T>
		inline static constexpr TypeWrapper<void> Check(...)
		{
			return TypeWrapper<void>();
		}

	public:
		using Type = typename decltype(Check<RemoveCVRefType<IteratorType>>(0))::Type;
	};

	template<typename T>
	struct GetIteratorValueType<T*>
	{
		using Type = T;
	};

	template<typename IteratorType>
	using IteratorValueType = typename GetIteratorValueType<IteratorType>::Type;

	template<typename T>
	concept IsBaseIterator = IsCopyable<T> && IsEqualityComparable<T> && requires(T t) {
		++t;
		t++;
		*t;
	};

	template<typename T>
	concept IsBidirectionalBaseIterator = IsBaseIterator<T> && requires(T t) {
		--t;
		t--;
	};

	template<typename T>
	concept IsSequentialIterator = IsBaseIterator<T> && requires(T t, T t2) {
		t + 1;
		t += 1;
		{
			t - t2
		}
		-> std::convertible_to<SizeType>;
	};

	template<typename T>
	concept IsBidirectionalSequentialIterator = IsBidirectionalBaseIterator<T> && IsSequentialIterator<T> && requires(T t) {
		t - 1;
		t -= 1;
	};

}

/*!
@}
*/