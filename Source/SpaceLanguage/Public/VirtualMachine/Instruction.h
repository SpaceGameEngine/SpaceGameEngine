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
#include "Utility/Singleton.hpp"
#include "Container/Vector.hpp"

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

	class SPACE_LANGUAGE_API InstructionsGenerator
	{
	public:
		void AddInstruction(UInt8 instr_idx, UInt8 arg0);
		void AddInstruction(UInt8 instr_idx, UInt8 arg0, UInt8 arg1);
		void AddInstruction(UInt8 instr_idx, UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void AddInstruction(UInt8 instr_idx, UInt64 data);
		void AddInstruction(UInt8 instr_idx, UInt8 arg0, UInt64 data);

		const void* GetData() const;
		SizeType GetSize() const;
		const Vector<UInt8>& GetVector() const;

		void ExternalCall(UInt8 arg0, UInt64 data);
		void Set(UInt8 arg0, UInt64 data);
		void Copy(UInt8 arg0, UInt8 arg1);
		void Goto(UInt64 data);
		void GotoRegister(UInt8 arg0);
		void If(UInt8 arg0, UInt64 data);

		void Add(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Subtract(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Multiply(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Divide(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Mod(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void And(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Or(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Xor(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Not(UInt8 arg0, UInt8 arg1);
		void ShiftLeft(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void ShiftRight(UInt8 arg0, UInt8 arg1, UInt8 arg2);

		void Equal(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void NotEqual(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Less(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void LessEqual(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void Greater(UInt8 arg0, UInt8 arg1, UInt8 arg2);
		void GreaterEqual(UInt8 arg0, UInt8 arg1, UInt8 arg2);

	private:
		Vector<UInt8> m_Content;
	};

	/*!
	@}
	*/
}