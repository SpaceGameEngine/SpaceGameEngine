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
#include "Utility/Utility.hpp"
#include "Meta/TypeList.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename... Ts>
	class Tuple
	{
	};

	template<typename T, typename... Ts>
	class Tuple<T, Ts...> : public Tuple<Ts...>
	{
	public:
		using Types = TypeList<T, Ts...>;

		inline Tuple<T, Ts...>() = default;

		template<typename U, typename... Us>
			requires(sizeof...(Ts) == sizeof...(Us))
		inline Tuple<T, Ts...>(U&& value, Us&&... values)
			: m_Content(std::forward<U>(value)), Tuple<Ts...>(std::forward<Us>(values)...)
		{
		}

		template<typename U, typename... Us>
			requires(sizeof...(Ts) == sizeof...(Us))
		inline Tuple<T, Ts...>(const Tuple<U, Us...>& other)
			: m_Content(other.template Get<0>()), Tuple<Ts...>(other.template GetRest<0>())
		{
		}

		template<typename U, typename... Us>
			requires(sizeof...(Ts) == sizeof...(Us))
		inline Tuple<T, Ts...>(Tuple<U, Us...>&& other)
			: m_Content(std::move(other.template Get<0>())), Tuple<Ts...>(std::move(other.template GetRest<0>()))
		{
		}

		inline Tuple<T, Ts...>(const Tuple<T, Ts...>& other) = default;
		inline Tuple<T, Ts...>(Tuple<T, Ts...>&& other) = default;

		template<typename U, typename... Us>
			requires(sizeof...(Ts) == sizeof...(Us))
		inline Tuple<T, Ts...>& operator=(const Tuple<U, Us...>& other)
		{
			m_Content = other.template Get<0>();
			Tuple<Ts...>::operator=(other.template GetRest<0>());
			return *this;
		}

		template<typename U, typename... Us>
			requires(sizeof...(Ts) == sizeof...(Us))
		inline Tuple<T, Ts...>& operator=(Tuple<U, Us...>&& other)
		{
			m_Content = std::move(other.template Get<0>());
			Tuple<Ts...>::operator=(std::move(other.template GetRest<0>()));
			return *this;
		}

		inline Tuple<T, Ts...>& operator=(const Tuple<T, Ts...>& other) = default;
		inline Tuple<T, Ts...>& operator=(Tuple<T, Ts...>&& other) = default;

		template<SizeType Index>
			requires(Index < Types::Size)
		inline decltype(auto) Get()
		{
			if constexpr (Index == 0)
				return static_cast<T&>(m_Content);
			else
				return Tuple<Ts...>::template Get<Index - 1>();
		}

		template<SizeType Index>
			requires(Index < Types::Size)
		inline decltype(auto) Get() const
		{
			if constexpr (Index == 0)
				return static_cast<const T&>(m_Content);
			else
				return Tuple<Ts...>::template Get<Index - 1>();
		}

		template<SizeType Index>
			requires(Index < Types::Size)
		inline decltype(auto) GetRest()
		{
			if constexpr (Index == 0)
				return static_cast<Tuple<Ts...>&>(*this);
			else
				return Tuple<Ts...>::template GetRest<Index - 1>();
		}

		template<SizeType Index>
			requires(Index < Types::Size)
		inline decltype(auto) GetRest() const
		{
			if constexpr (Index == 0)
				return static_cast<const Tuple<Ts...>&>(*this);
			else
				return Tuple<Ts...>::template GetRest<Index - 1>();
		}

	private:
		T m_Content;
	};

	template<>
	class Tuple<>
	{
	public:
	};

	namespace Detail
	{
		template<typename T>
		inline constexpr const bool IsTupleValue = false;

		template<typename... Ts>
		inline constexpr const bool IsTupleValue<Tuple<Ts...>> = true;
	}

	template<typename T>
	concept IsTuple = Detail::IsTupleValue<std::remove_cvref_t<T>>;

	namespace Detail
	{
		template<IsTuple A, IsTuple B, SizeType... AIndices, SizeType... BIndices>
		inline auto Concat2TuplesImplement(A&& a, B&& b, std::integer_sequence<SizeType, AIndices...>, std::integer_sequence<SizeType, BIndices...>)
		{
			return Tuple<typename std::remove_cvref_t<A>::Types::template Get<AIndices>..., typename std::remove_cvref_t<B>::Types::template Get<BIndices>...>(ForwardLike<A>(a.template Get<AIndices>())..., ForwardLike<B>(b.template Get<BIndices>())...);
		}

		template<IsTuple A, IsTuple B>
		inline auto Concat2Tuples(A&& a, B&& b)
		{
			return Concat2TuplesImplement(std::forward<A>(a), std::forward<B>(b), std::make_integer_sequence<SizeType, std::remove_cvref_t<A>::Types::Size>(), std::make_integer_sequence<SizeType, std::remove_cvref_t<B>::Types::Size>());
		}
	}

	template<IsTuple A, IsTuple B>
	inline auto ConcatTuples(A&& a, B&& b)
	{
		return Detail::Concat2Tuples(std::forward<A>(a), std::forward<B>(b));
	}

	template<IsTuple A, IsTuple B, IsTuple... Rest>
	inline auto ConcatTuples(A&& a, B&& b, Rest&&... rest)
	{
		return ConcatTuples(Detail::Concat2Tuples(std::forward<A>(a), std::forward<B>(b)), std::forward<Rest>(rest)...);
	}

	template<typename... Args>
	inline auto MakeTuple(Args&&... args)
	{
		return Tuple<std::remove_cvref_t<Args>...>(std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline auto ForwardAsTuple(Args&&... args)
	{
		return Tuple<Args&&...>(std::forward<Args>(args)...);
	}
}

/*!
@}
*/