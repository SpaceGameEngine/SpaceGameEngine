/*
Copyright 2022 creatorlxd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License InstructionSet::GetSingleton() distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include "VirtualMachine/VirtualMachine.h"
#include "gtest/gtest.h"
#include <cstring>

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(Registers, Test)
{
	ASSERT_EQ(RegistersSize, 32);
	const Registers cregs;
	ASSERT_EQ(cregs.Get(31), 0);
	Registers regs;
	ASSERT_EQ(regs.Get(31), 0);
	regs.Get(31) = 125;
	ASSERT_EQ(regs.Get(31), 125);
}

TEST(ExternalCaller, Test)
{
	ExternalCaller ec;
	ASSERT_FALSE(ec.IsHasExternalCallFunction(123));
	ec.AddExternalCallFunction(123, (ExternalCallFunctionType)456);
	ASSERT_TRUE(ec.IsHasExternalCallFunction(123));
	ASSERT_EQ(ec.GetExternalCallFunction(123), (ExternalCallFunctionType)456);
}

TEST(InstructionSet, ExternalCallTest)
{
	ExternalCaller ec;
	Registers regs;
	ec.AddExternalCallFunction(123, [](RegisterType&, RegisterType&, RegisterType&) -> RegisterType {
		return 456;
	});
	const auto& ext_call = InstructionSet::GetSingleton().Get(InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(ext_call.m_Index, 0);
	ASSERT_EQ(ext_call.m_Index, InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(ext_call.m_Name, SGE_STR("ExternalCall"));
	ASSERT_EQ(ext_call.m_Size, 10);

	Byte test_args[9];

	test_args[0] = 31;
	UInt64 ext_call_idx = 123;
	memcpy(test_args + 1, &ext_call_idx, sizeof(ext_call_idx));
	ext_call.m_pFunction(regs, test_args, ec);

	ASSERT_EQ(regs.Get(31), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);
}

TEST(InstructionSet, SetTest)
{
	ExternalCaller ec;
	Registers regs;
	ec.AddExternalCallFunction(123, [](RegisterType& r1, RegisterType& r2, RegisterType& r3) -> RegisterType {
		r3 = r1 + r2;
		return 1;
	});
	const auto& set = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set);
	ASSERT_EQ(set.m_Index, 1);
	ASSERT_EQ(set.m_Index, InstructionTypeIndex::Set);
	ASSERT_EQ(set.m_Name, SGE_STR("Set"));
	ASSERT_EQ(set.m_Size, 10);

	Byte test_args[9];

	test_args[0] = SpecialRegister::Argument0;
	UInt64 buffer = 2;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	set.m_pFunction(regs, test_args, ec);

	test_args[0] = SpecialRegister::Argument1;
	buffer = 3;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	set.m_pFunction(regs, test_args, ec);

	test_args[0] = 31;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::ExternalCall).m_pFunction(regs, test_args, ec);

	ASSERT_EQ(regs.Get(SpecialRegister::Argument2), 5);
	ASSERT_EQ(regs.Get(31), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);
}

TEST(InstructionSet, CopyTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& copy = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Copy);
	ASSERT_EQ(copy.m_Index, 2);
	ASSERT_EQ(copy.m_Index, InstructionTypeIndex::Copy);
	ASSERT_EQ(copy.m_Name, SGE_STR("Copy"));
	ASSERT_EQ(copy.m_Size, 3);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 123);
	ASSERT_EQ(regs.Get(11), 0);

	test_args[0] = 11;
	test_args[1] = 10;
	copy.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 123);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 13);
}

TEST(InstructionSet, GotoTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& goto_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Goto);
	ASSERT_EQ(goto_.m_Index, 3);
	ASSERT_EQ(goto_.m_Index, InstructionTypeIndex::Goto);
	ASSERT_EQ(goto_.m_Name, SGE_STR("Goto"));
	ASSERT_EQ(goto_.m_Size, 9);

	Byte test_args[9];

	UInt64 buffer = 123;
	memcpy(test_args, &buffer, sizeof(buffer));

	goto_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 123);
}

TEST(InstructionSet, GotoRegisterTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& goto_reg = InstructionSet::GetSingleton().Get(InstructionTypeIndex::GotoRegister);
	ASSERT_EQ(goto_reg.m_Index, 4);
	ASSERT_EQ(goto_reg.m_Index, InstructionTypeIndex::GotoRegister);
	ASSERT_EQ(goto_reg.m_Name, SGE_STR("GotoRegister"));
	ASSERT_EQ(goto_reg.m_Size, 2);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	goto_reg.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 123);
}

TEST(InstructionSet, IfTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& if_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::If);
	ASSERT_EQ(if_.m_Index, 5);
	ASSERT_EQ(if_.m_Index, InstructionTypeIndex::If);
	ASSERT_EQ(if_.m_Name, SGE_STR("If"));
	ASSERT_EQ(if_.m_Size, 10);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	if_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 10;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	if_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 123);
}

TEST(InstructionSet, AddTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& add = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Add);
	ASSERT_EQ(add.m_Index, 6);
	ASSERT_EQ(add.m_Index, InstructionTypeIndex::Add);
	ASSERT_EQ(add.m_Name, SGE_STR("Add"));
	ASSERT_EQ(add.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	add.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 123 + 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, SubtractTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& subtract = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Subtract);
	ASSERT_EQ(subtract.m_Index, 7);
	ASSERT_EQ(subtract.m_Index, InstructionTypeIndex::Subtract);
	ASSERT_EQ(subtract.m_Name, SGE_STR("Subtract"));
	ASSERT_EQ(subtract.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	subtract.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 - 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, MultiplyTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& multiply = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Multiply);
	ASSERT_EQ(multiply.m_Index, 8);
	ASSERT_EQ(multiply.m_Index, InstructionTypeIndex::Multiply);
	ASSERT_EQ(multiply.m_Name, SGE_STR("Multiply"));
	ASSERT_EQ(multiply.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	multiply.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 * 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, DivideTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& divide = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Divide);
	ASSERT_EQ(divide.m_Index, 9);
	ASSERT_EQ(divide.m_Index, InstructionTypeIndex::Divide);
	ASSERT_EQ(divide.m_Name, SGE_STR("Divide"));
	ASSERT_EQ(divide.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	divide.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 / 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, ModTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& mod = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Mod);
	ASSERT_EQ(mod.m_Index, 10);
	ASSERT_EQ(mod.m_Index, InstructionTypeIndex::Mod);
	ASSERT_EQ(mod.m_Name, SGE_STR("Mod"));
	ASSERT_EQ(mod.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	mod.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 % 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, AndTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& and_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::And);
	ASSERT_EQ(and_.m_Index, 11);
	ASSERT_EQ(and_.m_Index, InstructionTypeIndex::And);
	ASSERT_EQ(and_.m_Name, SGE_STR("And"));
	ASSERT_EQ(and_.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	and_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 & 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, OrTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& or_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Or);
	ASSERT_EQ(or_.m_Index, 12);
	ASSERT_EQ(or_.m_Index, InstructionTypeIndex::Or);
	ASSERT_EQ(or_.m_Name, SGE_STR("Or"));
	ASSERT_EQ(or_.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	or_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 | 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, XorTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& xor_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Xor);
	ASSERT_EQ(xor_.m_Index, 13);
	ASSERT_EQ(xor_.m_Index, InstructionTypeIndex::Xor);
	ASSERT_EQ(xor_.m_Name, SGE_STR("Xor"));
	ASSERT_EQ(xor_.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	xor_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 ^ 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, NotTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& not_ = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Not);
	ASSERT_EQ(not_.m_Index, 14);
	ASSERT_EQ(not_.m_Index, InstructionTypeIndex::Not);
	ASSERT_EQ(not_.m_Name, SGE_STR("Not"));
	ASSERT_EQ(not_.m_Size, 3);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	test_args[1] = 10;
	not_.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), ~456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 13);
}

TEST(InstructionSet, ShiftLeftTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& shift_left = InstructionSet::GetSingleton().Get(InstructionTypeIndex::ShiftLeft);
	ASSERT_EQ(shift_left.m_Index, 15);
	ASSERT_EQ(shift_left.m_Index, InstructionTypeIndex::ShiftLeft);
	ASSERT_EQ(shift_left.m_Name, SGE_STR("ShiftLeft"));
	ASSERT_EQ(shift_left.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 12;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 12);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	shift_left.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 << 12);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, ShiftRightTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& shift_right = InstructionSet::GetSingleton().Get(InstructionTypeIndex::ShiftRight);
	ASSERT_EQ(shift_right.m_Index, 16);
	ASSERT_EQ(shift_right.m_Index, InstructionTypeIndex::ShiftRight);
	ASSERT_EQ(shift_right.m_Name, SGE_STR("ShiftRight"));
	ASSERT_EQ(shift_right.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 2;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 2);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	test_args[1] = 10;
	test_args[2] = 11;
	shift_right.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456 >> 2);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 24);
}

TEST(InstructionSet, EqualTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& equal = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Equal);
	ASSERT_EQ(equal.m_Index, 17);
	ASSERT_EQ(equal.m_Index, InstructionTypeIndex::Equal);
	ASSERT_EQ(equal.m_Name, SGE_STR("Equal"));
	ASSERT_EQ(equal.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 11;
	equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 34);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 12;
	equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 38);
}

TEST(InstructionSet, NotEqualTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& not_equal = InstructionSet::GetSingleton().Get(InstructionTypeIndex::NotEqual);
	ASSERT_EQ(not_equal.m_Index, 18);
	ASSERT_EQ(not_equal.m_Index, InstructionTypeIndex::NotEqual);
	ASSERT_EQ(not_equal.m_Name, SGE_STR("NotEqual"));
	ASSERT_EQ(not_equal.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 11;
	not_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 34);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 12;
	not_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 38);
}

TEST(InstructionSet, LessTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& less = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Less);
	ASSERT_EQ(less.m_Index, 19);
	ASSERT_EQ(less.m_Index, InstructionTypeIndex::Less);
	ASSERT_EQ(less.m_Name, SGE_STR("Less"));
	ASSERT_EQ(less.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 789;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 789);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 11;
	less.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 34);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 12;
	less.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 38);
}

TEST(InstructionSet, LessEqualTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& less_equal = InstructionSet::GetSingleton().Get(InstructionTypeIndex::LessEqual);
	ASSERT_EQ(less_equal.m_Index, 20);
	ASSERT_EQ(less_equal.m_Index, InstructionTypeIndex::LessEqual);
	ASSERT_EQ(less_equal.m_Name, SGE_STR("LessEqual"));
	ASSERT_EQ(less_equal.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	buffer = 789;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 789);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 40);

	test_args[0] = 14;
	test_args[1] = 10;
	test_args[2] = 11;
	less_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(14), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 44);

	test_args[0] = 14;
	test_args[1] = 10;
	test_args[2] = 12;
	less_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(14), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 48);

	test_args[0] = 15;
	test_args[1] = 10;
	test_args[2] = 13;
	less_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(15), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 52);
}

TEST(InstructionSet, GreaterTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& greater = InstructionSet::GetSingleton().Get(InstructionTypeIndex::Greater);
	ASSERT_EQ(greater.m_Index, 21);
	ASSERT_EQ(greater.m_Index, InstructionTypeIndex::Greater);
	ASSERT_EQ(greater.m_Name, SGE_STR("Greater"));
	ASSERT_EQ(greater.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 789;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 789);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 11;
	greater.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 34);

	test_args[0] = 13;
	test_args[1] = 10;
	test_args[2] = 12;
	greater.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 38);
}

TEST(InstructionSet, GreaterEqualTest)
{
	ExternalCaller ec;
	Registers regs;

	const auto& greater_equal = InstructionSet::GetSingleton().Get(InstructionTypeIndex::GreaterEqual);
	ASSERT_EQ(greater_equal.m_Index, 22);
	ASSERT_EQ(greater_equal.m_Index, InstructionTypeIndex::GreaterEqual);
	ASSERT_EQ(greater_equal.m_Name, SGE_STR("GreaterEqual"));
	ASSERT_EQ(greater_equal.m_Size, 4);

	Byte test_args[9];

	test_args[0] = 10;
	UInt64 buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(10), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 10);

	test_args[0] = 11;
	buffer = 123;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(11), 123);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 20);

	test_args[0] = 12;
	buffer = 456;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(12), 456);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 30);

	test_args[0] = 13;
	buffer = 789;
	memcpy(test_args + 1, &buffer, sizeof(buffer));
	InstructionSet::GetSingleton().Get(InstructionTypeIndex::Set).m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(13), 789);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 40);

	test_args[0] = 14;
	test_args[1] = 10;
	test_args[2] = 11;
	greater_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(14), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 44);

	test_args[0] = 15;
	test_args[1] = 10;
	test_args[2] = 12;
	greater_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(15), 1);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 48);

	test_args[0] = 15;
	test_args[1] = 10;
	test_args[2] = 13;
	greater_equal.m_pFunction(regs, test_args, ec);
	ASSERT_EQ(regs.Get(15), 0);
	ASSERT_EQ(regs.Get(SpecialRegister::ProgramCounter), 52);
}

TEST(VirtualMachine, Test)
{
	VirtualMachine vm;
}