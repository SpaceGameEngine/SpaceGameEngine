/*
Copyright 2025 creatorlxd

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
#include <random>
#include <cmath>
#include "Utility/Utility.hpp"
#include "Utility/Pair.hpp"
#include "SGEString.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(Less, LessTest)
{
	ASSERT_TRUE(Less<int>::Compare(1, 2));
	ASSERT_FALSE(Less<int>::Compare(1, 1));
	ASSERT_FALSE(Less<int>::Compare(2, 1));
}

TEST(Greater, GreaterTest)
{
	ASSERT_TRUE(Greater<int>::Compare(2, 1));
	ASSERT_FALSE(Greater<int>::Compare(1, 1));
	ASSERT_FALSE(Greater<int>::Compare(1, 2));
}

TEST(Digits10, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<10>(num), (UInt64)(std::log10(num)) + 1);
	}
}

TEST(Digits2, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<2>(num), (UInt64)(std::log2(num)) + 1);
	}
}

TEST(Digits16, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<16>(num), (UInt64)(std::log2(num) / 4.0) + 1);
	}
}
