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

class InferReturnTypeOperationTypeForTestOperation : public OperationType, public Interfaces<InferReturnTypeOperationTypeForTestOperation, InferReturnTypeOperationTypeInterface>, public DynamicCastHelperForDerived<InferReturnTypeOperationTypeForTestOperation, OperationType>
{
public:
	friend class Dialect;

	inline InferReturnTypeOperationTypeForTestOperation(Dialect& dialect)
		: OperationType(SGE_STR("InferReturnTypeOperationTypeForTestOperation"), dialect)
	{
	}

	// returns one result per argument, fails when there is no argument
	inline Optional<Vector<const Type*>> InferReturnTypes(const Vector<const Value*>& arguments) const override
	{
		if (arguments.GetSize() == 0)
			return OptionalTag::EmptyOptional;
		Vector<const Type*> types;
		const Type& type = GetBelongedDialect().GetType<TypeForTestOperation>();
		for (SizeType i = 0; i < arguments.GetSize(); ++i)
			types.PushBack(&type);
		return Optional<Vector<const Type*>>(std::move(types));
	}

	using DynamicCastHelperForDerived<InferReturnTypeOperationTypeForTestOperation, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, InferReturnTypeOperationTypeForTestOperation);
SGE_DEFINE_TYPE_ID(, InferReturnTypeOperationTypeForTestOperation);

class DialectForTestOperation : public Dialect, public DynamicCastHelperForDerived<DialectForTestOperation, Dialect>
{
public:
	friend class Context;

	inline DialectForTestOperation(Context& context)
		: Dialect(SGE_STR("DialectForTestOperation"), context)
	{
		AddOperationType<OperationType1ForTestOperationType>();
		AddOperationType<VerifyFailedOperationTypeForTestOperation>();
		AddOperationType<InferReturnTypeOperationTypeForTestOperation>();
		AddType<TypeForTestOperation>();
	}

	using DynamicCastHelperForDerived<DialectForTestOperation, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestOperation);
SGE_DEFINE_TYPE_ID(, DialectForTestOperation);

TEST(OperationBuilder, AddArgumentTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();

	Operation op = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).AddArgument<StringValue>(SGE_STR("test")).AddArgument<BooleanValue>(true).Build();
	ASSERT_EQ(op.GetArgumentSize(), 3);
	ASSERT_EQ(op.GetResultSize(), 0);
	ASSERT_EQ(DynamicCast<const IntegerValue>(op.GetArgument(0))->GetValue(), 1);
	ASSERT_EQ(DynamicCast<const StringValue>(op.GetArgument(1))->GetValue(), SGE_STR("test"));
	ASSERT_EQ(DynamicCast<const BooleanValue>(op.GetArgument(2))->GetValue(), true);
	for (SizeType i = 0; i < op.GetArgumentSize(); ++i)
		ASSERT_EQ(op.GetArgument(i).GetBelongedOperation(), &op);
}

TEST(OperationBuilder, AddResultTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	Operation op = OperationBuilder(operation_type).AddResult(type).AddResult(type).Build();
	ASSERT_EQ(op.GetArgumentSize(), 0);
	ASSERT_EQ(op.GetResultSize(), 2);
	ASSERT_NE(&op.GetResult(0), &op.GetResult(1));
	for (SizeType i = 0; i < op.GetResultSize(); ++i)
	{
		ASSERT_EQ(&op.GetResult(i).GetType(), &type);
		ASSERT_EQ(op.GetResult(i).GetBelongedOperation(), &op);
	}
}

TEST(OperationBuilder, AddAttributeTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();

	Operation op = OperationBuilder(operation_type)
					   .AddAttribute<IntegerAttribute>(SGE_STR("int"), 42)
					   .AddAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("test"))
					   .AddAttribute<IntegerAttribute>(SGE_STR("int"), 7)
					   .Build();

	const DictionaryAttribute& attributes = op.GetAttributes();
	const Attribute* pint = attributes.QueryAttribute(SGE_STR("int"));
	ASSERT_NE(pint, nullptr);
	ASSERT_TRUE(IntegerAttribute::IsInstance(*pint));
	ASSERT_EQ(DynamicCast<const IntegerAttribute>(*pint)->GetValue(), 7);

	const Attribute* pstr = attributes.QueryAttribute(SGE_STR("str"));
	ASSERT_NE(pstr, nullptr);
	ASSERT_TRUE(StringAttribute::IsInstance(*pstr));
	ASSERT_EQ(DynamicCast<const StringAttribute>(*pstr)->GetValue(), SGE_STR("test"));

	ASSERT_EQ(attributes.QueryAttribute(SGE_STR("none")), nullptr);

	SizeType cnt = 0;
	attributes.Visit([&](const String&, const Attribute*) { ++cnt; });
	ASSERT_EQ(cnt, 2);
}

TEST(OperationBuilder, InferReturnValuesTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& infer_operation_type = dialect.GetOperationType<InferReturnTypeOperationTypeForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	ASSERT_NE(infer_operation_type.QueryInterface<InferReturnTypeOperationTypeInterface>(), nullptr);
	ASSERT_EQ(operation_type.QueryInterface<InferReturnTypeOperationTypeInterface>(), nullptr);

	Operation op1 = OperationBuilder(infer_operation_type).AddArgument<IntegerValue>(1).AddArgument<BooleanValue>(true).InferReturnValues().Build();
	ASSERT_EQ(op1.GetResultSize(), 2);
	ASSERT_EQ(&op1.GetResult(0).GetType(), &type);
	ASSERT_EQ(&op1.GetResult(1).GetType(), &type);
	ASSERT_EQ(op1.GetResult(0).GetBelongedOperation(), &op1);
	ASSERT_EQ(op1.GetResult(1).GetBelongedOperation(), &op1);

	Operation op2 = OperationBuilder(infer_operation_type).InferReturnValues().Build();
	ASSERT_EQ(op2.GetResultSize(), 0);

	Operation op3 = OperationBuilder(infer_operation_type).AddArgument<IntegerValue>(1).AddResult(type).InferReturnValues().Build();
	ASSERT_EQ(op3.GetResultSize(), 2);

	Operation op4 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).InferReturnValues().Build();
	ASSERT_EQ(op4.GetResultSize(), 0);
}

TEST(OperationBuilder, BuildTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	Operation empty_op = OperationBuilder(operation_type).Build();
	ASSERT_EQ(&empty_op.GetOperationType(), &operation_type);
	ASSERT_EQ(empty_op.GetArgumentSize(), 0);
	ASSERT_EQ(empty_op.GetResultSize(), 0);
	ASSERT_TRUE(empty_op.Verify());

	OperationBuilder builder(operation_type);
	builder.AddArgument<IntegerValue>(1).AddArgument<StringValue>(SGE_STR("test")).AddResult(type);
	Operation op = builder.Build();
	ASSERT_EQ(&op.GetOperationType(), &operation_type);
	ASSERT_EQ(op.GetArgumentSize(), 2);
	ASSERT_EQ(op.GetResultSize(), 1);
	ASSERT_TRUE(op.Verify());
}

TEST(Operation, MoveConstructionTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	Operation op1 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).AddResult(type).AddAttribute<BooleanAttribute>(SGE_STR("flag"), true).Build();
	const Value* pargument = &op1.GetArgument(0);
	const ResultValue* presult = &op1.GetResult(0);
	ASSERT_EQ(pargument->GetBelongedOperation(), &op1);
	ASSERT_EQ(presult->GetBelongedOperation(), &op1);

	Operation op2(std::move(op1));
	ASSERT_EQ(&op2.GetOperationType(), &operation_type);
	const Attribute* pflag = op2.GetAttributes().QueryAttribute(SGE_STR("flag"));
	ASSERT_NE(pflag, nullptr);
	ASSERT_TRUE(DynamicCast<const BooleanAttribute>(*pflag)->GetValue());
	ASSERT_EQ(op2.GetArgumentSize(), 1);
	ASSERT_EQ(op2.GetResultSize(), 1);
	ASSERT_EQ(&op2.GetArgument(0), pargument);
	ASSERT_EQ(&op2.GetResult(0), presult);
	ASSERT_EQ(op2.GetArgument(0).GetBelongedOperation(), &op2);
	ASSERT_EQ(op2.GetResult(0).GetBelongedOperation(), &op2);
	ASSERT_TRUE(op2.Verify());
}

TEST(Operation, MoveAssignmentTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	OperationType& operation_type2 = dialect.GetOperationType<VerifyFailedOperationTypeForTestOperation>();

	Operation op1 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).AddResult(type).AddAttribute<BooleanAttribute>(SGE_STR("flag"), true).Build();
	const Value* pargument = &op1.GetArgument(0);
	const ResultValue* presult = &op1.GetResult(0);

	Operation op2 = OperationBuilder(operation_type2).AddArgument<BooleanValue>(true).AddArgument<IntegerValue>(2).AddResult(type).AddResult(type).AddAttribute<IntegerAttribute>(SGE_STR("int"), 1).Build();
	ASSERT_EQ(op2.GetArgumentSize(), 2);
	ASSERT_EQ(op2.GetResultSize(), 2);

	op2 = std::move(op1);
	ASSERT_EQ(&op2.GetOperationType(), &operation_type);
	ASSERT_EQ(op2.GetAttributes().QueryAttribute(SGE_STR("int")), nullptr);
	const Attribute* pflag = op2.GetAttributes().QueryAttribute(SGE_STR("flag"));
	ASSERT_NE(pflag, nullptr);
	ASSERT_TRUE(DynamicCast<const BooleanAttribute>(*pflag)->GetValue());
	ASSERT_EQ(op2.GetArgumentSize(), 1);
	ASSERT_EQ(op2.GetResultSize(), 1);
	ASSERT_EQ(&op2.GetArgument(0), pargument);
	ASSERT_EQ(&op2.GetResult(0), presult);
	ASSERT_EQ(DynamicCast<const IntegerValue>(op2.GetArgument(0))->GetValue(), 1);
	ASSERT_EQ(op2.GetArgument(0).GetBelongedOperation(), &op2);
	ASSERT_EQ(op2.GetResult(0).GetBelongedOperation(), &op2);
	ASSERT_TRUE(op2.Verify());
}

TEST(Operation, GetOperationTypeTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type1 = dialect.GetOperationType<OperationType1ForTestOperationType>();
	OperationType& operation_type2 = dialect.GetOperationType<VerifyFailedOperationTypeForTestOperation>();

	Operation op1 = OperationBuilder(operation_type1).Build();
	Operation op2 = OperationBuilder(operation_type2).Build();
	ASSERT_EQ(&op1.GetOperationType(), &operation_type1);
	ASSERT_EQ(&op2.GetOperationType(), &operation_type2);
}

TEST(Operation, GetArgumentTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();

	Operation empty_op = OperationBuilder(operation_type).Build();
	ASSERT_EQ(empty_op.GetArgumentSize(), 0);

	Operation op = OperationBuilder(operation_type).AddArgument<IntegerValue>(42).AddArgument<StringValue>(SGE_STR("test")).Build();
	ASSERT_EQ(op.GetArgumentSize(), 2);
	ASSERT_TRUE(IntegerValue::IsInstance(op.GetArgument(0)));
	ASSERT_EQ(DynamicCast<const IntegerValue>(op.GetArgument(0))->GetValue(), 42);
	ASSERT_TRUE(StringValue::IsInstance(op.GetArgument(1)));
	ASSERT_EQ(DynamicCast<const StringValue>(op.GetArgument(1))->GetValue(), SGE_STR("test"));
	ASSERT_EQ(op.GetArgument(0).GetBelongedOperation(), &op);
	ASSERT_EQ(op.GetArgument(1).GetBelongedOperation(), &op);
}

TEST(Operation, GetResultTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();
	Type& type = dialect.GetType<TypeForTestOperation>();

	Operation empty_op = OperationBuilder(operation_type).Build();
	ASSERT_EQ(empty_op.GetResultSize(), 0);

	Operation op = OperationBuilder(operation_type).AddResult(type).AddResult(type).Build();
	ASSERT_EQ(op.GetResultSize(), 2);
	ASSERT_EQ(&op.GetResult(0).GetType(), &type);
	ASSERT_EQ(&op.GetResult(1).GetType(), &type);
	ASSERT_NE(&op.GetResult(0), &op.GetResult(1));
	ASSERT_EQ(op.GetResult(0).GetBelongedOperation(), &op);
	ASSERT_EQ(op.GetResult(1).GetBelongedOperation(), &op);
}

TEST(Operation, GetAttributesTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperation>();
	OperationType& operation_type = dialect.GetOperationType<OperationType1ForTestOperationType>();

	Operation empty_op = OperationBuilder(operation_type).Build();
	SizeType empty_cnt = 0;
	empty_op.GetAttributes().Visit([&](const String&, const Attribute*) { ++empty_cnt; });
	ASSERT_EQ(empty_cnt, 0);
	ASSERT_EQ(empty_op.GetAttributes().QueryAttribute(SGE_STR("int")), nullptr);

	Operation op = OperationBuilder(operation_type)
					   .AddAttribute<IntegerAttribute>(SGE_STR("int"), 42)
					   .AddAttribute<BooleanAttribute>(SGE_STR("flag"), true)
					   .Build();
	const DictionaryAttribute& attributes = op.GetAttributes();
	ASSERT_EQ(&attributes, &op.GetAttributes());

	const Attribute* pint = attributes.QueryAttribute(SGE_STR("int"));
	ASSERT_NE(pint, nullptr);
	ASSERT_TRUE(IntegerAttribute::IsInstance(*pint));
	ASSERT_EQ(DynamicCast<const IntegerAttribute>(*pint)->GetValue(), 42);

	const Attribute* pflag = attributes.QueryAttribute(SGE_STR("flag"));
	ASSERT_NE(pflag, nullptr);
	ASSERT_TRUE(BooleanAttribute::IsInstance(*pflag));
	ASSERT_TRUE(DynamicCast<const BooleanAttribute>(*pflag)->GetValue());

	ASSERT_EQ(attributes.QueryAttribute(SGE_STR("none")), nullptr);

	SizeType cnt = 0;
	attributes.Visit([&](const String&, const Attribute*) { ++cnt; });
	ASSERT_EQ(cnt, 2);
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