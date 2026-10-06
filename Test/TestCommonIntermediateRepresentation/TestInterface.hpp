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
#include "Interface.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

struct TestInterface1
{
	virtual ~TestInterface1() = default;
	int m_Value1 = 1;
};

struct TestInterface2
{
	virtual ~TestInterface2() = default;
	int m_Value2 = 2;
};

struct TestInterfaceContainerBase : public InterfaceContainer<TestInterfaceContainerBase>
{
	virtual ~TestInterfaceContainerBase() = default;
	int m_Value0 = 0;
};

struct TestEmptyInterfacesObject : public TestInterfaceContainerBase
{
	virtual ~TestEmptyInterfacesObject() = default;
};

struct TestInterfacesObject : public TestInterfaceContainerBase, Interfaces<TestInterfacesObject, TestInterface1, TestInterface2>
{
	int m_Value3 = 3;
};

SGE_DECLARE_TYPE_ID(, TestInterface1);
SGE_DEFINE_TYPE_ID(, TestInterface1);
SGE_DECLARE_TYPE_ID(, TestInterface2);
SGE_DEFINE_TYPE_ID(, TestInterface2);

struct TestComplexInterfaceBase
{
	virtual ~TestComplexInterfaceBase() = default;
	virtual int GetBaseValue() const
	{
		return 100;
	}

	double m_BaseValue = 1.5;
};

struct TestComplexInterface1 : public TestComplexInterfaceBase
{
	virtual int GetBaseValue() const override
	{
		return 101;
	}

	char m_Padding[13] = {};
	int m_Value1 = 11;
};

struct TestComplexInterface2 : public TestComplexInterfaceBase
{
	virtual int GetBaseValue() const override
	{
		return 102;
	}

	long long m_Value2 = 22;
};

struct TestComplexHeadPadding
{
	virtual ~TestComplexHeadPadding() = default;
	char m_Padding[7] = {};
	double m_Value = 7.5;
};

struct TestComplexContainerBase : public TestComplexHeadPadding, public InterfaceContainer<TestComplexContainerBase>
{
	virtual ~TestComplexContainerBase() = default;
	int m_Value0 = 0;
};

struct TestComplexObject : public TestComplexHeadPadding, public TestComplexContainerBase, public Interfaces<TestComplexObject, TestComplexInterface1>, public Interfaces<TestComplexObject, TestComplexInterface2>
{
	int m_Value3 = 33;
};

SGE_DECLARE_TYPE_ID(, TestComplexInterface1);
SGE_DEFINE_TYPE_ID(, TestComplexInterface1);
SGE_DECLARE_TYPE_ID(, TestComplexInterface2);
SGE_DEFINE_TYPE_ID(, TestComplexInterface2);

TEST(InterfaceContainer, HasInterfaceTest)
{
	TestEmptyInterfacesObject empty_obj;
	const TestEmptyInterfacesObject& cempty_obj = empty_obj;
	ASSERT_FALSE((empty_obj.HasInterface<TestInterface1>()));
	ASSERT_FALSE((empty_obj.HasInterface<TestInterface2>()));
	ASSERT_FALSE((cempty_obj.HasInterface<TestInterface1>()));
	ASSERT_FALSE((cempty_obj.HasInterface<TestInterface2>()));

	TestInterfacesObject obj;
	const TestInterfacesObject& cobj = obj;
	ASSERT_TRUE((obj.HasInterface<TestInterface1>()));
	ASSERT_TRUE((obj.HasInterface<TestInterface2>()));
	ASSERT_TRUE((cobj.HasInterface<TestInterface1>()));
	ASSERT_TRUE((cobj.HasInterface<TestInterface2>()));
}

TEST(InterfaceContainer, GetInterfaceTest)
{
	TestInterfacesObject obj;
	TestInterface1& itf1 = obj.GetInterface<TestInterface1>();
	TestInterface2& itf2 = obj.GetInterface<TestInterface2>();
	ASSERT_EQ(&itf1, static_cast<TestInterface1*>(&obj));
	ASSERT_EQ(&itf2, static_cast<TestInterface2*>(&obj));
	ASSERT_EQ(itf1.m_Value1, 1);
	ASSERT_EQ(itf2.m_Value2, 2);

	const TestInterfacesObject& cobj = obj;
	const TestInterface1& citf1 = cobj.GetInterface<TestInterface1>();
	const TestInterface2& citf2 = cobj.GetInterface<TestInterface2>();
	ASSERT_EQ(&citf1, static_cast<const TestInterface1*>(&obj));
	ASSERT_EQ(&citf2, static_cast<const TestInterface2*>(&obj));
	ASSERT_EQ(citf1.m_Value1, 1);
	ASSERT_EQ(citf2.m_Value2, 2);
}

TEST(InterfaceContainer, QueryInterfaceTest)
{
	TestEmptyInterfacesObject empty_obj;
	const TestEmptyInterfacesObject& cempty_obj = empty_obj;
	ASSERT_EQ((empty_obj.QueryInterface<TestInterface1>()), nullptr);
	ASSERT_EQ((empty_obj.QueryInterface<TestInterface2>()), nullptr);
	ASSERT_EQ((cempty_obj.QueryInterface<TestInterface1>()), nullptr);
	ASSERT_EQ((cempty_obj.QueryInterface<TestInterface2>()), nullptr);

	TestInterfacesObject obj;
	const TestInterfacesObject& cobj = obj;
	ASSERT_EQ((obj.QueryInterface<TestInterface1>()), static_cast<TestInterface1*>(&obj));
	ASSERT_EQ((obj.QueryInterface<TestInterface2>()), static_cast<TestInterface2*>(&obj));
	ASSERT_EQ((cobj.QueryInterface<TestInterface1>()), static_cast<const TestInterface1*>(&obj));
	ASSERT_EQ((cobj.QueryInterface<TestInterface2>()), static_cast<const TestInterface2*>(&obj));
}

TEST(InterfaceContainer, ComplexLayoutTest)
{
	TestComplexObject obj;
	const TestComplexObject& cobj = obj;

	ASSERT_TRUE((obj.HasInterface<TestComplexInterface1>()));
	ASSERT_TRUE((obj.HasInterface<TestComplexInterface2>()));
	ASSERT_FALSE((obj.HasInterface<TestInterface1>()));

	ASSERT_TRUE((cobj.HasInterface<TestComplexInterface1>()));
	ASSERT_TRUE((cobj.HasInterface<TestComplexInterface2>()));
	ASSERT_FALSE((cobj.HasInterface<TestInterface1>()));

	TestComplexInterface1& itf1 = obj.GetInterface<TestComplexInterface1>();
	TestComplexInterface2& itf2 = obj.GetInterface<TestComplexInterface2>();
	ASSERT_EQ(&itf1, static_cast<TestComplexInterface1*>(&obj));
	ASSERT_EQ(&itf2, static_cast<TestComplexInterface2*>(&obj));

	// the container base is not at the beginning of the object, so the offsets are computed relatively to it
	ASSERT_NE(reinterpret_cast<UInt64>(static_cast<TestComplexContainerBase*>(&obj)), reinterpret_cast<UInt64>(&obj));
	ASSERT_GT(reinterpret_cast<UInt64>(&itf1), reinterpret_cast<UInt64>(static_cast<TestComplexContainerBase*>(&obj)));
	ASSERT_GT(reinterpret_cast<UInt64>(&itf2), reinterpret_cast<UInt64>(&itf1));

	ASSERT_EQ(itf1.m_Value1, 11);
	ASSERT_EQ(itf2.m_Value2, 22);
	ASSERT_EQ(itf1.GetBaseValue(), 101);
	ASSERT_EQ(itf2.GetBaseValue(), 102);

	itf1.m_Value1 = 110;
	itf2.m_Value2 = 220;
	ASSERT_EQ(obj.TestComplexInterface1::m_Value1, 110);
	ASSERT_EQ(obj.TestComplexInterface2::m_Value2, 220);

	const TestComplexInterface1& citf1 = cobj.GetInterface<TestComplexInterface1>();
	const TestComplexInterface2& citf2 = cobj.GetInterface<TestComplexInterface2>();
	ASSERT_EQ(&citf1, static_cast<const TestComplexInterface1*>(&obj));
	ASSERT_EQ(&citf2, static_cast<const TestComplexInterface2*>(&obj));

	ASSERT_EQ((obj.QueryInterface<TestComplexInterface1>()), static_cast<TestComplexInterface1*>(&obj));
	ASSERT_EQ((obj.QueryInterface<TestComplexInterface2>()), static_cast<TestComplexInterface2*>(&obj));
	ASSERT_EQ((obj.QueryInterface<TestInterface1>()), nullptr);

	ASSERT_EQ((cobj.QueryInterface<TestComplexInterface1>()), static_cast<const TestComplexInterface1*>(&obj));
	ASSERT_EQ((cobj.QueryInterface<TestComplexInterface2>()), static_cast<const TestComplexInterface2*>(&obj));
	ASSERT_EQ((cobj.QueryInterface<TestInterface1>()), nullptr);
}
