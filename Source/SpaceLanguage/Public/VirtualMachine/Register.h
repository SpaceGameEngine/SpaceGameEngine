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
#include "SpaceLanguageAPI.h"
#include "TypeDefinition.hpp"
#include "Error.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage
{

	using RegisterType = UInt64;

	inline constexpr const SizeType RegistersSize = 64;
	inline constexpr const UInt8 SpecialRegistersSize = 3;
	inline constexpr const UInt8 ArgumentRegistersStartIndex = 32;
	inline constexpr const UInt8 CommonRegistersSize = ArgumentRegistersStartIndex - SpecialRegistersSize;
	inline constexpr const UInt8 ArgumentRegistersSize = RegistersSize - ArgumentRegistersStartIndex;

	namespace Register
	{
		inline constexpr const UInt8 ProgramCounter = 0;
		inline constexpr const UInt8 BasePointer = 1;
		inline constexpr const UInt8 StackPointer = 2;

		inline constexpr UInt8 Common(UInt8 idx)
		{
			return SpecialRegistersSize + idx;
		}

		inline constexpr UInt8 Argument(UInt8 idx)
		{
			return ArgumentRegistersStartIndex + idx;
		}
	}

	class SPACE_LANGUAGE_API Registers : public UncopyableAndUnmovable
	{
	public:
		Registers();
		RegisterType& Get(UInt8 index);
		const RegisterType& Get(UInt8 index) const;

	private:
		RegisterType m_Registers[RegistersSize];
	};

}

/*!
@}
*/