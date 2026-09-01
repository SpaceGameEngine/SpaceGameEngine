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
#include "Utility/DynamicCast.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct TestDynamicCastBase
{
	int type_id = 0;
};

struct TestDynamicCastDerived1 : TestDynamicCastBase
{
	TestDynamicCastDerived1()
	{
		type_id = 1;
	}

	static bool IsInstance(const TestDynamicCastBase& base)
	{
		return base.type_id == 1;
	}
};

struct TestDynamicCastDerived2 : TestDynamicCastBase
{
	TestDynamicCastDerived2()
	{
		type_id = 2;
	}

	static bool IsInstance(const TestDynamicCastBase& base)
	{
		return base.type_id == 2;
	}
};

TEST(DynamicCast, Test)
{
	TestDynamicCastBase base;
	TestDynamicCastDerived1 derived1;
	TestDynamicCastDerived2 derived2;

	ASSERT_TRUE(TestDynamicCastDerived1::IsInstance(derived1));
	ASSERT_FALSE(TestDynamicCastDerived1::IsInstance(derived2));
	ASSERT_FALSE(TestDynamicCastDerived1::IsInstance(base));

	ASSERT_EQ(DynamicCast<TestDynamicCastDerived1>(base), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastDerived1>((TestDynamicCastBase&)derived2), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastDerived1>((TestDynamicCastBase&)derived1), &derived1);

	ASSERT_TRUE(TestDynamicCastDerived2::IsInstance(derived2));
	ASSERT_FALSE(TestDynamicCastDerived2::IsInstance(derived1));
	ASSERT_FALSE(TestDynamicCastDerived2::IsInstance(base));

	ASSERT_EQ(DynamicCast<TestDynamicCastDerived2>(base), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastDerived2>((TestDynamicCastBase&)derived1), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastDerived2>((TestDynamicCastBase&)derived2), &derived2);
}