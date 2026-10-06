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

struct TestInterfaceContainerBase : public InterfaceContainer
{
	virtual ~TestInterfaceContainerBase() = default;
	int m_Value0 = 0;
};

struct TestInterfacesObject : public TestInterfaceContainerBase, Interfaces<TestInterfacesObject, TestInterface1, TestInterface2>
{
	int m_Value3 = 3;
};

SGE_DECLARE_TYPE_ID(, TestInterface1);
SGE_DEFINE_TYPE_ID(, TestInterface1);
SGE_DECLARE_TYPE_ID(, TestInterface2);
SGE_DEFINE_TYPE_ID(, TestInterface2);

TEST(InterfaceContainer, HasInterfaceTest)
{
	InterfaceContainer ic;
	ASSERT_FALSE((ic.HasInterface<TestInterface1, TestInterface1>()));
	ASSERT_FALSE((ic.HasInterface<TestInterface2, TestInterface2>()));
	ASSERT_FALSE((static_cast<const InterfaceContainer&>(ic).HasInterface<TestInterface1, TestInterface1>()));

	TestInterfacesObject obj;
	const TestInterfacesObject& cobj = obj;
	ASSERT_TRUE((obj.HasInterface<TestInterfacesObject, TestInterface1>()));
	ASSERT_TRUE((obj.HasInterface<TestInterfacesObject, TestInterface2>()));
	ASSERT_TRUE((cobj.HasInterface<TestInterfacesObject, TestInterface1>()));
	ASSERT_TRUE((cobj.HasInterface<TestInterfacesObject, TestInterface2>()));
}

TEST(InterfaceContainer, GetInterfaceTest)
{
	TestInterfacesObject obj;
	TestInterface1& itf1 = obj.GetInterface<TestInterfacesObject, TestInterface1>(obj);
	TestInterface2& itf2 = obj.GetInterface<TestInterfacesObject, TestInterface2>(obj);
	ASSERT_EQ(&itf1, static_cast<TestInterface1*>(&obj));
	ASSERT_EQ(&itf2, static_cast<TestInterface2*>(&obj));
	ASSERT_EQ(itf1.m_Value1, 1);
	ASSERT_EQ(itf2.m_Value2, 2);

	const TestInterfacesObject& cobj = obj;
	const TestInterface1& citf1 = cobj.GetInterface<TestInterfacesObject, TestInterface1>(cobj);
	const TestInterface2& citf2 = cobj.GetInterface<TestInterfacesObject, TestInterface2>(cobj);
	ASSERT_EQ(&citf1, static_cast<const TestInterface1*>(&obj));
	ASSERT_EQ(&citf2, static_cast<const TestInterface2*>(&obj));
	ASSERT_EQ(citf1.m_Value1, 1);
	ASSERT_EQ(citf2.m_Value2, 2);
}

TEST(InterfaceContainer, QueryInterfaceTest)
{
	InterfaceContainer ic;
	TestInterface1 itf1;
	TestInterface2 itf2;
	ASSERT_EQ((ic.QueryInterface<TestInterface1, TestInterface1>(itf1)), nullptr);
	ASSERT_EQ((ic.QueryInterface<TestInterface2, TestInterface2>(itf2)), nullptr);

	const InterfaceContainer& cic = ic;
	ASSERT_EQ((cic.QueryInterface<TestInterface1, TestInterface1>(itf1)), nullptr);
	ASSERT_EQ((cic.QueryInterface<TestInterface2, TestInterface2>(itf2)), nullptr);

	TestInterfacesObject obj;
	ASSERT_EQ((obj.QueryInterface<TestInterfacesObject, TestInterface1>(obj)), static_cast<TestInterface1*>(&obj));
	ASSERT_EQ((obj.QueryInterface<TestInterfacesObject, TestInterface2>(obj)), static_cast<TestInterface2*>(&obj));

	const TestInterfacesObject& cobj = obj;
	ASSERT_EQ((cobj.QueryInterface<TestInterfacesObject, TestInterface1>(cobj)), static_cast<const TestInterface1*>(&obj));
	ASSERT_EQ((cobj.QueryInterface<TestInterfacesObject, TestInterface2>(cobj)), static_cast<const TestInterface2*>(&obj));
}
