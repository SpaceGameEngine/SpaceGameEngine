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

struct TestDynamicCastHelperBase : public DynamicCastHelperForBase<TestDynamicCastHelperBase>
{
	virtual ~TestDynamicCastHelperBase() = default;
};

struct TestDynamicCastHelperDerived1 : public TestDynamicCastHelperBase, public DynamicCastHelperForDerived<TestDynamicCastHelperDerived1, TestDynamicCastHelperBase>
{
	using DynamicCastHelperForDerived<TestDynamicCastHelperDerived1, TestDynamicCastHelperBase>::IsInstance;
};

struct TestDynamicCastHelperDerived2 : public TestDynamicCastHelperBase, public DynamicCastHelperForDerived<TestDynamicCastHelperDerived2, TestDynamicCastHelperBase>
{
	using DynamicCastHelperForDerived<TestDynamicCastHelperDerived2, TestDynamicCastHelperBase>::IsInstance;
};

struct TestDynamicCastHelperDerived3 : public TestDynamicCastHelperDerived1, public DynamicCastHelperForDerived<TestDynamicCastHelperDerived3, TestDynamicCastHelperBase>
{
	using DynamicCastHelperForDerived<TestDynamicCastHelperDerived3, TestDynamicCastHelperBase>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TestDynamicCastHelperBase);
SGE_DEFINE_TYPE_ID(, TestDynamicCastHelperBase);
SGE_DECLARE_TYPE_ID(, TestDynamicCastHelperDerived1);
SGE_DEFINE_TYPE_ID(, TestDynamicCastHelperDerived1);
SGE_DECLARE_TYPE_ID(, TestDynamicCastHelperDerived2);
SGE_DEFINE_TYPE_ID(, TestDynamicCastHelperDerived2);
SGE_DECLARE_TYPE_ID(, TestDynamicCastHelperDerived3);
SGE_DEFINE_TYPE_ID(, TestDynamicCastHelperDerived3);

TEST(DynamicCastHelper, GetTypeIdTest)
{
	TestDynamicCastHelperBase base;
	TestDynamicCastHelperDerived1 derived1;
	TestDynamicCastHelperDerived2 derived2;
	TestDynamicCastHelperDerived3 derived3;

	ASSERT_EQ(base.GetTypeId(), GetTypeId<TestDynamicCastHelperBase>());
	ASSERT_EQ(derived1.GetTypeId(), GetTypeId<TestDynamicCastHelperDerived1>());
	ASSERT_EQ(derived2.GetTypeId(), GetTypeId<TestDynamicCastHelperDerived2>());
	ASSERT_EQ(derived3.GetTypeId(), GetTypeId<TestDynamicCastHelperDerived3>());

	ASSERT_NE(derived1.GetTypeId(), derived2.GetTypeId());
	ASSERT_NE(derived1.GetTypeId(), derived3.GetTypeId());
	ASSERT_NE(base.GetTypeId(), derived1.GetTypeId());
}

TEST(DynamicCastHelper, IsInstanceTest)
{
	TestDynamicCastHelperBase base;
	TestDynamicCastHelperDerived1 derived1;
	TestDynamicCastHelperDerived2 derived2;
	TestDynamicCastHelperDerived3 derived3;

	ASSERT_TRUE(TestDynamicCastHelperBase::IsInstance(base));
	ASSERT_TRUE(TestDynamicCastHelperBase::IsInstance(derived1));
	ASSERT_TRUE(TestDynamicCastHelperBase::IsInstance(derived2));
	ASSERT_TRUE(TestDynamicCastHelperBase::IsInstance(derived3));

	ASSERT_TRUE(TestDynamicCastHelperDerived1::IsInstance(derived1));
	ASSERT_FALSE(TestDynamicCastHelperDerived1::IsInstance(base));
	ASSERT_FALSE(TestDynamicCastHelperDerived1::IsInstance(derived2));
	ASSERT_FALSE(TestDynamicCastHelperDerived1::IsInstance(derived3));

	ASSERT_TRUE(TestDynamicCastHelperDerived2::IsInstance(derived2));
	ASSERT_FALSE(TestDynamicCastHelperDerived2::IsInstance(base));
	ASSERT_FALSE(TestDynamicCastHelperDerived2::IsInstance(derived1));
	ASSERT_FALSE(TestDynamicCastHelperDerived2::IsInstance(derived3));

	ASSERT_TRUE(TestDynamicCastHelperDerived3::IsInstance(derived3));
	ASSERT_FALSE(TestDynamicCastHelperDerived3::IsInstance(base));
	ASSERT_FALSE(TestDynamicCastHelperDerived3::IsInstance(derived1));
	ASSERT_FALSE(TestDynamicCastHelperDerived3::IsInstance(derived2));
}

TEST(DynamicCastHelper, DynamicCastTest)
{
	TestDynamicCastHelperBase base;
	TestDynamicCastHelperDerived1 derived1;
	TestDynamicCastHelperDerived2 derived2;
	TestDynamicCastHelperDerived3 derived3;

	TestDynamicCastHelperBase& base_ref = base;
	TestDynamicCastHelperBase& derived1_ref = derived1;
	TestDynamicCastHelperBase& derived2_ref = derived2;
	TestDynamicCastHelperBase& derived3_ref = derived3;

	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(base_ref), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(derived1_ref), &derived1);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(derived2_ref), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(derived3_ref), nullptr);

	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived2>(derived2_ref), &derived2);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived3>(derived3_ref), &derived3);

	const TestDynamicCastHelperBase& cbase_ref = base;
	const TestDynamicCastHelperBase& cderived1_ref = derived1;
	const TestDynamicCastHelperBase& cderived2_ref = derived2;

	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(cbase_ref), nullptr);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(cderived1_ref), &derived1);
	ASSERT_EQ(DynamicCast<TestDynamicCastHelperDerived1>(cderived2_ref), nullptr);
}