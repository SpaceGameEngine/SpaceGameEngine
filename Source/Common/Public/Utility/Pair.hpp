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
#include "Error.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
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
			SGE_ASSERT(SelfAssignmentError, this, &c);
			m_First = c.m_First;
			m_Second = c.m_Second;
			return *this;
		}

		inline Pair<T, U>& operator=(Pair<T, U>&& c)
		{
			SGE_ASSERT(SelfAssignmentError, this, &c);
			m_First = std::move(c.m_First);
			m_Second = std::move(c.m_Second);
			return *this;
		}

		template<typename T2, typename U2>
		inline Pair(const Pair<T2, U2>& c)
			: Pair(c.m_First, c.m_Second)
		{
		}

		template<typename T2, typename U2>
		inline Pair(Pair<T2, U2>&& c)
			: Pair(std::move(c.m_First), std::move(c.m_Second))
		{
		}

		template<typename T2, typename U2>
		inline Pair<T, U>& operator=(const Pair<T2, U2>& c)
		{
			m_First = c.m_First;
			m_Second = c.m_Second;
			return *this;
		}

		template<typename T2, typename U2>
		inline Pair<T, U>& operator=(Pair<T2, U2>&& c)
		{
			m_First = std::move(c.m_First);
			m_Second = std::move(c.m_Second);
			return *this;
		}

		inline bool operator==(const Pair<T, U>& c) const
		{
			return c.m_First == m_First && c.m_Second == m_Second;
		}

		inline bool operator!=(const Pair<T, U>& c) const
		{
			return c.m_First != m_First || c.m_Second != m_Second;
		}

		template<typename T2, typename U2>
		inline bool operator==(const Pair<T2, U2>& c) const
		{
			return c.m_First == m_First && c.m_Second == m_Second;
		}

		template<typename T2, typename U2>
		inline bool operator!=(const Pair<T2, U2>& c) const
		{
			return c.m_First != m_First || c.m_Second != m_Second;
		}

		T m_First;
		U m_Second;
	};

	template<typename T, typename U>
	inline constexpr Pair<std::decay_t<T>, std::decay_t<U>> MakePair(T&& first, U&& second)
	{
		return Pair<std::decay_t<T>, std::decay_t<U>>(std::forward<T>(first), std::forward<U>(second));
	}

	template<typename T>
	concept IsPair = requires(typename std::remove_cvref_t<T> pair) {
		pair.m_First;
		pair.m_Second;
		{
			typename std::remove_cvref_t<T>(std::declval<decltype(pair.m_First)>(), std::declval<decltype(pair.m_Second)>())
		} -> std::same_as<std::remove_cvref_t<T>>;
	};

	namespace Detail
	{
		template<typename T, typename U>
		struct KeyValuePair : public Pair<const T, U>
		{
			template<typename... Args>
			inline explicit KeyValuePair(Args&&... args)
				: Pair<const T, U>(std::forward<Args>(args)...)
			{
			}

			inline KeyValuePair(const KeyValuePair<T, U>& c)
				: Pair<const T, U>(c)
			{
			}

			inline KeyValuePair(KeyValuePair<T, U>&& c)
				: Pair<const T, U>(std::move(c))
			{
			}

			inline KeyValuePair<T, U>& operator=(const KeyValuePair<T, U>& c)
			{
				SGE_ASSERT(SelfAssignmentError, this, &c);
				Pair<const T, U>::m_Second = c.m_Second;
				return *this;
			}

			inline KeyValuePair<T, U>& operator=(KeyValuePair<T, U>&& c)
			{
				SGE_ASSERT(SelfAssignmentError, this, &c);
				Pair<const T, U>::m_Second = std::move(c.m_Second);
				return *this;
			}

			template<IsPair P>
			inline KeyValuePair<T, U>& operator=(P&& c)
			{
				if constexpr (std::is_same_v<P, std::remove_reference_t<P>>)
					Pair<const T, U>::m_Second = std::move(c.m_Second);	   // rvalue
				else
					Pair<const T, U>::m_Second = c.m_Second;	// lvalue
				return *this;
			}
		};
	}

	template<typename Comparer>
	struct KeyComparer
	{
		template<IsPair P1, IsPair P2>
		inline static constexpr bool Compare(const P1& lhs, const P2& rhs)
		{
			return Comparer::Compare(lhs.m_First, rhs.m_First);
		}

		template<IsPair P, typename K>
			requires std::is_convertible_v<std::remove_cvref_t<K>, std::remove_cvref_t<decltype(std::declval<P>().m_First)>>
		inline static constexpr bool Compare(const K& key, const P& pair)
		{
			return Comparer::Compare(key, pair.m_First);
		}

		template<IsPair P, typename K>
			requires std::is_convertible_v<std::remove_cvref_t<K>, std::remove_cvref_t<decltype(std::declval<P>().m_First)>>
		inline static constexpr bool Compare(const P& pair, const K& key)
		{
			return Comparer::Compare(pair.m_First, key);
		}
	};
}

/*!
@}
*/