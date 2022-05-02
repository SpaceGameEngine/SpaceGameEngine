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
	Set 32 16
	Set 33 16
	ExternalCall 33 CoreModule:Allocate
	Set 32 )")) + ToString<String>((UInt64)(&result)) +
									  SGE_STR(R"(
	ExternalCall 10 TestModule:GetResult
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
	Set 32 16
	Set 33 16
	ExternalCall 33 CoreModule:Allocate
	Set 32 )")) + ToString<String>((UInt64)(&result)) +
									  SGE_STR(R"(
	ExternalCall 10 TestModule:GetResult
	Copy 32 33
	ExternalCall 10 CoreModule:Free
	)"),
								  formatter);
	vm.Run(code.GetData(), code.GetSize());
	ASSERT_NE(result, 0);
}