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
#include "Meta/Trait.hpp"
#include <type_traits>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	namespace Detail
	{
		template<SizeType Index, typename... Ts>
		struct GetTypeFromTypes;

		template<SizeType Index, typename T, typename... Ts>
		struct GetTypeFromTypes<Index, T, Ts...>
		{
			using Type = typename GetTypeFromTypes<Index - 1, Ts...>::Type;
		};

		template<typename T, typename... Ts>
		struct GetTypeFromTypes<0, T, Ts...>
		{
			using Type = T;
		};

		template<template<typename> typename T>
		concept IsTypeFilter = std::is_same_v<std::remove_cvref_t<decltype(T<void>::Value)>, bool>;

		template<template<typename> typename Filter, SizeType Index, typename... Ts>
			requires IsTypeFilter<Filter>
		struct FirstIndexInTypesCore;

		template<template<typename> typename Filter, SizeType Index, typename T, typename... Ts>
			requires IsTypeFilter<Filter>
		struct FirstIndexInTypesCore<Filter, Index, T, Ts...>
		{
			inline static constexpr const SizeType Value = Filter<T>::Value ? Index : FirstIndexInTypesCore<Filter, Index + 1, Ts...>::Value;
		};

		template<template<typename> typename Filter, SizeType Index>
			requires IsTypeFilter<Filter>
		struct FirstIndexInTypesCore<Filter, Index>
		{
			inline static constexpr const SizeType Value = Index;
		};

		template<template<typename> typename Filter, typename... Ts>
			requires IsTypeFilter<Filter>
		inline constexpr const SizeType FirstIndexInTypes = FirstIndexInTypesCore<Filter, 0, Ts...>::Value;

		template<template<typename> typename T>
		concept IsTypeMapper = requires { typename T<void>::Type; };
	}

	template<typename... Ts>
	struct TypeList
	{
		inline static constexpr const SizeType Size = sizeof...(Ts);

		template<template<typename...> typename T>
		using Apply = T<Ts...>;

		template<SizeType Index>
		using Get = typename Detail::GetTypeFromTypes<Index, Ts...>::Type;

		template<template<typename> typename Filter>
			requires Detail::IsTypeFilter<Filter>
		inline static constexpr const SizeType FirstIndex = Detail::FirstIndexInTypes<Filter, Ts...>;

		template<template<typename> typename Mapper>
			requires Detail::IsTypeMapper<Mapper>
		using Map = TypeList<typename Mapper<Ts>::Type...>;
	};

	namespace Detail
	{
		template<typename T>
		struct IsTypeListCore
		{
			inline static constexpr const bool Value = false;
		};

		template<typename... Ts>
		struct IsTypeListCore<TypeList<Ts...>>
		{
			inline static constexpr const bool Value = true;
		};
	}

	template<typename T>
	concept IsTypeList = Detail::IsTypeListCore<T>::Value;

}

/*!
@}
*/