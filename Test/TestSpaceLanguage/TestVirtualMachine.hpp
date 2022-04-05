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
#include "VirtualMachine/VirtualMachine.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(Registers, Test)
{
	ASSERT_EQ(Registers::sm_Size, 32);
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