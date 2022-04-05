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
#include "ExternalCaller.h"
#include "SGEString.hpp"

namespace SpaceGameEngine::SpaceLanguage
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

	using InstructionFunctionType = void (*)(Registers&, void*, const ExternalCaller&);

	struct SPACE_LANGUAGE_API InstructionType
	{
		InstructionType();
		InstructionType(UInt8 index, const String& name, UInt8 size, InstructionFunctionType pfunc);

		UInt8 m_Index;
		String m_Name;
		UInt8 m_Size;
		InstructionFunctionType m_pFunction;
	};

	class InstructionSet : public UncopyableAndUnmovable
	{
	public:
		inline static constexpr const SizeType sm_Size = 34;
		inline static constexpr const SizeType sm_MaxInstructionSize = 10;

		SPACE_LANGUAGE_API InstructionSet();

	private:
		SPACE_LANGUAGE_API static void ExternalCall(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

	private:
		InstructionType m_InstructionTypes[sm_Size];
	};

	/*!
	@}
	*/
}