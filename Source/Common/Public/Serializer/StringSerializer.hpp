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
#include "Serializer/Serializer.h"
#include "SGEString.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<IsString StringType>
	class StringSerializer
	{
	public:
		template<typename T>
		inline static MemoryData Serialize(const T& value)
		{
			return MakeMemoryData(ToString<StringType>(value));
		}

		template<typename T>
		inline static T Deserialize(const MemoryData& md)
		{
			using CharType = StringType::CharType;
			return StringTo<StringType, T>(StringType((const CharType*)md.GetData(), (const CharType*)((Byte*)md.GetData() + md.GetSize())));
		}
	};
}

/*!
@}
*/