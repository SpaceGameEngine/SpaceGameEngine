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
#include "Trait.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@file
	@todo use cpp20's concept instead of sfinae(change IsXxx to Xxx).
	*/

	template<typename T>
	struct IsDefaultConstructible
	{
		inline static constexpr const bool Value = std::is_default_constructible_v<T>;
	};

	template<typename T>
	struct IsCopyConstructible
	{
		inline static constexpr const bool Value = std::is_copy_constructible_v<T>;
	};

	template<typename T>
	struct IsMoveConstructible
	{
		inline static constexpr const bool Value = std::is_move_constructible_v<T>;
	};

	template<typename T>
	struct IsCopyAssignable
	{
		inline static constexpr const bool Value = std::is_copy_assignable_v<T>;
	};

	template<typename T>
	struct IsMoveAssignable
	{
		inline static constexpr const bool Value = std::is_move_assignable_v<T>;
	};

	template<typename T>
	struct IsMovable
	{
		inline static constexpr const bool Value = IsMoveConstructible<T>::Value && IsMoveAssignable<T>::Value;
	};

	template<typename T>
	struct IsCopyable
	{
		inline static constexpr const bool Value = IsMovable<T>::Value && IsCopyConstructible<T>::Value && IsCopyAssignable<T>::Value;
	};

	template<typename T, typename U = T>
	struct IsWeakEqualityComparable
	{
	private:
		template<typename _T, typename _U>
		inline static constexpr std::enable_if_t<
			std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() == std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() == std::declval<const RemoveCVRefType<_T>&>()), bool>,
			bool>
		Check(int)
		{
			return true;
		}
		template<typename _T, typename _U>
		inline static constexpr bool Check(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = Check<T, T>(0) && Check<U, U>(0) && Check<T, U>(0);
	};

	template<typename T, typename U = T>
	struct IsEqualityComparable
	{
	private:
		template<typename _T, typename _U>
		inline static constexpr std::enable_if_t<
			IsWeakEqualityComparable<_T, _U>::Value &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() != std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() != std::declval<const RemoveCVRefType<_T>&>()), bool>,
			bool>
		Check(int)
		{
			return true;
		}
		template<typename _T, typename _U>
		inline static constexpr bool Check(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = Check<T, T>(0) && Check<U, U>(0) && Check<T, U>(0);
	};

	template<typename T, typename U = T>
	struct IsTotallyOrdered
	{
	private:
		template<typename _T, typename _U>
		inline static constexpr std::enable_if_t<
			std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() < std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() > std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() <= std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_T>&>() >= std::declval<const RemoveCVRefType<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() < std::declval<const RemoveCVRefType<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() > std::declval<const RemoveCVRefType<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() <= std::declval<const RemoveCVRefType<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const RemoveCVRefType<_U>&>() >= std::declval<const RemoveCVRefType<_T>&>()), bool>,
			bool>
		Check(int)
		{
			return true;
		}
		template<typename _T, typename _U>
		inline static constexpr bool Check(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = IsEqualityComparable<T, U>::Value && Check<T, T>(0) && Check<U, U>(0) && Check<T, U>(0);
	};

	template<typename T>
	struct IsTrivial
	{
		inline static constexpr const bool Value = std::is_trivial_v<T>;
	};

	/*!
	@}
	*/
}