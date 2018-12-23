﻿/*
Copyright 2018 creatorlxd

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
#include "../../ThirdParty/Catch2/catch.hpp"
#include "../../Source/Common/Public/String.h"
#include "../../Source/Common/Private/StringImplement/StdTStringImplement.h"

using namespace SpaceGameEngine;

TEST_CASE("Test String", "[Common][String]")
{
	SECTION("test use std t string to operate")
	{
		String str = StdTString(SGE_TSTR("TestString"));
		REQUIRE(str == StdTString(SGE_TSTR("TestString")));
		REQUIRE(str.GetSize() == StdTString(SGE_TSTR("TestString")).size());
	}
	SECTION("test use std t char ptr to operate")
	{
		String str = SGE_TSTR("TestString");
		REQUIRE(memcmp(str.GetConstData(), SGE_TSTR("TestString"), str.GetSize()) == 0);
		//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
		REQUIRE(str.GetSize() == (sizeof(SGE_TSTR("TestString")) / sizeof(StdTChar)) - 1);
	}
}

TEST_CASE("Test StdTStringImplement")
{
	SECTION("test use std t string to operate")
	{
		StdTStringImplement str = StdTString(SGE_TSTR("TestString"));
		REQUIRE(str == StdTString(SGE_TSTR("TestString")));
		REQUIRE(str.GetSize() == StdTString(SGE_TSTR("TestString")).size());
	}
	SECTION("test use std t char ptr to operate")
	{
		StdTStringImplement str = SGE_TSTR("TestString");
		REQUIRE(memcmp(str.GetConstData(), SGE_TSTR("TestString"), str.GetSize()) == 0);
		//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
		REQUIRE(str.GetSize() == (sizeof(SGE_TSTR("TestString")) / sizeof(StdTChar)) - 1);
	}
}