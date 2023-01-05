/*
Copyright 2023 creatorlxd

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
#include "Utility/Singleton.hpp"
#include "Container/Vector.hpp"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage
{

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

	inline constexpr const SizeType InstructionSetSize = 23;
	inline constexpr const SizeType MaxInstructionSize = 10;

	namespace InstructionTypeIndex
	{
		inline constexpr const UInt8 ExternalCall = 0;
		inline constexpr const UInt8 Set = 1;
		inline constexpr const UInt8 Copy = 2;
		inline constexpr const UInt8 Goto = 3;
		inline constexpr const UInt8 GotoRegister = 4;
		inline constexpr const UInt8 If = 5;

		inline constexpr const UInt8 Add = 6;
		inline constexpr const UInt8 Subtract = 7;
		inline constexpr const UInt8 Multiply = 8;
		inline constexpr const UInt8 Divide = 9;
		inline constexpr const UInt8 Mod = 10;
		inline constexpr const UInt8 And = 11;
		inline constexpr const UInt8 Or = 12;
		inline constexpr const UInt8 Xor = 13;
		inline constexpr const UInt8 Not = 14;
		inline constexpr const UInt8 ShiftLeft = 15;
		inline constexpr const UInt8 ShiftRight = 16;

		inline constexpr const UInt8 Equal = 17;
		inline constexpr const UInt8 NotEqual = 18;
		inline constexpr const UInt8 Less = 19;
		inline constexpr const UInt8 LessEqual = 20;
		inline constexpr const UInt8 Greater = 21;
		inline constexpr const UInt8 GreaterEqual = 22;
	}

	class SPACE_LANGUAGE_API InstructionSet : public UncopyableAndUnmovable, public Singleton<InstructionSet>
	{
	private:
		InstructionSet();

	public:
		const InstructionType& Get(UInt8 index) const;

		friend DefaultAllocator;

	private:
		static void ExternalCall(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Set(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Copy(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Goto(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void GotoRegister(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void If(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

		static void Add(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Subtract(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Multiply(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Divide(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Mod(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void And(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Or(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Xor(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Not(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void ShiftLeft(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void ShiftRight(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

		static void Equal(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void NotEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Less(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void LessEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void Greater(Registers& regs, void* pargs, const ExternalCaller& ext_caller);
		static void GreaterEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller);

	private:
		InstructionType m_InstructionTypes[InstructionSetSize];
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<UInt8>;
#endif

	namespace InstructionsGenerator
	{
		SPACE_LANGUAGE_API void AddInstruction(Vector<UInt8>& vec, UInt8 instr_idx, UInt8 arg0);
		SPACE_LANGUAGE_API void AddInstruction(Vector<UInt8>& vec, UInt8 instr_idx, UInt8 arg0, UInt8 arg1);
		SPACE_LANGUAGE_API void AddInstruction(Vector<UInt8>& vec, UInt8 instr_idx, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void AddInstruction(Vector<UInt8>& vec, UInt8 instr_idx, UInt64 data);
		SPACE_LANGUAGE_API void AddInstruction(Vector<UInt8>& vec, UInt8 instr_idx, UInt8 arg0, UInt64 data);

		SPACE_LANGUAGE_API void ExternalCall(Vector<UInt8>& vec, UInt8 arg0, UInt64 data);
		SPACE_LANGUAGE_API void Set(Vector<UInt8>& vec, UInt8 arg0, UInt64 data);
		SPACE_LANGUAGE_API void Copy(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1);
		SPACE_LANGUAGE_API void Goto(Vector<UInt8>& vec, UInt64 data);
		SPACE_LANGUAGE_API void GotoRegister(Vector<UInt8>& vec, UInt8 arg0);
		SPACE_LANGUAGE_API void If(Vector<UInt8>& vec, UInt8 arg0, UInt64 data);

		SPACE_LANGUAGE_API void Add(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Subtract(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Multiply(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Divide(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Mod(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void And(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Or(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Xor(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Not(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1);
		SPACE_LANGUAGE_API void ShiftLeft(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void ShiftRight(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);

		SPACE_LANGUAGE_API void Equal(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void NotEqual(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Less(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void LessEqual(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void Greater(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		SPACE_LANGUAGE_API void GreaterEqual(Vector<UInt8>& vec, UInt8 arg0, UInt8 arg1, UInt8 arg2);
	};

	struct InvalidInstructionsError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Instructions is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const void* ptr, SizeType size);
	};

}

/*!
@}
*/