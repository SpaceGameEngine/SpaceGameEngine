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
#include "Value.h"
#include "Dialect.h"
#include "Context.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

class OperationTypeForTestValue : public OperationType, public DynamicCastHelperForDerived<OperationTypeForTestValue, OperationType>
{
public:
	friend class Dialect;

	inline OperationTypeForTestValue(Dialect& dialect)
		: OperationType(SGE_STR("OperationTypeForTestValue"), dialect)
	{
	}

	using DynamicCastHelperForDerived<OperationTypeForTestValue, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, OperationTypeForTestValue);
SGE_DEFINE_TYPE_ID(, OperationTypeForTestValue);

class TypeForTestValue : public Type, public DynamicCastHelperForDerived<TypeForTestValue, Type>
{
public:
	friend class Dialect;

	inline TypeForTestValue(Dialect& dialect)
		: Type(SGE_STR("TypeForTestValue"), dialect)
	{
	}

	using DynamicCastHelperForDerived<TypeForTestValue, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TypeForTestValue);
SGE_DEFINE_TYPE_ID(, TypeForTestValue);

class DialectForTestValue : public Dialect, public DynamicCastHelperForDerived<DialectForTestValue, Dialect>
{
public:
	friend class Context;

	inline DialectForTestValue(Context& context)
		: Dialect(SGE_STR("DialectForTestValue"), context)
	{
		AddOperationType<OperationTypeForTestValue>();
		AddType<TypeForTestValue>();
	}

	using DynamicCastHelperForDerived<DialectForTestValue, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestValue);
SGE_DEFINE_TYPE_ID(, DialectForTestValue);

inline DialectForTestValue& GetDialectForTestValue(Context& context)
{
	return context.HasDialect<DialectForTestValue>() ? context.GetDialect<DialectForTestValue>() : context.AddDialect<DialectForTestValue>();
}

inline OperationType& GetOperationTypeForTestValue(Context& context)
{
	return GetDialectForTestValue(context).GetOperationType<OperationTypeForTestValue>();
}

inline Type& GetTypeForTestValue(Context& context)
{
	return GetDialectForTestValue(context).GetType<TypeForTestValue>();
}

TEST(Value, GetBelongedOperationTest)
{
	Value value;
	ASSERT_EQ(value.GetBelongedOperation(), nullptr);
}

TEST(Value, DynamicCastTest)
{
	Value value;
	IntegerValue int_value(1);
	ASSERT_TRUE(Value::IsInstance(value));
	ASSERT_TRUE(Value::IsInstance(int_value));	  // Value is a base class of IntegerValue, so this should return true
	ASSERT_EQ(DynamicCast<Value>(value), &value);
}

TEST(Value, GetTypeIdTest)
{
	Value value;

	ASSERT_EQ(value.GetTypeId(), GetTypeId<Value>());
}

class TypeForTestResultValue : public Type, public DynamicCastHelperForDerived<TypeForTestResultValue, Type>
{
public:
	friend class Dialect;
	inline TypeForTestResultValue(Dialect& dialect)
		: Type(SGE_STR("TypeForTestResultValue"), dialect)
	{
	}
	using DynamicCastHelperForDerived<TypeForTestResultValue, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, TypeForTestResultValue);
SGE_DEFINE_TYPE_ID(, TypeForTestResultValue);

class DialectForTestResultValue : public Dialect, public DynamicCastHelperForDerived<DialectForTestResultValue, Dialect>
{
public:
	friend class Context;
	inline DialectForTestResultValue(Context& context)
		: Dialect(SGE_STR("DialectForTestResultValue"), context)
	{
		AddType<TypeForTestResultValue>();
	}
	using DynamicCastHelperForDerived<DialectForTestResultValue, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestResultValue);
SGE_DEFINE_TYPE_ID(, DialectForTestResultValue);

TEST(ResultValue, GetTypeTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestResultValue>();
	Type& test_type = dialect.GetType<TypeForTestResultValue>();
	ResultValue result_with_type(test_type);
	ASSERT_EQ(&result_with_type.GetType(), &test_type);

	ResultValue result(GetTypeForTestValue(context));
	ASSERT_EQ(&result.GetType(), &GetTypeForTestValue(context));
}

TEST(ResultValue, ReferenceManagementTest)
{
	Context op_context;
	ResultValue result(GetTypeForTestValue(op_context));
	ASSERT_EQ(result.GetFirstReference(), nullptr);

	{
		ReferenceValue ref1(result);
		ASSERT_EQ(ref1.GetResultValue(), &result);
		ASSERT_EQ(result.GetFirstReference(), &ref1);
		ASSERT_EQ(ref1.GetPrevious(), &ref1);
		ASSERT_EQ(ref1.GetNext(), &ref1);

		ReferenceValue ref2(result);
		ASSERT_EQ(ref2.GetResultValue(), &result);
		ASSERT_EQ(result.GetFirstReference(), &ref1);
		ASSERT_EQ(ref1.GetNext(), &ref2);
		ASSERT_EQ(ref1.GetPrevious(), &ref2);
		ASSERT_EQ(ref2.GetPrevious(), &ref1);
		ASSERT_EQ(ref2.GetNext(), &ref1);

		{
			ReferenceValue ref3(result);
			ASSERT_EQ(ref3.GetResultValue(), &result);
			ASSERT_EQ(result.GetFirstReference(), &ref1);
			ASSERT_EQ(ref1.GetNext(), &ref2);
			ASSERT_EQ(ref1.GetPrevious(), &ref3);
			ASSERT_EQ(ref2.GetPrevious(), &ref1);
			ASSERT_EQ(ref2.GetNext(), &ref3);
			ASSERT_EQ(ref3.GetPrevious(), &ref2);
			ASSERT_EQ(ref3.GetNext(), &ref1);
		}

		ASSERT_EQ(ref2.GetResultValue(), &result);
		ASSERT_EQ(result.GetFirstReference(), &ref1);
		ASSERT_EQ(ref1.GetNext(), &ref2);
		ASSERT_EQ(ref1.GetPrevious(), &ref2);
		ASSERT_EQ(ref2.GetPrevious(), &ref1);
		ASSERT_EQ(ref2.GetNext(), &ref1);
	}

	ASSERT_EQ(result.GetFirstReference(), nullptr);
}

TEST(ResultValue, DynamicCastTest)
{
	Context op_context;
	ResultValue result(GetTypeForTestValue(op_context));
	ReferenceValue ref(result);
	Value& base_ref = result;

	ASSERT_TRUE(ResultValue::IsInstance(result));
	ASSERT_FALSE(ResultValue::IsInstance(ref));
	ASSERT_EQ(DynamicCast<ResultValue>(base_ref), &result);
}

TEST(ResultValue, GetTypeIdTest)
{
	Context op_context;
	ResultValue result(GetTypeForTestValue(op_context));

	ASSERT_EQ(result.GetTypeId(), GetTypeId<ResultValue>());
}

TEST(ReferenceValue, DynamicCastTest)
{
	Context op_context;
	ResultValue result(GetTypeForTestValue(op_context));
	ReferenceValue ref(result);
	Value& base_ref = ref;

	ASSERT_TRUE(ReferenceValue::IsInstance(ref));
	ASSERT_FALSE(ReferenceValue::IsInstance(result));
	ASSERT_EQ(DynamicCast<ReferenceValue>(base_ref), &ref);
}

TEST(ReferenceValue, GetTypeIdTest)
{
	Context op_context;
	ResultValue result(GetTypeForTestValue(op_context));
	ReferenceValue ref(result);

	ASSERT_EQ(ref.GetTypeId(), GetTypeId<ReferenceValue>());
}

class Type1ForTestTypeValue : public Type, public DynamicCastHelperForDerived<Type1ForTestTypeValue, Type>
{
public:
	friend class Dialect;

	inline Type1ForTestTypeValue(Dialect& dialect)
		: Type(SGE_STR("Type1ForTestTypeValue"), dialect)
	{
	}

	using DynamicCastHelperForDerived<Type1ForTestTypeValue, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, Type1ForTestTypeValue);
SGE_DEFINE_TYPE_ID(, Type1ForTestTypeValue);

class Type2ForTestTypeValue : public Type, public DynamicCastHelperForDerived<Type2ForTestTypeValue, Type>
{
public:
	friend class Dialect;
	inline Type2ForTestTypeValue(Dialect& dialect)
		: Type(SGE_STR("Type2ForTestTypeValue"), dialect)
	{
	}
	using DynamicCastHelperForDerived<Type2ForTestTypeValue, Type>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, Type2ForTestTypeValue);
SGE_DEFINE_TYPE_ID(, Type2ForTestTypeValue);

class DialectForTestTypeValue : public Dialect, public DynamicCastHelperForDerived<DialectForTestTypeValue, Dialect>
{
public:
	friend class Context;
	inline DialectForTestTypeValue(Context& context)
		: Dialect(SGE_STR("DialectForTestTypeValue"), context)
	{
		AddType<Type1ForTestTypeValue>();
		AddType<Type2ForTestTypeValue>();
	}
	using DynamicCastHelperForDerived<DialectForTestTypeValue, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestTypeValue);
SGE_DEFINE_TYPE_ID(, DialectForTestTypeValue);

TEST(TypeValue, GetTypeTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestTypeValue>();
	Type& test_type = dialect.GetType<Type1ForTestTypeValue>();
	TypeValue type_value_with_type(test_type);
	ASSERT_EQ(&type_value_with_type.GetType(), &test_type);
}

TEST(TypeValue, DynamicCastTest)
{
	Context op_context;
	TypeValue type_value(GetTypeForTestValue(op_context));
	OperationTypeValue operation_type_value(GetOperationTypeForTestValue(op_context));
	Value& base_ref = type_value;

	ASSERT_TRUE(TypeValue::IsInstance(type_value));
	ASSERT_FALSE(TypeValue::IsInstance(operation_type_value));
	ASSERT_EQ(DynamicCast<TypeValue>(base_ref), &type_value);
}

TEST(TypeValue, GetTypeIdTest)
{
	Context op_context;
	TypeValue type_value(GetTypeForTestValue(op_context));

	ASSERT_EQ(type_value.GetTypeId(), GetTypeId<TypeValue>());
}

class OperationType1ForTestOperationTypeValue : public OperationType, public DynamicCastHelperForDerived<OperationType1ForTestOperationTypeValue, OperationType>
{
public:
	friend class Dialect;

	inline OperationType1ForTestOperationTypeValue(Dialect& dialect)
		: OperationType(SGE_STR("OperationType1ForTestOperationTypeValue"), dialect)
	{
	}

	using DynamicCastHelperForDerived<OperationType1ForTestOperationTypeValue, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, OperationType1ForTestOperationTypeValue);
SGE_DEFINE_TYPE_ID(, OperationType1ForTestOperationTypeValue);

class OperationType2ForTestOperationTypeValue : public OperationType, public DynamicCastHelperForDerived<OperationType2ForTestOperationTypeValue, OperationType>
{
public:
	friend class Dialect;

	inline OperationType2ForTestOperationTypeValue(Dialect& dialect)
		: OperationType(SGE_STR("OperationType2ForTestOperationTypeValue"), dialect)
	{
	}

	using DynamicCastHelperForDerived<OperationType2ForTestOperationTypeValue, OperationType>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, OperationType2ForTestOperationTypeValue);
SGE_DEFINE_TYPE_ID(, OperationType2ForTestOperationTypeValue);

class DialectForTestOperationTypeValue : public Dialect, public DynamicCastHelperForDerived<DialectForTestOperationTypeValue, Dialect>
{
public:
	friend class Context;

	inline DialectForTestOperationTypeValue(Context& context)
		: Dialect(SGE_STR("DialectForTestOperationTypeValue"), context)
	{
		AddOperationType<OperationType1ForTestOperationTypeValue>();
		AddOperationType<OperationType2ForTestOperationTypeValue>();
	}

	using DynamicCastHelperForDerived<DialectForTestOperationTypeValue, Dialect>::IsInstance;
};

SGE_DECLARE_TYPE_ID(, DialectForTestOperationTypeValue);
SGE_DEFINE_TYPE_ID(, DialectForTestOperationTypeValue);

TEST(OperationTypeValue, GetOperationTypeTest)
{
	Context context;
	Dialect& dialect = context.AddDialect<DialectForTestOperationTypeValue>();
	OperationType& test_operation_type = dialect.GetOperationType<OperationType1ForTestOperationTypeValue>();
	OperationTypeValue operation_type_value_with_type(test_operation_type);
	ASSERT_EQ(&operation_type_value_with_type.GetOperationType(), &test_operation_type);
}

TEST(OperationTypeValue, DynamicCastTest)
{
	Context op_context;
	OperationTypeValue operation_type_value(GetOperationTypeForTestValue(op_context));
	BlockValue block_value(List<Operation>{});
	Value& base_ref = operation_type_value;

	ASSERT_TRUE(OperationTypeValue::IsInstance(operation_type_value));
	ASSERT_FALSE(OperationTypeValue::IsInstance(block_value));
	ASSERT_EQ(DynamicCast<OperationTypeValue>(base_ref), &operation_type_value);
}

TEST(OperationTypeValue, GetTypeIdTest)
{
	Context op_context;
	OperationTypeValue operation_type_value(GetOperationTypeForTestValue(op_context));

	ASSERT_EQ(operation_type_value.GetTypeId(), GetTypeId<OperationTypeValue>());
}

TEST(BlockValue, GetOperationsTest)
{
	Context context;
	OperationType& operation_type = GetOperationTypeForTestValue(context);
	BlockValue block_value(List<Operation>{});
	ASSERT_EQ(block_value.GetOperations().GetSize(), 0);
	Operation op1 = OperationBuilder(operation_type).AddArgument<IntegerValue>(1).Build();
	Operation op2 = OperationBuilder(operation_type).AddArgument<IntegerValue>(2).Build();
	List<Operation> operations;
	operations.EmplaceBack(OperationBuilder(operation_type).AddArgument<IntegerValue>(1).Build());
	operations.EmplaceBack(OperationBuilder(operation_type).AddArgument<IntegerValue>(2).Build());
	BlockValue block_value_with_ops(std::move(operations));
	ASSERT_EQ(block_value_with_ops.GetOperations().GetSize(), 2);
	ASSERT_EQ(*block_value_with_ops.GetOperations().GetBegin(), op1);
	ASSERT_EQ(*(block_value_with_ops.GetOperations().GetBegin() + 1), op2);
}

TEST(BlockValue, DynamicCastTest)
{
	BlockValue block_value(List<Operation>{});
	IntegerValue int_value(0);
	Value& base_ref = block_value;

	ASSERT_TRUE(BlockValue::IsInstance(block_value));
	ASSERT_FALSE(BlockValue::IsInstance(int_value));
	ASSERT_EQ(DynamicCast<BlockValue>(base_ref), &block_value);
}

TEST(BlockValue, GetTypeIdTest)
{
	BlockValue block_value(List<Operation>{});

	ASSERT_EQ(block_value.GetTypeId(), GetTypeId<BlockValue>());
}

TEST(IntegerValue, GetValueTest)
{
	IntegerValue int_value(100);
	ASSERT_EQ(int_value.GetValue(), 100);
	IntegerValue int_value2(0);
	ASSERT_EQ(int_value2.GetValue(), 0);
}

TEST(IntegerValue, DynamicCastTest)
{
	IntegerValue int_value(0);
	FloatValue float_value(0.0f);
	Value& base_ref = int_value;

	ASSERT_TRUE(IntegerValue::IsInstance(int_value));
	ASSERT_FALSE(IntegerValue::IsInstance(float_value));
	ASSERT_EQ(DynamicCast<IntegerValue>(base_ref), &int_value);
}

TEST(IntegerValue, GetTypeIdTest)
{
	IntegerValue int_value(0);

	ASSERT_EQ(int_value.GetTypeId(), GetTypeId<IntegerValue>());
}

TEST(FloatValue, GetValueTest)
{
	FloatValue float_value(1.618f);
	ASSERT_FLOAT_EQ(float_value.GetValue(), 1.618f);
	FloatValue float_value2(-0.577f);
	ASSERT_FLOAT_EQ(float_value2.GetValue(), -0.577f);
}

TEST(FloatValue, DynamicCastTest)
{
	FloatValue float_value(0.0f);
	DoubleValue double_value(0.0);
	Value& base_ref = float_value;

	ASSERT_TRUE(FloatValue::IsInstance(float_value));
	ASSERT_FALSE(FloatValue::IsInstance(double_value));
	ASSERT_EQ(DynamicCast<FloatValue>(base_ref), &float_value);
}

TEST(FloatValue, GetTypeIdTest)
{
	FloatValue float_value(0.0f);

	ASSERT_EQ(float_value.GetTypeId(), GetTypeId<FloatValue>());
}

TEST(DoubleValue, GetValueTest)
{
	DoubleValue double_value(1.4142135623730951);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), 1.4142135623730951);
	DoubleValue double_value2(-0.6931471805599453);
	ASSERT_DOUBLE_EQ(double_value2.GetValue(), -0.6931471805599453);
}

TEST(DoubleValue, DynamicCastTest)
{
	DoubleValue double_value(0.0);
	BooleanValue bool_value(false);
	Value& base_ref = double_value;

	ASSERT_TRUE(DoubleValue::IsInstance(double_value));
	ASSERT_FALSE(DoubleValue::IsInstance(bool_value));
	ASSERT_EQ(DynamicCast<DoubleValue>(base_ref), &double_value);
}

TEST(DoubleValue, GetTypeIdTest)
{
	DoubleValue double_value(0.0);

	ASSERT_EQ(double_value.GetTypeId(), GetTypeId<DoubleValue>());
}

TEST(BooleanValue, GetValueTest)
{
	BooleanValue bool_value(true);
	ASSERT_TRUE(bool_value.GetValue());
	BooleanValue bool_value2(false);
	ASSERT_FALSE(bool_value2.GetValue());
}

TEST(BooleanValue, DynamicCastTest)
{
	BooleanValue bool_value(false);
	StringValue string_value(SGE_STR(""));
	Value& base_ref = bool_value;

	ASSERT_TRUE(BooleanValue::IsInstance(bool_value));
	ASSERT_FALSE(BooleanValue::IsInstance(string_value));
	ASSERT_EQ(DynamicCast<BooleanValue>(base_ref), &bool_value);
}

TEST(BooleanValue, GetTypeIdTest)
{
	BooleanValue bool_value(false);

	ASSERT_EQ(bool_value.GetTypeId(), GetTypeId<BooleanValue>());
}

TEST(StringValue, GetValueTest)
{
	StringValue string_value(SGE_STR("Initial Value"));
	ASSERT_EQ(string_value.GetValue(), SGE_STR("Initial Value"));
	const String test_string(SGE_STR("Another Value"));
	StringValue string_value2(test_string);
	ASSERT_EQ(string_value2.GetValue(), SGE_STR("Another Value"));
	String moved_string(SGE_STR("Moved Value"));
	StringValue string_value3(std::move(moved_string));
	ASSERT_EQ(string_value3.GetValue(), SGE_STR("Moved Value"));
}

TEST(StringValue, DynamicCastTest)
{
	StringValue string_value(SGE_STR(""));
	IntegerValue int_value(0);
	Value& base_ref = string_value;

	ASSERT_TRUE(StringValue::IsInstance(string_value));
	ASSERT_FALSE(StringValue::IsInstance(int_value));
	ASSERT_EQ(DynamicCast<StringValue>(base_ref), &string_value);
}

TEST(StringValue, GetTypeIdTest)
{
	StringValue string_value(SGE_STR(""));

	ASSERT_EQ(string_value.GetTypeId(), GetTypeId<StringValue>());
}

TEST(ListValue, GetValuesTest)
{
	ListValue empty_list_value(Vector<const Value*>{});
	ASSERT_EQ(empty_list_value.GetValues().GetSize(), 0);

	Value* new_value1 = DefaultAllocator::New<DoubleValue>(3.14);
	Value* new_value2 = DefaultAllocator::New<BooleanValue>(true);
	Value* new_value3 = DefaultAllocator::New<StringValue>(SGE_STR("Test"));
	Vector<const Value*> input_values;
	input_values.PushBack(new_value1);
	input_values.PushBack(new_value2);
	input_values.PushBack(new_value3);
	ListValue list_value(std::move(input_values));
	const Vector<const Value*>& values = list_value.GetValues();
	ASSERT_EQ(values.GetSize(), 3);
	ASSERT_EQ(values[0], new_value1);
	ASSERT_EQ(values[1], new_value2);
	ASSERT_EQ(values[2], new_value3);
	ASSERT_DOUBLE_EQ(static_cast<const DoubleValue*>(values[0])->GetValue(), 3.14);
	ASSERT_TRUE(static_cast<const BooleanValue*>(values[1])->GetValue());
	ASSERT_EQ(static_cast<const StringValue*>(values[2])->GetValue(), SGE_STR("Test"));
}

TEST(ListValue, DynamicCastTest)
{
	ListValue list_value(Vector<const Value*>{});
	BooleanValue bool_value(false);
	Value& base_ref = list_value;

	ASSERT_TRUE(ListValue::IsInstance(list_value));
	ASSERT_FALSE(ListValue::IsInstance(bool_value));
	ASSERT_EQ(DynamicCast<ListValue>(base_ref), &list_value);
}

TEST(ListValue, GetTypeIdTest)
{
	ListValue list_value(Vector<const Value*>{});

	ASSERT_EQ(list_value.GetTypeId(), GetTypeId<ListValue>());
}