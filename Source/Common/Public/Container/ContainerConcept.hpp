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
#include "Meta/Trait.hpp"
#include "Meta/Concept.hpp"

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

	template<typename IteratorType>
	struct GetIteratorValueType
	{
	private:
		template<typename T>
		inline static constexpr TypeWrapper<typename T::ValueType> Check(int)
		{
			return TypeWrapper<typename T::ValueType>();
		}

		template<typename T>
		inline static constexpr TypeWrapper<typename T::value_type> Check(long)
		{
			return TypeWrapper<typename T::value_type>();
		}

		template<typename T>
		inline static constexpr TypeWrapper<void> Check(...)
		{
			return TypeWrapper<void>();
		}

	public:
		using Type = typename decltype(Check<RemoveCVRefType<IteratorType>>(0))::Type;
	};

	template<typename T>
	struct GetIteratorValueType<T*>
	{
		using Type = T;
	};

	template<typename IteratorType>
	using IteratorValueType = typename GetIteratorValueType<IteratorType>::Type;

	/*!
	@brief check the type to make sure that it is Base Iterator Type.
	@param U the type need to be checked.
	@todo use concept.
	*/
	template<typename U>
	struct IsBaseIterator
	{
	private:
		template<typename _U>
		inline static constexpr std::enable_if_t<
			IsCopyable<_U>::Value &&
				std::is_same_v<RemoveCVRefType<decltype(++std::declval<_U>())>, RemoveCVRefType<_U>> &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>()++)>, RemoveCVRefType<_U>> &&
				std::is_same_v<RemoveCVRefType<decltype(*std::declval<_U>())>, RemoveCVRefType<IteratorValueType<_U>>> &&
				IsEqualityComparable<_U>::Value,
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
		inline static constexpr const bool Value = Check<RemoveCVRefType<U>>(0);
	};

	template<typename T>
	struct IsBaseIterator<T*>
	{
		inline static constexpr const bool Value = true;
	};

	/*!
	@brief check the type to make sure that it is Base Bidirectional Iterator Type.
	@param U the type need to be checked.
	@todo use concept.
	*/
	template<typename U>
	struct IsBidirectionalBaseIterator
	{
	private:
		template<typename _U>
		inline static constexpr std::enable_if_t<
			IsBaseIterator<_U>::Value &&
				std::is_same_v<RemoveCVRefType<decltype(--std::declval<_U>())>, RemoveCVRefType<_U>> &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>()--)>, RemoveCVRefType<_U>>,
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
		inline static constexpr const bool Value = Check<RemoveCVRefType<U>>(0);
	};

	template<typename T>
	struct IsBidirectionalBaseIterator<T*>
	{
		inline static constexpr const bool Value = true;
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
			IsBaseIterator<_U>::Value &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>() + std::declval<SizeType>())>, RemoveCVRefType<_U>> &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>() += std::declval<SizeType>())>, RemoveCVRefType<_U>> &&
				std::is_same_v<decltype(static_cast<SizeType>(std::declval<_U>() - std::declval<_U>())), SizeType>,
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
		inline static constexpr const bool Value = Check<RemoveCVRefType<U>>(0);
	};

	template<typename T>
	struct IsSequentialIterator<T*>
	{
		inline static constexpr const bool Value = true;
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
			IsBidirectionalBaseIterator<_U>::Value &&
				IsSequentialIterator<_U>::Value &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>() - std::declval<SizeType>())>, RemoveCVRefType<_U>> &&
				std::is_same_v<RemoveCVRefType<decltype(std::declval<_U>() -= std::declval<SizeType>())>, RemoveCVRefType<_U>>,
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
		inline static constexpr const bool Value = Check<RemoveCVRefType<U>>(0);
	};

	template<typename T>
	struct IsBidirectionalSequentialIterator<T*>
	{
		inline static constexpr const bool Value = true;
	};
	/*!
	@}
	*/
}