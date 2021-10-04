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
#include "ForwardDefinition.hpp"
#include "Meta/Trait.hpp"
#include "CommonAPI.h"
#include <utility>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	struct COMMON_API UncopyableAndUnmovable
	{
		inline UncopyableAndUnmovable() = default;
		inline UncopyableAndUnmovable(const UncopyableAndUnmovable&) = delete;
		inline UncopyableAndUnmovable(UncopyableAndUnmovable&&) = delete;
		inline UncopyableAndUnmovable& operator=(const UncopyableAndUnmovable&) = delete;
		inline UncopyableAndUnmovable& operator=(UncopyableAndUnmovable&&) = delete;
	};

	template<typename T, typename U>
	struct Pair
	{
		inline Pair() = default;
		template<typename T2, typename U2>
		inline Pair(T2&& t, U2&& u)
			: m_First(std::forward<T2>(t)), m_Second(std::forward<U2>(u))
		{
		}
		inline Pair(const Pair<T, U>& c)
			: Pair(c.m_First, c.m_Second)
		{
		}
		inline Pair(Pair<T, U>&& c)
			: Pair(std::move(c.m_First), std::move(c.m_Second))
		{
		}
		inline Pair<T, U>& operator=(const Pair<T, U>& c)
		{
			m_First = c.m_First;
			m_Second = c.m_Second;
			return *this;
		}
		inline Pair<T, U>& operator=(Pair<T, U>&& c)
		{
			m_First = std::move(c.m_First);
			m_Second = std::move(c.m_Second);
			return *this;
		}
		T m_First;
		U m_Second;
	};

	template<typename T>
	inline T Min(const T& a, const T& b)
	{
		return (a < b ? a : b);
	}

	template<typename T>
	inline T Max(const T& a, const T& b)
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
	struct Greater
	{
		inline static constexpr bool Compare(const T& lhs, const T& rhs)
		{
			return lhs > rhs;
		}
	};


	inline UInt64 Digits10(UInt64 v)
	{
		UInt64 re = 1;
		while (true)
		{
			if (v < 10)
			{
				return re;
			}
			if (v < 100)
			{
				return re + 1;
			}
			if (v < 1000)
			{
				return re + 2;
			}
			if (v < 10000)
			{
				return re + 3;
			}
			v /= 10000U;
			re += 4;
		}
	}
	/*!
	@}
	*/
}