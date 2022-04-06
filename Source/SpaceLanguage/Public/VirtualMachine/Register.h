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

namespace SpaceGameEngine::SpaceLanguage
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

	using RegisterType = UInt64;

	inline constexpr const SizeType RegistersSize = 32;

	namespace SpecialRegister
	{
		inline constexpr const UInt8 Argument0 = 0;
		inline constexpr const UInt8 Argument1 = 1;
		inline constexpr const UInt8 Argument2 = 2;
		inline constexpr const UInt8 ProgramCounter = 3;
		inline constexpr const UInt8 BasePointer = 4;
		inline constexpr const UInt8 StackPointer = 5;
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

	/*!
	@}
	*/
}