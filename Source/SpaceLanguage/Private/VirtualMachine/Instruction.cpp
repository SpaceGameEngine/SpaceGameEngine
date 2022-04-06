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
#include "VirtualMachine/Instruction.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

SpaceGameEngine::SpaceLanguage::InstructionType::InstructionType()
	: m_Index(0), m_Name(), m_Size(0), m_pFunction(nullptr)
{
}

SpaceGameEngine::SpaceLanguage::InstructionType::InstructionType(UInt8 index, const String& name, UInt8 size, InstructionFunctionType pfunc)
	: m_Index(index), m_Name(name), m_Size(size), m_pFunction(pfunc)
{
	SGE_ASSERT(InvalidValueError, index, 0, InstructionSetSize - 1);
	SGE_ASSERT(String::EmptyStringCoreError, name.GetSize());
	SGE_ASSERT(InvalidValueError, size, 1, MaxInstructionSize);
	SGE_ASSERT(NullPointerError, pfunc);
}

SpaceGameEngine::SpaceLanguage::InstructionSet::InstructionSet()
{
#define DEF_INSTR(idx, name, size, pfunc) m_InstructionTypes[idx] = InstructionType(idx, name, size, pfunc);
	DEF_INSTR(0, SGE_STR("ExternalCall"), 10, ExternalCall);
	DEF_INSTR(1, SGE_STR("Set"), 10, Set);
	DEF_INSTR(2, SGE_STR("Copy"), 3, Copy);
	DEF_INSTR(3, SGE_STR("Goto"), 9, Goto);
	DEF_INSTR(4, SGE_STR("GotoRegister"), 2, GotoRegister);
	DEF_INSTR(5, SGE_STR("If"), 10, If);
	DEF_INSTR(6, SGE_STR("Add"), 4, Add);
	DEF_INSTR(7, SGE_STR("Subtract"), 4, Subtract);
	DEF_INSTR(8, SGE_STR("Multiply"), 4, Multiply);
	DEF_INSTR(9, SGE_STR("Divide"), 4, Divide);
	DEF_INSTR(10, SGE_STR("Mod"), 4, Mod);
	DEF_INSTR(11, SGE_STR("And"), 4, And);
	DEF_INSTR(12, SGE_STR("Or"), 4, Or);
	DEF_INSTR(13, SGE_STR("Xor"), 4, Xor);
	DEF_INSTR(14, SGE_STR("Not"), 3, Not);
	DEF_INSTR(15, SGE_STR("ShiftLeft"), 4, ShiftLeft);
	DEF_INSTR(16, SGE_STR("ShiftRight"), 4, ShiftRight);
	DEF_INSTR(17, SGE_STR("Equal"), 4, Equal);
	DEF_INSTR(18, SGE_STR("NotEqual"), 4, NotEqual);
	DEF_INSTR(19, SGE_STR("Less"), 4, Less);
	DEF_INSTR(20, SGE_STR("LessEqual"), 4, LessEqual);
	DEF_INSTR(21, SGE_STR("Greater"), 4, Greater);
	DEF_INSTR(22, SGE_STR("GreaterEqual"), 4, GreaterEqual);
}

const InstructionType& SpaceGameEngine::SpaceLanguage::InstructionSet::Get(UInt8 index) const
{
	SGE_ASSERT(InvalidValueError, index, 0, InstructionSetSize - 1);
	return m_InstructionTypes[index];
}

#define PARGS_CAST(offset, type) *(type*)((AddressType)pargs + offset)
#define ARG0 *(UInt8*)(pargs)
#define ARG1 PARGS_CAST(1, UInt8)
#define ARG2 PARGS_CAST(2, UInt8)
#define DATA0 PARGS_CAST(0, UInt64)
#define DATA1 PARGS_CAST(1, UInt64)

#define EC_ARG0 regs.Get(SpecialRegister::Argument0)
#define EC_ARG1 regs.Get(SpecialRegister::Argument1)
#define EC_ARG2 regs.Get(SpecialRegister::Argument2)
#define PC regs.Get(SpecialRegister::ProgramCounter)
#define R_ARG0 regs.Get(ARG0)
#define R_ARG1 regs.Get(ARG1)
#define R_ARG2 regs.Get(ARG2)

void SpaceGameEngine::SpaceLanguage::InstructionSet::ExternalCall(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = ext_caller.GetExternalCallFunction(DATA1)(EC_ARG0, EC_ARG1, EC_ARG2);
	PC += 10;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Set(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = DATA1;
	PC += 10;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Copy(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1;
	PC += 3;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Goto(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	PC = DATA0;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::GotoRegister(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	PC = R_ARG0;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::If(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	if (R_ARG0)
		PC = DATA1;
	else
		PC += 10;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Add(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 + R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Subtract(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 - R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Multiply(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 * R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Divide(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 / R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Mod(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 % R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::And(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 & R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Or(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 | R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Xor(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 ^ R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Not(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = ~R_ARG1;
	PC += 3;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::ShiftLeft(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 << R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::ShiftRight(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = R_ARG1 >> R_ARG2;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Equal(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 == R_ARG2) ? 1 : 0;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::NotEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 != R_ARG2) ? 1 : 0;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Less(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 < R_ARG2) ? 1 : 0;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::LessEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 <= R_ARG2) ? 1 : 0;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::Greater(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 > R_ARG2) ? 1 : 0;
	PC += 4;
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::GreaterEqual(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
	SGE_ASSERT(NullPointerError, pargs);
	R_ARG0 = (R_ARG1 >= R_ARG2) ? 1 : 0;
	PC += 4;
}
