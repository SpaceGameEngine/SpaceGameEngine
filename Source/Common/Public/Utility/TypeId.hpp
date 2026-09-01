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
#include "TypeDefinition.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	namespace Detail
	{
		template<typename T>
		struct GetTypeIdCore
		{
		};
	}

	/*!
	@brief Get the unique type ID for a given type T.
	@warning The result is only guaranteed to be unique within a single execution of the program. It may not be consistent across different runs or different platforms.
	*/
	template<typename T>
	inline UInt64 GetTypeId()
	{
		return Detail::GetTypeIdCore<T>::Run();
	}

#define SGE_DECLARE_TYPE_ID(API_MACRO, TYPE)      \
	namespace SpaceGameEngine::Detail             \
	{                                             \
		template<>                                \
		struct API_MACRO GetTypeIdCore<TYPE>      \
		{                                         \
			static SpaceGameEngine::UInt64 Run(); \
		};                                        \
	}

#define SGE_DEFINE_TYPE_ID(API_MACRO, TYPE)                                     \
	SpaceGameEngine::UInt64 SpaceGameEngine::Detail::GetTypeIdCore<TYPE>::Run() \
	{                                                                           \
		static int val = 0;                                                     \
		return reinterpret_cast<SpaceGameEngine::UInt64>(&val);                 \
	}
}

/*!
@}
*/