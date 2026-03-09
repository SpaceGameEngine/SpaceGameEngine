/*
Copyright 2026 creatorlxd

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
#include "Utility/DebugInformation.h"
#include "Utility/Pair.hpp"
#include "SGEString.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

DebugInformation TestDebugInformation()
{
	return DebugInformation(SGE_DEBUG_INFORMATION);
}

TEST(DebugInformation, DebugInformationTest)
{
	DebugInformation di = TestDebugInformation();
	TString<> filename(di.m_pFileName);
	TString<> funcname(di.m_pFunctionName);
	ASSERT_NE(filename.Find(SGE_TSTR("TestDebugInformation.hpp"), filename.GetConstBegin(), filename.GetConstEnd()), filename.GetConstEnd());
	ASSERT_NE(funcname.Find(SGE_TSTR("TestDebugInformation"), funcname.GetConstBegin(), funcname.GetConstEnd()), funcname.GetConstEnd());
	ASSERT_EQ(di.m_LineNumber, 26);
}