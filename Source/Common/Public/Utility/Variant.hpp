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
#include "Utility/Utility.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	namespace Detail
	{
		template<typename T, typename... Ts>
		struct MaxSizeOfTypes
		{
			inline static constexpr const SizeType Value = Max(sizeof(T), MaxSizeOfTypes<Ts...>::Value);
		};

		template<typename T>
		struct MaxSizeOfTypes<T>
		{
			inline static constexpr const SizeType Value = sizeof(T);
		};

		template<typename T, typename... Ts>
		struct MaxAlignmentOfTypes
		{
			inline static constexpr const SizeType Value = Max(alignof(T), MaxAlignmentOfTypes<Ts...>::Value);
		};

		template<typename T>
		struct MaxAlignmentOfTypes<T>
		{
			inline static constexpr const SizeType Value = alignof(T);
		};
	}

	template<typename... Ts>
	class Variant
	{
	public:
		inline static constexpr const SizeType Size = Detail::MaxSizeOfTypes<Ts...>::Value;
		inline static constexpr const SizeType Alignment = Detail::MaxAlignmentOfTypes<Ts...>::Value;
	};
}

/*!
@}
*/