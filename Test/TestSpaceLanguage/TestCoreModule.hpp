/*
Copyright 2024 creatorlxd

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
#include "VirtualMachine/ExternalCallerModule/CoreModule.h"
#include "VirtualMachine/Assembler.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::ExternalCallerModule;

inline void LoadTestModule(VirtualMachine& vm, Assembler& assembler)
{
	vm.GetExternalCaller().AddExternalCallFunction(1, 0, [](Registers& regs) -> RegisterType {
		*(UInt64*)regs.Get(Register::Argument(0)) = regs.Get(Register::Argument(1));
		return 0;
	});
	assembler.RegisterExternalCallerModule(SGE_STR("TestModule"), 1, {
																		 {SGE_STR("GetResult"), 0},
																	 });
}

TEST(CoreModule, AllocateTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	UInt64 result = 0;
	auto code = assembler.Compile(String(SGE_STR(R"(
	Set a0 16
	Set a1 16
	ExternalCall a1 CoreModule:Allocate
	Set a0 )")) + ToString<String>((UInt64)(&result)) +
									  SGE_STR(R"(
	ExternalCall c0 TestModule:GetResult
	)"),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_NE(result, 0);
}

TEST(CoreModule, FreeTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	UInt64 result = 0;
	auto code = assembler.Compile(String(SGE_STR(R"(
	Set a0 16
	Set a1 16
	ExternalCall a1 CoreModule:Allocate
	Set a0 )")) + ToString<String>((UInt64)(&result)) +
									  SGE_STR(R"(
	ExternalCall c0 TestModule:GetResult
	Copy a0 a1
	ExternalCall c0 CoreModule:Free
	)"),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_NE(result, 0);
}

TEST(CoreModule, LoadTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	UInt64 result = 0;
	auto code = assembler.Compile(String(SGE_STR(R"(
	Set a0 1
	Set a1 4
	ExternalCall a1 CoreModule:Allocate
	Set a0 )")) + ToString<String>((UInt64)(&result)) +
									  SGE_STR(R"(
	ExternalCall c0 TestModule:GetResult
	)"),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_NE(result, 0);

	*(Int8*)result = 123;

	code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 0x{:H}
		ExternalCall c0 CoreModule:Int8Load
		ExternalCall c1 CoreModule:Free
		Copy a1 c0
		Set a0 0b{:B}
		ExternalCall c0 TestModule:GetResult
				)")),
									result, (UInt64)&result),
							 formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 123);
}

TEST(CoreModule, StoreTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 78
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&result),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 78);
}

TEST(CoreModule, CopyTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 112;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8Copy
	)")),
										 (UInt64)&result, (UInt64)&arg0),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 112);
}

TEST(CoreModule, AddTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 3;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Add
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 15);
}

TEST(CoreModule, SubtractTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 3;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Subtract
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 9);
}

TEST(CoreModule, MultiplyTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 3;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Multiply
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 36);
}

TEST(CoreModule, DivideTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 3;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Divide
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 4);
}

TEST(CoreModule, ModTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 7;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Mod
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 5);
}

TEST(CoreModule, AndTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 7;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8And
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 4);
}

TEST(CoreModule, OrTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 7;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Or
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 15);
}

TEST(CoreModule, XorTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	Int8 arg1 = 7;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8Xor
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 11);
}

TEST(CoreModule, NotTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 12;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8Not
	)")),
										 (UInt64)&result, (UInt64)&arg0),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, -13);
}

TEST(CoreModule, ShiftLeftTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 5;
	Int8 arg1 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8ShiftLeft
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 20);
}

TEST(CoreModule, ShiftRightTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result = 0;
	Int8 arg0 = 5;
	Int8 arg1 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		Set a2 {}
		ExternalCall c0 CoreModule:Int8ShiftRight
	)")),
										 (UInt64)&result, (UInt64)&arg0, (UInt64)&arg1),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, 1);
}

TEST(CoreModule, EqualTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 arg0 = 5;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8Equal
		Set a0 {}
		ExternalCall c1 CoreModule:Int8Equal
		Set a0 {}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 0);
	ASSERT_EQ(result2, 1);
}

TEST(CoreModule, NotEqualTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 arg0 = 5;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8NotEqual
		Set a0 {}
		ExternalCall c1 CoreModule:Int8NotEqual
		Set a0 {}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 1);
	ASSERT_EQ(result2, 0);
}

TEST(CoreModule, LessTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 arg0 = 1;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8Less
		Set a0 {}
		ExternalCall c1 CoreModule:Int8Less
		Set a0 {}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 1);
	ASSERT_EQ(result2, 0);
}

TEST(CoreModule, LessEqualTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 result3 = 0;
	Int8 arg0 = 1;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {0}
		Set a1 {1}
		ExternalCall c0 CoreModule:Int8LessEqual
		Set a0 {2}
		ExternalCall c1 CoreModule:Int8LessEqual
		Set a1 {0}
		ExternalCall c2 CoreModule:Int8LessEqual
		Set a0 {3}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {4}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {5}
		Copy a1 c2
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2, (UInt64)&result3),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 1);
	ASSERT_EQ(result2, 1);
	ASSERT_EQ(result3, 0);
}

TEST(CoreModule, GreaterTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 arg0 = 3;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {}
		Set a1 {}
		ExternalCall c0 CoreModule:Int8Greater
		Set a0 {}
		ExternalCall c1 CoreModule:Int8Greater
		Set a0 {}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 1);
	ASSERT_EQ(result2, 0);
}

TEST(CoreModule, GreaterEqualTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	Int8 result1 = 0;
	Int8 result2 = 0;
	Int8 result3 = 0;
	Int8 arg0 = 3;
	Int8 arg1 = 2;
	Int8 arg2 = 2;
	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {0}
		Set a1 {1}
		ExternalCall c0 CoreModule:Int8GreaterEqual
		Set a0 {2}
		ExternalCall c1 CoreModule:Int8GreaterEqual
		Set a1 {0}
		ExternalCall c2 CoreModule:Int8GreaterEqual
		Set a0 {3}
		Copy a1 c0
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {4}
		Copy a1 c1
		ExternalCall c0 CoreModule:Int8Store
		Set a0 {5}
		Copy a1 c2
		ExternalCall c0 CoreModule:Int8Store
	)")),
										 (UInt64)&arg0, (UInt64)&arg1, (UInt64)&arg2, (UInt64)&result1, (UInt64)&result2, (UInt64)&result3),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result1, 1);
	ASSERT_EQ(result2, 1);
	ASSERT_EQ(result3, 0);
}

TEST(CoreModule, FloatTest)
{
	VirtualMachine vm;
	Assembler assembler;
	String formatter(SGE_STR("line:{} column:{}, {}"));

	CoreModule::LoadIntoExternalCaller(vm.GetExternalCaller());
	CoreModule::LoadIntoAssembler(assembler);

	LoadTestModule(vm, assembler);

	float result = 0.0f;
	float data = 123.45f;

	auto code = assembler.Compile(Format(String(SGE_STR(R"(
		Set a0 {0}
		Set a1 {1}
		ExternalCall c0 CoreModule:UInt32Store
	)")),
										 (UInt64)(&result), *(UInt64*)(&data)),
								  formatter);
	ASSERT_NE(result, data);
	ASSERT_EQ(result, 0.0f);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_EQ(result, data);
	ASSERT_EQ(result, 123.45f);
}