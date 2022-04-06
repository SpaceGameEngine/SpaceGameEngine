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
		inline static constexpr const SizeType sm_Size = 23;
		inline static constexpr const SizeType sm_MaxInstructionSize = 10;

		inline static constexpr const SizeType sm_ExternalCall = 0;
		inline static constexpr const SizeType sm_Set = 1;
		inline static constexpr const SizeType sm_Copy = 2;
		inline static constexpr const SizeType sm_Goto = 3;
		inline static constexpr const SizeType sm_GotoRegister = 4;
		inline static constexpr const SizeType sm_If = 5;

		inline static constexpr const SizeType sm_Add = 6;
		inline static constexpr const SizeType sm_Subtract = 7;
		inline static constexpr const SizeType sm_Multiply = 8;
		inline static constexpr const SizeType sm_Divide = 9;
		inline static constexpr const SizeType sm_Mod = 10;
		inline static constexpr const SizeType sm_And = 11;
		inline static constexpr const SizeType sm_Or = 12;
		inline static constexpr const SizeType sm_Xor = 13;
		inline static constexpr const SizeType sm_Not = 14;
		inline static constexpr const SizeType sm_ShiftLeft = 15;
		inline static constexpr const SizeType sm_ShiftRight = 16;

		inline static constexpr const SizeType sm_Equal = 17;
		inline static constexpr const SizeType sm_NotEqual = 18;
		inline static constexpr const SizeType sm_Less = 19;
		inline static constexpr const SizeType sm_LessEqual = 20;
		inline static constexpr const SizeType sm_Greater = 21;
		inline static constexpr const SizeType sm_GreaterEqual = 22;

		SPACE_LANGUAGE_API InstructionSet();
		SPACE_LANGUAGE_API const InstructionType& Get(SizeType index) const;

	private:
		SPACE_LANGUAGE_API static void ExternalCall(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Set(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Copy(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Goto(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void GotoRegister(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void If(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

		SPACE_LANGUAGE_API static void Add(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Subtract(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Multiply(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Divide(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Mod(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void And(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Or(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Xor(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Not(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void ShiftLeft(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void ShiftRight(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

		SPACE_LANGUAGE_API static void Equal(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void NotEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Less(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void LessEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void Greater(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		SPACE_LANGUAGE_API static void GreaterEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

	private:
		InstructionType m_InstructionTypes[sm_Size];
	};

	/*!
	@}
	*/
}