/*
Copyright 2022 creatorlxd

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
#include "CommonAPI.h"
#include "TypeDefinition.hpp"
#include "Error.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	enum class Endian : UInt8
	{
		Little = 0,
		Big = 1
	};

	struct InvalidEndianError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Endian is invalid.");
		static COMMON_API bool Judge(Endian endian);
	};

	COMMON_API Endian GetSystemEndian();

	COMMON_API void ChangeEndian(void* ptr, SizeType size, Endian dst, Endian src);

	template<typename T>
	inline void ChangeEndian(T& val, Endian dst, Endian src)
	{
		SGE_ASSERT(InvalidEndianError, dst);
		SGE_ASSERT(InvalidEndianError, src);
		ChangeEndian(&val, sizeof(T), dst, src);
	}

}

/*!
@}
*/