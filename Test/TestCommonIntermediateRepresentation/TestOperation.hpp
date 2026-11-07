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
#include "Operation.h"
#include "Value.h"
#include "Dialect.h"
#include "Context.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

class OperationType1ForTestOperationType : public OperationType, public DynamicCastHelperForDerived<OperationType1ForTestOperationType, OperationType>
{
public:
	friend class Dialect;

	inline OperationType1ForTestOperationType(Dialect& dialect)
		: OperationType(SGE_STR("OperationType1ForTestOperationType"), dialect)
	{
	}

	using DynamicCastHelperForDerived<OperationType1ForTestOperationType, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, OperationType1ForTestOperationType);
SGE_DEFINE_TYPE_ID(, OperationType1ForTestOperationType);

class OperationType2ForTestOperationType : public OperationType, public DynamicCastHelperForDerived<OperationType2ForTestOperationType, OperationType>
{
public:
	friend class Dialect;

	inline OperationType2ForTestOperationType(Dialect& dialect)
		: OperationType(SGE_STR("OperationType2ForTestOperationType"), dialect)
	{
	}

	using DynamicCastHelperForDerived<OperationType2ForTestOperationType, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, OperationType2ForTestOperationType);
SGE_DEFINE_TYPE_ID(, OperationType2ForTestOperationType);

class DialectForTestOperationType : public Dialect, public DynamicCastHelperForDerived<DialectForTestOperationType, Dialect>
{
public:
	friend class Context;

	inline DialectForTestOperationType(Context& context)
		: Dialect(SGE_STR("DialectForTestOperationType"), context)
	{
		AddOperationType<OperationType1ForTestOperationType>();
		AddOperationType<OperationType2ForTestOperationType>();
	}

	using DynamicCastHelperForDerived<DialectForTestOperationType, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestOperationType);
SGE_DEFINE_TYPE_ID(, DialectForTestOperationType);

TEST(OperationType, GetNameTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperationType>();
	OperationType& operation_type1 = dialect.GetOperationType<OperationType1ForTestOperationType>();
	const OperationType& coperation_type2 = dialect.GetOperationType<OperationType2ForTestOperationType>();

	ASSERT_EQ(operation_type1.GetName(), SGE_STR("OperationType1ForTestOperationType"));
	ASSERT_EQ(coperation_type2.GetName(), SGE_STR("OperationType2ForTestOperationType"));
}

TEST(OperationType, GetBelongedDialectTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperationType>();
	const Dialect& cdialect = dialect;
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	const OperationType& coperation_type = operation_type;

	ASSERT_EQ(&operation_type.GetBelongedDialect(), &dialect);
	ASSERT_EQ(&coperation_type.GetBelongedDialect(), &cdialect);
}

TEST(OperationType, DynamicCastTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperationType>();
	OperationType& operation_type1 = dialect.GetOperationType<OperationType1ForTestOperationType>();
	OperationType& operation_type2 = dialect.GetOperationType<OperationType2ForTestOperationType>();

	ASSERT_TRUE(OperationType::IsInstance(operation_type1));
	ASSERT_TRUE(OperationType::IsInstance(operation_type2));

	ASSERT_TRUE(OperationType1ForTestOperationType::IsInstance(operation_type1));
	ASSERT_FALSE(OperationType1ForTestOperationType::IsInstance(operation_type2));
	ASSERT_TRUE(OperationType2ForTestOperationType::IsInstance(operation_type2));
	ASSERT_FALSE(OperationType2ForTestOperationType::IsInstance(operation_type1));

	ASSERT_EQ(DynamicCast<OperationType1ForTestOperationType>(operation_type1), &operation_type1);
	ASSERT_EQ(DynamicCast<OperationType2ForTestOperationType>(operation_type1), nullptr);

	const OperationType& coperation_type2 = operation_type2;
	ASSERT_EQ(DynamicCast<const OperationType2ForTestOperationType>(coperation_type2), &operation_type2);
	ASSERT_EQ(DynamicCast<const OperationType1ForTestOperationType>(coperation_type2), nullptr);
}

TEST(OperationType, GetTypeIdTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperationType>();
	OperationType& operation_type1 = dialect.GetOperationType<OperationType1ForTestOperationType>();
	OperationType& operation_type2 = dialect.GetOperationType<OperationType2ForTestOperationType>();

	ASSERT_EQ(operation_type1.GetTypeId(), GetTypeId<OperationType1ForTestOperationType>());
	ASSERT_EQ(operation_type2.GetTypeId(), GetTypeId<OperationType2ForTestOperationType>());
	ASSERT_NE(operation_type1.GetTypeId(), operation_type2.GetTypeId());
}

class TypeForTestOperation : public Type, public DynamicCastHelperForDerived<TypeForTestOperation, Type>
{
public:
	friend class Dialect;

	inline TypeForTestOperation(Dialect& dialect)
		: Type(SGE_STR("TypeForTestOperation"), dialect)
	{
	}

	using DynamicCastHelperForDerived<TypeForTestOperation, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TypeForTestOperation);
SGE_DEFINE_TYPE_ID(, TypeForTestOperation);

class VerifyFailedOperationTypeForTestOperation : public OperationType, public DynamicCastHelperForDerived<VerifyFailedOperationTypeForTestOperation, OperationType>
{
public:
	friend class Dialect;

	inline VerifyFailedOperationTypeForTestOperation(Dialect& dialect)
		: OperationType(SGE_STR("VerifyFailedOperationTypeForTestOperation"), dialect)
	{
	}

	inline bool VerifyArguments(const Vector<const Value*>& arguments) const override
	{
		return arguments.GetSize() == 1;
	}

	inline bool VerifyResults(const Vector<const ResultValue*>& results) const override
	{
		return results.GetSize() == 1;
	}

	using DynamicCastHelperForDerived<VerifyFailedOperationTypeForTestOperation, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, VerifyFailedOperationTypeForTestOperation);
SGE_DEFINE_TYPE_ID(, VerifyFailedOperationTypeForTestOperation);

class DialectForTestOperation : public Dialect, public DynamicCastHelperForDerived<DialectForTestOperation, Dialect>
{
public:
	friend class Context;

	inline DialectForTestOperation(Context& context)
		: Dialect(SGE_STR("DialectForTestOperation"), context)
	{
		AddOperationType<OperationType1ForTestOperationType>();
		AddOperationType<VerifyFailedOperationTypeForTestOperation>();
		AddType<TypeForTestOperation>();
	}

	using DynamicCastHelperForDerived<DialectForTestOperation, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestOperation);
SGE_DEFINE_TYPE_ID(, DialectForTestOperation);

TEST(OperationBuilder, BuildTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	OperationBuilder builder(operation_type);
	builder.AddArgument<IntegerValue>(1).AddArgument<StringValue>(SGE_STR("test")).AddResult(type);
	Operation op = builder.Build();
	ASSERT_TRUE(op.Verify());
}

TEST(Operation, VerifyTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<VerifyFailedOperationTypeForTestOperation>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	Operation op1 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).AddResult(type).Build();
	ASSERT_TRUE(op1.Verify());

	Operation op2 = OperationBuilder(operation_type).AddResult(type).Build();
	ASSERT_FALSE(op2.Verify());

	Operation op3 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).Build();
	ASSERT_FALSE(op3.Verify());

	Operation op4 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).AddArgument<BooleanValue>(true).AddResult(type).Build();
	ASSERT_FALSE(op4.Verify());
}