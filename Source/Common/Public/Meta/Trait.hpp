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
#include <type_traits>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename T>
	struct RemoveCVRef
	{
		using Type = std::remove_cv_t<std::remove_reference_t<T>>;
	};

	template<typename T>
	using RemoveCVRefType = typename RemoveCVRef<T>::Type;

	template<typename T>
	struct TypeWrapper
	{
		using Type = T;
	};

	template<typename T>
	inline constexpr const TypeWrapper<T> InPlaceType;

	template<auto V>
	struct ValueWrapper
	{
		inline static constexpr const auto Value = V;
	};

	template<SizeType N>
	inline constexpr const ValueWrapper<N> InPlaceIndex;

	template<template<typename...> typename T, typename... Ts>
	struct BindFirstWithType
	{
		template<typename... Args>
		using Type = T<Ts..., Args...>;
	};

	template<template<auto...> typename T, auto... Vs>
	struct BindFirstWithValue
	{
		template<auto... Args>
		using Type = T<Vs..., Args...>;
	};

	template<template<typename...> typename T, template<typename> typename U>
	struct Compose
	{
		template<typename... Args>
		using Type = U<T<Args...>>;
	};

	template<typename... Ts>
	struct IsTypesUnique
	{
	};

	template<typename T>
	struct IsTypesUnique<T>
	{
		inline static constexpr const bool Value = true;
	};

	template<typename T, typename... Ts>
	struct IsTypesUnique<T, Ts...>
	{
		inline static constexpr const bool Value = (!std::is_same_v<T, Ts> && ...) && IsTypesUnique<Ts...>::Value;
	};

	template<typename T1, typename T2>
	struct IsSame
	{
		inline static constexpr const bool Value = std::is_same_v<T1, T2>;
	};

	template<typename T>
	struct IsSameWith
	{
		template<typename U>
		struct Type
		{
			inline static constexpr const bool Value = std::is_same_v<T, U>;
		};
	};
}

/*!
@}
*/