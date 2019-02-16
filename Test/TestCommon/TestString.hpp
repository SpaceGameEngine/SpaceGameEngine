﻿/*
Copyright 2019 creatorlxd

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
#include "Catch2/catch.hpp"
#include "SGEString.h"
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
		REQUIRE(memcmp(str.GetData(), SGE_TSTR("TestString"), str.GetSize()) == 0);
		//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
		REQUIRE(str.GetSize() == (sizeof(SGE_TSTR("TestString")) / sizeof(StdTChar)) - 1);
	}
	SECTION("test copy")
	{
		String str = SGE_TSTR("TestString");
		String str2 = str;
		REQUIRE(str == str2);
		//if str's implement is equal with str2's then their raw data address must be same
		REQUIRE(str.GetData() != str2.GetData());
	}
	SECTION("test move")
	{
		String str = SGE_TSTR("TestString");
		String str2 = SGE_TSTR("TestString");
		String str3(std::move(str));
		String str4 = std::move(str2);
		REQUIRE(str3 == str4);
	}
	SECTION("test cstring")
	{
		String str = SGE_TSTR("TestString");
		REQUIRE(str.GetCString()[str.GetSize()] == '\0');
	}
}

TEST_CASE("Test StdTStringImplement", "[Common][String]")
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
		REQUIRE(memcmp(str.GetData(), SGE_TSTR("TestString"), str.GetSize()) == 0);
		//-1 because SGE_TSTR("TestString") has a '\0' char as the end of the str
		REQUIRE(str.GetSize() == (sizeof(SGE_TSTR("TestString")) / sizeof(StdTChar)) - 1);
	}
	SECTION("test copy")
	{
		StdTStringImplement str = SGE_TSTR("TestString");
		StdTStringImplement str2 = str;
		REQUIRE(str == str2);
		REQUIRE(str.GetData() != str2.GetData());
	}
	SECTION("test move")
	{
		StdTStringImplement str = SGE_TSTR("TestString");
		StdTStringImplement str2 = SGE_TSTR("TestString");
		StdTStringImplement str3(std::move(str));
		StdTStringImplement str4 = std::move(str2);
		REQUIRE(str3 == str4);
	}
	SECTION("test cstring")
	{
		StdTStringImplement str = SGE_TSTR("TestString");
		REQUIRE(str.GetCString()[str.GetSize()] == '\0');
	}
}