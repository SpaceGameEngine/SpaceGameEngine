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
#include "VirtualMachine/ExternalCallerModule/CoreModule.h"
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

	ASSERT_FALSE(ec.IsHasExternalCallFunction(127, 789));
	ec.AddExternalCallFunction(127, 789, (ExternalCallFunctionType)101112);
	ASSERT_TRUE(ec.IsHasExternalCallFunction(127, 789));
	ASSERT_EQ(ec.GetExternalCallFunction(127, 789), (ExternalCallFunctionType)101112);
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

TEST(InstructionsGenerator, BasicTest)
{
	InstructionsGenerator ig;

	SizeType osize = ig.GetSize();
	ASSERT_EQ(osize, 0);

	ig.AddInstruction(InstructionTypeIndex::GotoRegister, (UInt8)10);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 2);
	ASSERT_EQ(*(UInt8*)ig.GetData(), InstructionTypeIndex::GotoRegister);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 1), 10);

	ig.AddInstruction(InstructionTypeIndex::Copy, (UInt8)11, (UInt8)12);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 5);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 2), InstructionTypeIndex::Copy);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 3), 11);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 4), 12);

	ig.AddInstruction(InstructionTypeIndex::Add, (UInt8)11, (UInt8)12, (UInt8)13);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 9);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 5), InstructionTypeIndex::Add);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 6), 11);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 7), 12);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 8), 13);

	UInt64 data = 123456;
	ig.AddInstruction(InstructionTypeIndex::Goto, data);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 18);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 9), InstructionTypeIndex::Goto);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 10, &data, sizeof(data)), 0);

	data = 654321;
	ig.AddInstruction(InstructionTypeIndex::Set, 14, data);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 28);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 18), InstructionTypeIndex::Set);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 19), 14);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 20, &data, sizeof(data)), 0);
}

TEST(InstructionsGenerator, InstructionTest)
{
	InstructionsGenerator ig;

	SizeType osize = ig.GetSize();
	UInt64 data = 0;
	ASSERT_EQ(osize, 0);

	data = 345;
	ig.ExternalCall(12, data);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 10);
	ASSERT_EQ(*(UInt8*)ig.GetData(), InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 1), 12);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 2, &data, sizeof(data)), 0);

	data = 678;
	ig.Set(13, data);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 20);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 10), InstructionTypeIndex::Set);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 11), 13);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 12, &data, sizeof(data)), 0);

	ig.Copy(14, 13);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 23);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 20), InstructionTypeIndex::Copy);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 21), 14);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 22), 13);

	data = 32;
	ig.Goto(32);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 32);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 23), InstructionTypeIndex::Goto);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 24, &data, sizeof(data)), 0);

	ig.GotoRegister(14);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 34);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 32), InstructionTypeIndex::GotoRegister);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 33), 14);

	data = 32;
	ig.If(15, data);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 44);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 34), InstructionTypeIndex::If);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 35), 15);
	ASSERT_EQ(memcmp((UInt8*)ig.GetData() + 36, &data, sizeof(data)), 0);

	ig.Add(16, 17, 18);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 48);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 44), InstructionTypeIndex::Add);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 45), 16);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 46), 17);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 47), 18);

	ig.Subtract(19, 20, 21);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 52);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 48), InstructionTypeIndex::Subtract);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 49), 19);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 50), 20);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 51), 21);

	ig.Multiply(22, 23, 24);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 56);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 52), InstructionTypeIndex::Multiply);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 53), 22);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 54), 23);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 55), 24);

	ig.Divide(25, 26, 27);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 60);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 56), InstructionTypeIndex::Divide);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 57), 25);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 58), 26);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 59), 27);

	ig.Mod(28, 29, 30);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 64);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 60), InstructionTypeIndex::Mod);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 61), 28);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 62), 29);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 63), 30);

	ig.And(29, 30, 31);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 68);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 64), InstructionTypeIndex::And);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 65), 29);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 66), 30);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 67), 31);

	ig.Or(26, 27, 28);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 72);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 68), InstructionTypeIndex::Or);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 69), 26);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 70), 27);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 71), 28);

	ig.Xor(23, 24, 25);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 76);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 72), InstructionTypeIndex::Xor);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 73), 23);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 74), 24);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 75), 25);

	ig.Not(21, 22);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 79);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 76), InstructionTypeIndex::Not);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 77), 21);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 78), 22);

	ig.ShiftLeft(18, 19, 20);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 83);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 79), InstructionTypeIndex::ShiftLeft);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 80), 18);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 81), 19);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 82), 20);

	ig.ShiftRight(15, 16, 17);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 87);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 83), InstructionTypeIndex::ShiftRight);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 84), 15);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 85), 16);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 86), 17);

	ig.Equal(12, 13, 14);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 91);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 87), InstructionTypeIndex::Equal);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 88), 12);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 89), 13);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 90), 14);

	ig.NotEqual(9, 10, 11);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 95);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 91), InstructionTypeIndex::NotEqual);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 92), 9);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 93), 10);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 94), 11);

	ig.Less(6, 7, 8);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 99);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 95), InstructionTypeIndex::Less);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 96), 6);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 97), 7);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 98), 8);

	ig.LessEqual(9, 10, 11);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 103);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 99), InstructionTypeIndex::LessEqual);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 100), 9);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 101), 10);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 102), 11);

	ig.Greater(12, 13, 14);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 107);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 103), InstructionTypeIndex::Greater);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 104), 12);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 105), 13);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 106), 14);

	ig.GreaterEqual(15, 16, 17);
	osize = ig.GetSize();
	ASSERT_EQ(osize, 111);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 107), InstructionTypeIndex::GreaterEqual);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 108), 15);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 109), 16);
	ASSERT_EQ(*((UInt8*)ig.GetData() + 110), 17);
}

TEST(InstructionsGenerator, CopyTest)
{
	InstructionsGenerator ig_src;
	ig_src.ExternalCall(123, 456);
	ig_src.Copy(123, 456);
	ASSERT_EQ(ig_src.GetSize(), 13);

	InstructionsGenerator ig_dst1(ig_src);
	ASSERT_EQ(ig_dst1.GetSize(), 13);
	ASSERT_EQ(memcmp(ig_dst1.GetData(), ig_src.GetData(), 13), 0);

	InstructionsGenerator ig_dst2;
	ig_dst2.Equal(0, 1, 2);
	ASSERT_EQ(ig_dst2.GetSize(), 4);

	ig_dst2 = ig_src;

	ASSERT_EQ(ig_dst2.GetSize(), 13);
	ASSERT_EQ(memcmp(ig_dst2.GetData(), ig_src.GetData(), 13), 0);
}

TEST(InstructionsGenerator, MoveTest)
{
	InstructionsGenerator ig_src1;
	ig_src1.ExternalCall(123, 456);
	ig_src1.Copy(123, 456);
	ASSERT_EQ(ig_src1.GetSize(), 13);

	InstructionsGenerator ig_src2(ig_src1);
	ASSERT_EQ(ig_src2.GetSize(), 13);
	ASSERT_EQ(memcmp(ig_src2.GetData(), ig_src1.GetData(), 13), 0);

	InstructionsGenerator ig_dst1(std::move(ig_src1));
	ASSERT_EQ(ig_dst1.GetSize(), 13);
	ASSERT_EQ(memcmp(ig_dst1.GetData(), ig_src2.GetData(), 13), 0);

	InstructionsGenerator ig_dst2;
	ig_dst2.Equal(0, 1, 2);
	ASSERT_EQ(ig_dst2.GetSize(), 4);

	ig_dst2 = std::move(ig_src2);

	ASSERT_EQ(ig_dst2.GetSize(), 13);
	ASSERT_EQ(memcmp(ig_dst2.GetData(), ig_dst1.GetData(), 13), 0);
}

TEST(VirtualMachine, Test)
{
	VirtualMachine vm;
	vm.GetExternalCaller().AddExternalCallFunction(1, 0, [](RegisterType& r1, RegisterType& r2, RegisterType& r3) -> RegisterType {
		*(UInt64*)r1 = r2;
		return 1;
	});
	UInt8 code[94];
	UInt64 data;
	UInt64 result = 0;
	/*
	for(i:0->10)
		sum+=i;
	*/
	code[0] = InstructionTypeIndex::Set;
	code[1] = 6;
	data = 0;
	memcpy(code + 2, &data, sizeof(data));

	code[10] = InstructionTypeIndex::Set;
	code[11] = 7;
	data = 1;
	memcpy(code + 12, &data, sizeof(data));

	code[20] = InstructionTypeIndex::Set;
	code[21] = 8;
	data = 0;
	memcpy(code + 22, &data, sizeof(data));

	code[30] = InstructionTypeIndex::Set;
	code[31] = 9;
	data = 10;
	memcpy(code + 32, &data, sizeof(data));

	code[40] = InstructionTypeIndex::Add;
	code[41] = 6;
	code[42] = 6;
	code[43] = 7;

	code[44] = InstructionTypeIndex::Greater;
	code[45] = 10;
	code[46] = 6;
	code[47] = 9;

	code[48] = InstructionTypeIndex::If;
	code[49] = 10;
	data = 71;
	memcpy(code + 50, &data, sizeof(data));

	code[58] = InstructionTypeIndex::Add;
	code[59] = 8;
	code[60] = 8;
	code[61] = 6;

	code[62] = InstructionTypeIndex::Goto;
	data = 40;
	memcpy(code + 63, &data, sizeof(data));

	code[71] = InstructionTypeIndex::Set;
	code[72] = SpecialRegister::Argument0;
	data = (UInt64)&result;
	memcpy(code + 73, &data, sizeof(data));

	code[81] = InstructionTypeIndex::Copy;
	code[82] = SpecialRegister::Argument1;
	code[83] = 8;

	code[84] = InstructionTypeIndex::ExternalCall;
	code[85] = 11;
	data = ExternalCaller::GetIndex(1, 0);
	memcpy(code + 86, &data, sizeof(data));

	vm.Run(code, sizeof(code));

	ASSERT_EQ(result, 55);
}

TEST(VirtualMachine, InstructionsGeneratorTest)
{
	VirtualMachine vm;
	vm.GetExternalCaller().AddExternalCallFunction(1, 0, [](RegisterType& r1, RegisterType& r2, RegisterType& r3) -> RegisterType {
		*(UInt64*)r1 = r2;
		return 1;
	});
	InstructionsGenerator ig;
	UInt64 result = 0;

	/*
	for(i:0->10)
		sum+=i;
	*/
	ig.Set(6, 0);
	ig.Set(7, 1);
	ig.Set(8, 0);
	ig.Set(9, 10);
	ig.Add(6, 6, 7);
	ig.Greater(10, 6, 9);
	ig.If(10, 71);
	ig.Add(8, 8, 6);
	ig.Goto(40);
	ig.Set(SpecialRegister::Argument0, (UInt64)&result);
	ig.Copy(SpecialRegister::Argument1, 8);
	ig.ExternalCall(11, ExternalCaller::GetIndex(1, 0));

	ASSERT_EQ(ig.GetSize(), 94);

	vm.Run(ig.GetData(), ig.GetSize());

	ASSERT_EQ(result, 55);
}