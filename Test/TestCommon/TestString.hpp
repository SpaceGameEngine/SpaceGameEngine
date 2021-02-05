/*
Copyright 2021 creatorlxd

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
#include "../../Source/Common/Private/StringImplement/StdTStringImplement.h"
#include "SGEString.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(String, StdTStringConvertTest)
{
	String str = StdTString(SGE_TSTR("TestString"));
	ASSERT_EQ(str, StdTString(SGE_TSTR("TestString")));
	ASSERT_EQ(str.GetSize(), StdTString(SGE_TSTR("TestString")).size());
}
TEST(String, TCharConvertTest)
{
	String str = SGE_TSTR("TestString");
	ASSERT_EQ(memcmp(str.GetData(), SGE_TSTR("TestString"), str.GetSize()), 0);
	//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
	ASSERT_EQ(str.GetSize(), (sizeof(SGE_TSTR("TestString")) / sizeof(TChar)) - 1);
}
TEST(String, CopyTest)
{
	String str = SGE_TSTR("TestString");
	String str2 = str;
	ASSERT_EQ(str, str2);
	// if str's implement is equal with str2's then their raw data address must be same
	ASSERT_NE(str.GetData(), str2.GetData());
}
TEST(String, MoveTest)
{
	String str = SGE_TSTR("TestString");
	String str2 = SGE_TSTR("TestString");
	String str3(std::move(str));
	String str4 = std::move(str2);
	ASSERT_EQ(str3, str4);
}
TEST(String, ToCStringTest)
{
	String str = SGE_TSTR("TestString");
	ASSERT_EQ(str.ToCString()[str.GetSize()], '\0');
}

TEST(StdTStringImplement, StdTStringConvertTest)
{
	StdTStringImplement str = StdTString(SGE_TSTR("TestString"));
	ASSERT_EQ(str, StdTString(SGE_TSTR("TestString")));
	ASSERT_EQ(str.GetSize(), StdTString(SGE_TSTR("TestString")).size());
}
TEST(StdTStringImplement, TCharConvertTest)
{
	StdTStringImplement str = SGE_TSTR("TestString");
	ASSERT_EQ(memcmp(str.GetData(), SGE_TSTR("TestString"), str.GetSize()), 0);
	//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
	ASSERT_EQ(str.GetSize(), (sizeof(SGE_TSTR("TestString")) / sizeof(TChar)) - 1);
}
TEST(StdTStringImplement, CopyTest)
{
	StdTStringImplement str = SGE_TSTR("TestString");
	StdTStringImplement str2 = str;
	ASSERT_EQ(str, str2);
	ASSERT_NE(str.GetData(), str2.GetData());
}
TEST(StdTStringImplement, MoveTest)
{
	StdTStringImplement str = SGE_TSTR("TestString");
	StdTStringImplement str2 = SGE_TSTR("TestString");
	StdTStringImplement str3(std::move(str));
	StdTStringImplement str4 = std::move(str2);
	ASSERT_EQ(str3, str4);
}
TEST(StdTStringImplement, ToCStringTest)
{
	StdTStringImplement str = SGE_TSTR("TestString");
	ASSERT_EQ(str.ToCString()[str.GetSize()], '\0');
}