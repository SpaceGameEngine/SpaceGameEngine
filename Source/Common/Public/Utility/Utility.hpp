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
#include "ForwardDefinition.hpp"
#include "Meta/Trait.hpp"
#include "CommonAPI.h"
#include <utility>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	struct COMMON_API Uncopyable
	{
		Uncopyable() = default;
		Uncopyable(const Uncopyable&) = delete;
		Uncopyable& operator=(const Uncopyable&) = delete;
	};

	struct COMMON_API UncopyableAndUnmovable
	{
		UncopyableAndUnmovable() = default;
		UncopyableAndUnmovable(const UncopyableAndUnmovable&) = delete;
		UncopyableAndUnmovable(UncopyableAndUnmovable&&) = delete;
		UncopyableAndUnmovable& operator=(const UncopyableAndUnmovable&) = delete;
		UncopyableAndUnmovable& operator=(UncopyableAndUnmovable&&) = delete;
	};

	template<typename T>
	inline constexpr T Min(const T& a, const T& b)
	{
		return (a < b ? a : b);
	}

	template<typename T>
	inline constexpr T Max(const T& a, const T& b)
	{
		return (a > b ? a : b);
	}

	template<typename T>
	struct Less
	{
		inline static constexpr bool Compare(const T& lhs, const T& rhs)
		{
			return lhs < rhs;
		}
	};

	template<typename T>
	struct Equal
	{
		inline static constexpr bool Compare(const T& lhs, const T& rhs)
		{
			return lhs == rhs;
		}
	};

	template<typename T>
	struct Greater
	{
		inline static constexpr bool Compare(const T& lhs, const T& rhs)
		{
			return lhs > rhs;
		}
	};

	template<UInt64 Base>
	inline UInt64 Digits(UInt64 v)
	{
		static constexpr const UInt64 Base2 = Base * Base;
		static constexpr const UInt64 Base3 = Base * Base * Base;
		static constexpr const UInt64 Base4 = Base * Base * Base * Base;
		UInt64 re = 1;
		while (true)
		{
			if (v < Base)
			{
				return re;
			}
			if (v < Base2)
			{
				return re + 1;
			}
			if (v < Base3)
			{
				return re + 2;
			}
			if (v < Base4)
			{
				return re + 3;
			}
			v /= Base4;
			re += 4;
		}
	}

	struct COMMON_API EmptyType
	{
	};

	template<class T, class U>
	inline constexpr auto&& ForwardLike(U&& x)
	{
		constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
		if constexpr (std::is_lvalue_reference_v<T&&>)
		{
			if constexpr (is_adding_const)
				return std::as_const(x);
			else
				return static_cast<U&>(x);
		}
		else
		{
			if constexpr (is_adding_const)
				return std::move(std::as_const(x));
			else
				return std::move(x);
		}
	}
}

/*!
@}
*/