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
#include "gtest/gtest.h"
#include "VirtualMachine/Assembler.h"
#include "VirtualMachine/VirtualMachine.h"
#include "Utility/Format.hpp"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(InstructionNameSet, Test)
{
	InstructionNameSet& ins = InstructionNameSet::GetSingleton();
	ASSERT_TRUE(ins.IsInstructionName(SGE_STR("Add")));
	ASSERT_FALSE(ins.IsInstructionName(SGE_STR("")));
	ASSERT_FALSE(ins.IsInstructionName(SGE_STR("test")));

	const auto& test_instr = ins.Get(SGE_STR("And"));
	ASSERT_EQ(test_instr.m_Index, 11);
	ASSERT_EQ(test_instr.m_Name, SGE_STR("And"));
	ASSERT_EQ(test_instr.m_Size, 4);
}

TEST(RegisterNameSet, Test)
{
	RegisterNameSet& rns = RegisterNameSet::GetSingleton();
	ASSERT_TRUE(rns.IsRegisterName(SGE_STR("pc")));
	ASSERT_TRUE(rns.IsRegisterName(SGE_STR("bp")));
	ASSERT_TRUE(rns.IsRegisterName(SGE_STR("sp")));
	ASSERT_TRUE(rns.IsRegisterName(SGE_STR("c1")));
	ASSERT_TRUE(rns.IsRegisterName(SGE_STR("a2")));
	ASSERT_FALSE(rns.IsRegisterName(SGE_STR("test")));

	ASSERT_EQ(rns.Get(SGE_STR("pc")), Register::ProgramCounter);
	ASSERT_EQ(rns.Get(SGE_STR("bp")), Register::BasePointer);
	ASSERT_EQ(rns.Get(SGE_STR("sp")), Register::StackPointer);
	ASSERT_EQ(rns.Get(SGE_STR("c0")), SpecialRegistersSize);
	ASSERT_EQ(rns.Get(Format(String(SGE_STR("c{}")), ArgumentRegistersStartIndex - 1 - SpecialRegistersSize)), ArgumentRegistersStartIndex - 1);
	ASSERT_EQ(rns.Get(SGE_STR("a0")), ArgumentRegistersStartIndex);
	ASSERT_EQ(rns.Get(Format(String(SGE_STR("a{}")), RegistersSize - 1 - ArgumentRegistersStartIndex)), RegistersSize - 1);
}

TEST(Assembler, Test)
{
	Assembler ab;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	auto res1 = ab.Compile(SGE_STR(R"(
	Set c0 123456/*test cross line comment block
	*/Copy c1 c0
	//test assembler 1
	)"),
						   formatter);
	ASSERT_EQ(res1.GetSize(), 13);
	ASSERT_EQ(res1[0], InstructionTypeIndex::Set);
	ASSERT_EQ(res1[1], Register::Common(0));
	ASSERT_EQ(*(UInt64*)(&res1[2]), 123456);
	ASSERT_EQ(res1[10], InstructionTypeIndex::Copy);
	ASSERT_EQ(res1[11], Register::Common(1));
	ASSERT_EQ(res1[12], Register::Common(0));

	auto res2 = ab.Compile(SGE_STR(R"(//test label
	:Start
	If 10 Then
	Goto Else
	:Else//19
	Copy 10 11
	Goto Then
	:Then//31
	Copy 12 10
	Goto Start
	)"),
						   formatter);
	ASSERT_EQ(res2.GetSize(), 43);
	ASSERT_EQ(res2[0], InstructionTypeIndex::If);
	ASSERT_EQ(res2[1], 10);
	ASSERT_EQ(*(UInt64*)(&res2[2]), 31);
	ASSERT_EQ(res2[10], InstructionTypeIndex::Goto);
	ASSERT_EQ(*(UInt64*)(&res2[11]), 19);
	ASSERT_EQ(res2[19], InstructionTypeIndex::Copy);
	ASSERT_EQ(res2[20], 10);
	ASSERT_EQ(res2[21], 11);
	ASSERT_EQ(res2[22], InstructionTypeIndex::Goto);
	ASSERT_EQ(*(UInt64*)(&res2[23]), 31);
	ASSERT_EQ(res2[31], InstructionTypeIndex::Copy);
	ASSERT_EQ(res2[32], 12);
	ASSERT_EQ(res2[33], 10);
	ASSERT_EQ(res2[34], InstructionTypeIndex::Goto);
	ASSERT_EQ(*(UInt64*)(&res2[35]), 0);

	ab.RegisterExternalCallerModule(SGE_STR("TestModule"), 123, {{SGE_STR("Test1"), 1}, {SGE_STR("Test2"), 2}});
	auto res3 = ab.Compile(SGE_STR(R"(//test external call
	ExternalCall 10 123456
	ExternalCall 11 TestModule:Test1
	ExternalCall 12 TestModule:Test2
	)"),
						   formatter);

	ASSERT_EQ(res3.GetSize(), 30);
	ASSERT_EQ(res3[0], InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(res3[1], 10);
	ASSERT_EQ(*(UInt64*)(&res3[2]), 123456);
	ASSERT_EQ(res3[10], InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(res3[11], 11);
	ASSERT_EQ(*(UInt64*)(&res3[12]), ExternalCaller::GetIndex(123, 1));
	ASSERT_EQ(res3[20], InstructionTypeIndex::ExternalCall);
	ASSERT_EQ(res3[21], 12);
	ASSERT_EQ(*(UInt64*)(&res3[22]), ExternalCaller::GetIndex(123, 2));

	ab.RegisterExternalCallerModule(SGE_STR("TestRun"), 1, {{SGE_STR("GetResult"), 0}});
	VirtualMachine vm;
	vm.GetExternalCaller().AddExternalCallFunction(1, 0, [](Registers& regs) -> RegisterType {
		*(UInt64*)regs.Get(Register::Argument(0)) = regs.Get(Register::Argument(1));
		return 1;
	});
	UInt64 result = 0;
	auto res4 = ab.Compile(Format(String(SGE_STR(R"(
		Set c0 0
		Set c1 1
		Set c2 0
		Set c3 10
:Loop	Add c0 c0 c1
		Greater c4 c0 c3
		If c4 LoopEnd
		Add c2 c2 c0
		Goto Loop
:LoopEnd
		Set a0 {}
		Copy a1 c2
		ExternalCall c0 TestRun:GetResult
	)")),
								  (UInt64)&result),
						   formatter);
	vm.Run(res4.GetData(), res4.GetSize());
	ASSERT_EQ(result, 55);
}