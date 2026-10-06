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

	/*!
	@brief Get the offset of the `Base` sub object in the `Derived` object.
	@warning `Base` must not be a virtual base class of `Derived`. This function computes the
	offset by casting a fake pointer, which requires the offset to be a compile time constant.
	For a virtual base class, `static_cast` needs to read the vbptr/vbtable in the real object
	to get the offset, so using a fake pointer will cause an access violation. Besides, the
	offset of a virtual base class is not fixed, it depends on the most derived type, so it can
	not be cached as a constant.
	*/
	template<class Derived, class Base>
	// requires std::derived_from<Derived, Base>	// CRTP can not use "requires" to check the inheritance relationship, because the Derived class is not fully defined at this point.
	inline UInt64 GetOffsetOfBase()
	{
		static_assert(std::is_base_of_v<Base, Derived>, "Derived must be derived from Base");
		return reinterpret_cast<UInt64>(static_cast<Base*>(reinterpret_cast<Derived*>(0x1000))) - 0x1000;
	}
}

/*!
@}
*/