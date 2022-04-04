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

	class Registers
	{
	public:
		inline static constexpr const SizeType sm_Size = 32;

		inline static constexpr const SizeType sm_Argument0 = 0;
		inline static constexpr const SizeType sm_Argument1 = 1;
		inline static constexpr const SizeType sm_Argument2 = 2;
		inline static constexpr const SizeType sm_ProgramCounter = 3;
		inline static constexpr const SizeType sm_BasePointer = 4;
		inline static constexpr const SizeType sm_StackPointer = 5;

		SPACE_LANGUAGE_API Registers();
		SPACE_LANGUAGE_API RegisterType& Get(SizeType index);
		SPACE_LANGUAGE_API const RegisterType& Get(SizeType index) const;

	private:
		RegisterType m_Registers[sm_Size];
	};

	struct InvalidRegisterIndexError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterIndex is invalid.");
		static SPACE_LANGUAGE_API bool Judge(SizeType reg_index);
	};

	/*!
	@}
	*/
}