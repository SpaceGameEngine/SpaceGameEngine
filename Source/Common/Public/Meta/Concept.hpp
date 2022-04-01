/*
Copyright 2022 creatorlxd

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
#include <concepts>
#include "Trait.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T>
	concept IsDefaultConstructible = std::is_default_constructible_v<T>;

	template<typename T>
	concept IsCopyConstructible = std::is_copy_constructible_v<T>;

	template<typename T>
	concept IsMoveConstructible = std::is_move_constructible_v<T>;

	template<typename T>
	concept IsCopyAssignable = std::is_copy_assignable_v<T>;

	template<typename T>
	concept IsMoveAssignable = std::is_move_assignable_v<T>;

	template<typename T>
	concept IsMovable = IsMoveConstructible<T>&& IsMoveAssignable<T>;

	template<typename T>
	concept IsCopyable = IsMovable<T>&& IsCopyConstructible<T>&& IsCopyAssignable<T>;

	template<typename T, typename U = T>
	concept IsWeakEqualityComparable = requires(T t, U u)
	{
		{
			t == u
		}
		->std::same_as<bool>;
		{
			u == t
		}
		->std::same_as<bool>;
	};

	template<typename T, typename U = T>
	concept IsEqualityComparable = IsWeakEqualityComparable<T, U>&& requires(T t, U u)
	{
		{
			t != u
		}
		->std::same_as<bool>;
		{
			u != t
		}
		->std::same_as<bool>;
	};

	template<typename T, typename U = T>
	concept IsTotallyOrdered = IsEqualityComparable<T, U>&& requires(T t, U u)
	{
		{
			t < u
		}
		->std::same_as<bool>;
		{
			t > u
		}
		->std::same_as<bool>;
		{
			t <= u
		}
		->std::same_as<bool>;
		{
			t >= u
		}
		->std::same_as<bool>;
		{
			u < t
		}
		->std::same_as<bool>;
		{
			u > t
		}
		->std::same_as<bool>;
		{
			u <= t
		}
		->std::same_as<bool>;
		{
			u >= t
		}
		->std::same_as<bool>;
	};

	template<typename T>
	concept IsTrivial = std::is_trivial_v<T>;

	/*!
	@}
	*/
}