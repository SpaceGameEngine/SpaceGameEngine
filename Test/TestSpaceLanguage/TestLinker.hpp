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
#include "Linker/CompiledObject.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(CompiledGlobalVariable, Test)
{
	CompiledGlobalVariable cgv(8, 4);
	ASSERT_EQ(cgv.GetSize(), 8);
	ASSERT_EQ(cgv.GetAlignment(), 4);
	CompiledGlobalVariable cgv2(8, 4);
	ASSERT_EQ(cgv, cgv2);
	CompiledGlobalVariable cgv3(8, 8);
	ASSERT_NE(cgv, cgv3);
}

TEST(CompiledFunction, Test)
{
	Vector<UInt8> instrs;
	HashMap<UInt64, Vector<UInt64>> gv_map;
	HashMap<UInt64, Vector<UInt64>> func_map;
	InstructionsGenerator::Set(instrs, Register::Common(0), 0);
	gv_map[0].PushBack(2);
	InstructionsGenerator::Goto(instrs, 0);
	func_map[1].PushBack(11);
	CompiledFunction cfunc(instrs, gv_map, func_map);
	ASSERT_EQ(cfunc.GetInstructions(), instrs);
	ASSERT_EQ(cfunc.GetGlobalVariableRequests(), gv_map);
	ASSERT_EQ(cfunc.GetFunctionRequests(), func_map);
	CompiledFunction cfunc2(instrs, gv_map, func_map);
	ASSERT_EQ(cfunc, cfunc2);
	CompiledFunction cfunc3({}, {}, {});
	ASSERT_NE(cfunc, cfunc3);
}

TEST(CompiledObject, Test)
{
	CompiledObject cobj;
	cobj.AddCompiledGlobalVariable(0, CompiledGlobalVariable(8, 4));
	cobj.AddCompiledGlobalVariable(1, CompiledGlobalVariable(0, 0));
	Vector<UInt8> instrs;
	HashMap<UInt64, Vector<UInt64>> gv_map;
	HashMap<UInt64, Vector<UInt64>> func_map;
	InstructionsGenerator::Set(instrs, Register::Common(0), 0);
	gv_map[0].PushBack(2);
	InstructionsGenerator::Goto(instrs, 0);
	func_map[1].PushBack(11);
	cobj.AddCompiledFunction(0, CompiledFunction(instrs, gv_map, func_map));
	cobj.AddCompiledFunction(1, CompiledFunction({}, {}, {}));
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(0)->m_Second, CompiledGlobalVariable(8, 4));
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(1)->m_Second, CompiledGlobalVariable(0, 0));
	ASSERT_EQ(cobj.GetCompiledFunctions().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(0)->m_Second, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(1)->m_Second, CompiledFunction({}, {}, {}));
	ASSERT_TRUE(IsValidCompiledObject(cobj));

	CompiledObject cobj2;
	cobj2.AddCompiledGlobalVariable(0, CompiledGlobalVariable(16, 8));
	cobj2.AddCompiledFunction(0, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_EQ(cobj2.GetCompiledGlobalVariables().GetSize(), 1);
	ASSERT_EQ(cobj2.GetCompiledGlobalVariables().Get(0), CompiledGlobalVariable(16, 8));
	ASSERT_EQ(cobj2.GetCompiledFunctions().GetSize(), 1);
	ASSERT_EQ(cobj2.GetCompiledFunctions().Get(0), CompiledFunction(instrs, gv_map, func_map));
	ASSERT_FALSE(IsValidCompiledObject(cobj2));

	cobj.Replace(cobj2, {{0, 1}}, {{0, 1}});

	ASSERT_EQ(cobj.GetCompiledGlobalVariables().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(0)->m_Second, CompiledGlobalVariable(8, 4));
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(1)->m_Second, CompiledGlobalVariable(16, 8));
	ASSERT_EQ(cobj.GetCompiledFunctions().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(0)->m_Second, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(1)->m_Second, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_TRUE(IsValidCompiledObject(cobj));

	CompiledObject cobj3;
	cobj3.AddCompiledGlobalVariable(0, CompiledGlobalVariable(32, 8));
	cobj3.AddCompiledGlobalVariable(1, CompiledGlobalVariable(64, 16));
	ASSERT_EQ(cobj3.GetCompiledGlobalVariables().GetSize(), 2);
	ASSERT_EQ(cobj3.GetCompiledGlobalVariables().Get(0), CompiledGlobalVariable(32, 8));
	ASSERT_EQ(cobj3.GetCompiledGlobalVariables().Get(1), CompiledGlobalVariable(64, 16));
	ASSERT_EQ(cobj3.GetCompiledFunctions().GetSize(), 0);
	ASSERT_TRUE(IsValidCompiledObject(cobj3));

	cobj.Replace(cobj3, {{0, 1}}, {});

	ASSERT_EQ(cobj.GetCompiledGlobalVariables().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(0)->m_Second, CompiledGlobalVariable(8, 4));
	ASSERT_EQ(cobj.GetCompiledGlobalVariables().Find(1)->m_Second, CompiledGlobalVariable(32, 8));
	ASSERT_EQ(cobj.GetCompiledFunctions().GetSize(), 2);
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(0)->m_Second, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_EQ(cobj.GetCompiledFunctions().Find(1)->m_Second, CompiledFunction(instrs, gv_map, func_map));
	ASSERT_TRUE(IsValidCompiledObject(cobj));
}