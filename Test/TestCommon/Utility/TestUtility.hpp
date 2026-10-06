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

TEST(ForwardLike, Test)
{
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<int&>(std::declval<char>())), char&>));
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<const int&>(std::declval<char>())), const char&>));
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<int&&>(std::declval<char>())), char&&>));
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<const int&&>(std::declval<char>())), const char&&>));
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<int>(std::declval<char>())), char&&>));
	ASSERT_TRUE((std::is_same_v<decltype(ForwardLike<const int>(std::declval<char>())), const char&&>));

	ASSERT_FALSE((std::is_same_v<decltype(ForwardLike<int&>(std::declval<char>())), const char&>));
	ASSERT_FALSE((std::is_same_v<decltype(ForwardLike<int>(std::declval<char>())), char>));
	ASSERT_FALSE((std::is_same_v<decltype(ForwardLike<const int>(std::declval<char>())), const char>));
}

struct TestGetOffsetOfBaseBase1
{
	virtual ~TestGetOffsetOfBaseBase1() = default;

	int m_Content1 = 1;
};

struct TestGetOffsetOfBaseBase2
{
	virtual ~TestGetOffsetOfBaseBase2() = default;

	int m_Content2 = 2;
};

struct TestGetOffsetOfBaseDerived : public TestGetOffsetOfBaseBase1, public TestGetOffsetOfBaseBase2
{
	int m_Content3 = 3;
};

TEST(GetOffsetOfBase, Test)
{
	ASSERT_EQ((GetOffsetOfBase<TestGetOffsetOfBaseBase1, TestGetOffsetOfBaseBase1>()), 0);

	TestGetOffsetOfBaseDerived obj;
	TestGetOffsetOfBaseDerived* pderived = &obj;

	ASSERT_EQ((GetOffsetOfBase<TestGetOffsetOfBaseDerived, TestGetOffsetOfBaseBase1>()),
			  reinterpret_cast<UInt64>(static_cast<TestGetOffsetOfBaseBase1*>(pderived)) - reinterpret_cast<UInt64>(pderived));
	ASSERT_EQ((GetOffsetOfBase<TestGetOffsetOfBaseDerived, TestGetOffsetOfBaseBase2>()),
			  reinterpret_cast<UInt64>(static_cast<TestGetOffsetOfBaseBase2*>(pderived)) - reinterpret_cast<UInt64>(pderived));

	ASSERT_NE((GetOffsetOfBase<TestGetOffsetOfBaseDerived, TestGetOffsetOfBaseBase2>()), 0);

	UInt64 offset = GetOffsetOfBase<TestGetOffsetOfBaseDerived, TestGetOffsetOfBaseBase2>();
	TestGetOffsetOfBaseBase2* pbase2 = reinterpret_cast<TestGetOffsetOfBaseBase2*>(reinterpret_cast<UInt64>(pderived) + offset);
	ASSERT_EQ(pbase2->m_Content2, 2);
}