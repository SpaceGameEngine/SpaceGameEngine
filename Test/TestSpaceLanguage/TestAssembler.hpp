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
#include "VirtualMachine/Assembler.h"
#include "VirtualMachine/VirtualMachine.h"
#include "gtest/gtest.h"

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

TEST(Assembler, Test)
{
	Assembler ab;
}