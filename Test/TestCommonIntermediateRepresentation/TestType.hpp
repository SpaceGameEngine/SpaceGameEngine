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
#include "Type.h"
#include "Context.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

class TestTypeA : public Type, public DynamicCastHelperForDerived<TestTypeA, Type>
{
public:
	inline TestTypeA(Dialect& dialect)
		: Type(SGE_STR("test_type_a"), dialect)
	{
	}

	using DynamicCastHelperForDerived<TestTypeA, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TestTypeA);
SGE_DEFINE_TYPE_ID(, TestTypeA);

class TestTypeB : public Type, public DynamicCastHelperForDerived<TestTypeB, Type>
{
public:
	inline TestTypeB(Dialect& dialect)
		: Type(SGE_STR("test_type_b"), dialect)
	{
	}

	using DynamicCastHelperForDerived<TestTypeB, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TestTypeB);
SGE_DEFINE_TYPE_ID(, TestTypeB);

class TestTypeDialect : public Dialect, public DynamicCastHelperForDerived<TestTypeDialect, Dialect>
{
public:
	inline TestTypeDialect(Context& context)
		: Dialect(SGE_STR("test_type_dialect"), context)
	{
	}

	using DynamicCastHelperForDerived<TestTypeDialect, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TestTypeDialect);
SGE_DEFINE_TYPE_ID(, TestTypeDialect);

TEST(Type, DialectRegisterTest)
{
	Context context;
	TestTypeDialect& dialect = context.AddDialect<TestTypeDialect>();
	TestTypeA& type_a = dialect.AddType<TestTypeA>();
	ASSERT_TRUE(dialect.HasType<TestTypeA>());
	ASSERT_FALSE(dialect.HasType<TestTypeB>());
	ASSERT_EQ(&dialect.GetType<TestTypeA>(), &type_a);
	ASSERT_EQ(dialect.QueryType<TestTypeB>(), nullptr);
	ASSERT_TRUE(dialect.HasTypeByName(SGE_STR("test_type_a")));
	ASSERT_EQ(&dialect.GetTypeByName(SGE_STR("test_type_a")), static_cast<Type*>(&type_a));
	ASSERT_EQ(dialect.QueryTypeByName(SGE_STR("test_type_b")), nullptr);
}

TEST(Type, GetNameTest)
{
	Context context;
	TestTypeDialect& dialect = context.AddDialect<TestTypeDialect>();
	TestTypeA& type_a = dialect.AddType<TestTypeA>();
	TestTypeB& type_b = dialect.AddType<TestTypeB>();
	ASSERT_EQ(type_a.GetName(), SGE_STR("test_type_a"));
	ASSERT_EQ(type_b.GetName(), SGE_STR("test_type_b"));

	const Type& ctype_a = type_a;
	ASSERT_EQ(ctype_a.GetName(), SGE_STR("test_type_a"));
}

TEST(Type, GetBelongedDialectTest)
{
	Context context;
	TestTypeDialect& dialect = context.AddDialect<TestTypeDialect>();
	TestTypeA& type_a = dialect.AddType<TestTypeA>();
	ASSERT_EQ(&type_a.GetBelongedDialect(), static_cast<Dialect*>(&dialect));

	const TestTypeA& ctype_a = type_a;
	ASSERT_EQ(&ctype_a.GetBelongedDialect(), static_cast<const Dialect*>(&dialect));
}

TEST(Type, GetTypeIdTest)
{
	Context context;
	TestTypeDialect& dialect = context.AddDialect<TestTypeDialect>();
	TestTypeA& type_a = dialect.AddType<TestTypeA>();
	TestTypeB& type_b = dialect.AddType<TestTypeB>();
	Type& base_a = type_a;
	Type& base_b = type_b;

	ASSERT_EQ(base_a.GetTypeId(), GetTypeId<TestTypeA>());
	ASSERT_EQ(base_b.GetTypeId(), GetTypeId<TestTypeB>());
	ASSERT_NE(base_a.GetTypeId(), base_b.GetTypeId());
	ASSERT_NE(base_a.GetTypeId(), GetTypeId<Type>());
}

TEST(Type, DynamicCastTest)
{
	Context context;
	TestTypeDialect& dialect = context.AddDialect<TestTypeDialect>();
	TestTypeA& type_a = dialect.AddType<TestTypeA>();
	TestTypeB& type_b = dialect.AddType<TestTypeB>();
	Type& base_a = type_a;
	Type& base_b = type_b;

	ASSERT_TRUE(Type::IsInstance(base_a));
	ASSERT_TRUE(TestTypeA::IsInstance(base_a));
	ASSERT_FALSE(TestTypeB::IsInstance(base_a));
	ASSERT_TRUE(TestTypeB::IsInstance(base_b));
	ASSERT_FALSE(TestTypeA::IsInstance(base_b));

	ASSERT_EQ(DynamicCast<TestTypeA>(base_a), &type_a);
	ASSERT_EQ(DynamicCast<TestTypeB>(base_a), nullptr);
	ASSERT_EQ(DynamicCast<TestTypeB>(base_b), &type_b);
	ASSERT_EQ(DynamicCast<TestTypeA>(base_b), nullptr);

	const Type& cbase_a = type_a;
	ASSERT_EQ(DynamicCast<const TestTypeA>(cbase_a), &type_a);
	ASSERT_EQ(DynamicCast<const TestTypeB>(cbase_a), nullptr);
}
