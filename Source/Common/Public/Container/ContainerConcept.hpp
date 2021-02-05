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
#include <type_traits>

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
	struct IsEqualityComparable
	{
	private:
		template<typename _T, typename _U>
		inline static constexpr std::enable_if_t<
			std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() == std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() != std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() == std::declval<const std::remove_cv_t<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() != std::declval<const std::remove_cv_t<_T>&>()), bool>,
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
			std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() < std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() > std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() <= std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_T>&>() >= std::declval<const std::remove_cv_t<_U>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() < std::declval<const std::remove_cv_t<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() > std::declval<const std::remove_cv_t<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() <= std::declval<const std::remove_cv_t<_T>&>()), bool> &&
				std::is_same_v<decltype(std::declval<const std::remove_cv_t<_U>&>() >= std::declval<const std::remove_cv_t<_T>&>()), bool>,
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

	/*!
	@brief check the type to make sure that it is sequential Iterator Type.
	@param U the type need to be checked.
	@todo use concept.
	*/
	template<typename U>
	struct IsSequentialIterator
	{
	private:
		template<typename _U>
		inline static constexpr std::enable_if_t<
			//IsError<typename _U::OutOfRangeError, const _U&, T*, T*>::Value &&
			//	std::is_same_v<decltype(_U::GetBegin(*(new Vector))), _U> &&
			//	std::is_same_v<decltype(_U::GetEnd(*(new Vector))), _U> &&
			std::is_same_v<decltype(new _U(std::declval<_U>())), _U*> &&
				std::is_same_v<decltype(std::declval<_U>() = std::declval<_U>()), _U&> &&
				std::is_same_v<decltype(std::declval<_U>() + std::declval<SizeType>()), _U> &&
				std::is_same_v<decltype(std::declval<_U>() += std::declval<SizeType>()), _U&> &&
				std::is_same_v<decltype(++std::declval<_U>()), _U&> &&
				std::is_same_v<decltype(std::declval<_U>()++), const _U> &&
				std::is_same_v<decltype(std::declval<_U>() - std::declval<_U>()), SizeType> &&
				(std::is_same_v<decltype(std::declval<_U>().operator->()), typename _U::ValueType*>)&&(std::is_same_v<decltype(std::declval<_U>().operator*()), typename _U::ValueType&>)&&std::is_same_v<decltype(std::declval<_U>() == std::declval<_U>()), bool> &&
				std::is_same_v<decltype(std::declval<_U>() != std::declval<_U>()), bool>,
			bool>
		Check(int)
		{
			return true;
		}

		template<typename _U>
		inline static constexpr bool Check(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = Check<std::remove_cv_t<U>>(0);
	};

	template<typename T>
	struct IsTrivial
	{
		inline static constexpr const bool Value = std::is_trivial_v<T>;
	};

	/*!
	@brief check the type to make sure that it is bidirectional Iterator Type.
	@param U the type need to be checked.
	@todo use concept.
	*/
	template<typename U>
	struct IsBidirectionalSequentialIterator
	{
	private:
		template<typename _U>
		inline static constexpr std::enable_if_t<
			IsSequentialIterator<_U>::Value &&
				std::is_same_v<decltype(std::declval<_U>() - std::declval<SizeType>()), _U> &&
				std::is_same_v<decltype(std::declval<_U>() -= std::declval<SizeType>()), _U&> &&
				std::is_same_v<decltype(--std::declval<_U>()), _U&> &&
				std::is_same_v<decltype(std::declval<_U>()--), const _U>,
			bool>
		Check(int)
		{
			return true;
		}

		template<typename _U>
		inline static constexpr bool Check(...)
		{
			return false;
		}

	public:
		inline static constexpr const bool Value = Check<std::remove_cv_t<U>>(0);
	};

	/*!
	@}
	*/
}