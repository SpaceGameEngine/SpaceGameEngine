﻿/*
Copyright 2023 creatorlxd

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
#include "Error.h"

using namespace SpaceGameEngine;

TEST(IsError, CorrectnessTest)
{
	ASSERT_TRUE((IsError<NullPointerError, const void*> == true));
	ASSERT_TRUE((IsError<InvalidValueError, SizeType, SizeType, SizeType> == true));
	ASSERT_TRUE((IsError<int> == false));
}

#ifdef SGE_DEBUG
TEST(Assert, HandleTest)
{
	int test = 0;
	SGE_ASSERT(NullPointerError, nullptr).Handle([&]() { test = 1; });
	ASSERT_EQ(test, 1);
}

TEST(Assert, RunTest)
{
	int test = 0;
	SGE_ASSERT(NullPointerError, nullptr).Run([&]() { test = 1; }).Handle([]() {});
	ASSERT_EQ(test, 1);
}
#endif

TEST(Check, HandleTest)
{
	int test = 0;
	SGE_CHECK(NullPointerError, nullptr).Handle([&]() { test = 1; });
	ASSERT_EQ(test, 1);
}

TEST(Check, RunTest)
{
	int test = 0;
	SGE_CHECK(NullPointerError, nullptr).Run([&]() { test = 1; }).Handle([]() {});
	ASSERT_EQ(test, 1);
}