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
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

TEST(Value, GetBelongedOperationTest)
{
	Operation op;
	Value value(op);
	ASSERT_EQ(value.GetBelongedOperation(), &op);
}

TEST(Value, DynamicCastTest)
{
	Operation op;
	Value value(op);
	ASSERT_TRUE(Value::IsInstance(value));
	ASSERT_EQ(DynamicCast<Value>(value), &value);
}

TEST(ResultValue, GetTypeTest)
{
	Operation op;
	ResultValue result(op);
	ASSERT_EQ(result.GetType(), nullptr);

	Type test_type;
	ResultValue result_with_type(op, &test_type);
	ASSERT_EQ(result_with_type.GetType(), &test_type);
}

TEST(ResultValue, ReferenceManagementTest)
{
	Operation op;
	ResultValue result(op);
	ASSERT_EQ(result.GetFirstReference(), nullptr);

	{
		ReferenceValue ref1(op, result);
		ASSERT_EQ(ref1.GetResultValue(), &result);
		ASSERT_EQ(result.GetFirstReference(), &ref1);
		ASSERT_EQ(ref1.GetPrevious(), &ref1);
		ASSERT_EQ(ref1.GetNext(), &ref1);

		ReferenceValue ref2(op, result);
		ASSERT_EQ(ref2.GetResultValue(), &result);
		ASSERT_EQ(result.GetFirstReference(), &ref1);
		ASSERT_EQ(ref1.GetNext(), &ref2);
		ASSERT_EQ(ref1.GetPrevious(), &ref2);
		ASSERT_EQ(ref2.GetPrevious(), &ref1);
		ASSERT_EQ(ref2.GetNext(), &ref1);

		{
			ReferenceValue ref3(op, result);
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
	Operation op;
	ResultValue result(op);
	ASSERT_TRUE(ResultValue::IsInstance(result));

	Value& base_ref = result;
	ASSERT_EQ(DynamicCast<ResultValue>(base_ref), &result);
}

TEST(ReferenceValue, DynamicCastTest)
{
	Operation op;
	ResultValue result(op);
	ReferenceValue ref(op, result);
	ASSERT_TRUE(ReferenceValue::IsInstance(ref));
	Value& base_ref = ref;
	ASSERT_EQ(DynamicCast<ReferenceValue>(base_ref), &ref);
}

TEST(TypeValue, GetTypeTest)
{
	Type test_type;
	Operation op;
	TypeValue type_value(op);
	ASSERT_EQ(type_value.GetType(), nullptr);
	TypeValue type_value_with_type(op, &test_type);
	ASSERT_EQ(type_value_with_type.GetType(), &test_type);
}

TEST(TypeValue, SetTypeTest)
{
	Type test_type1, test_type2;
	Operation op;
	TypeValue type_value(op, &test_type1);
	ASSERT_EQ(type_value.GetType(), &test_type1);
	type_value.SetType(&test_type2);
	ASSERT_EQ(type_value.GetType(), &test_type2);
}

TEST(TypeValue, DynamicCastTest)
{
	Operation op;
	TypeValue type_value(op);
	ASSERT_TRUE(TypeValue::IsInstance(type_value));
	Value& base_ref = type_value;
	ASSERT_EQ(DynamicCast<TypeValue>(base_ref), &type_value);
}

TEST(OperationTypeValue, SetOperationTypeTest)
{
	OperationType test_operation_type1, test_operation_type2;
	Operation op;
	OperationTypeValue operation_type_value(op, &test_operation_type1);
	ASSERT_EQ(operation_type_value.GetOperationType(), &test_operation_type1);
	operation_type_value.SetOperationType(&test_operation_type2);
	ASSERT_EQ(operation_type_value.GetOperationType(), &test_operation_type2);
}

TEST(OperationTypeValue, GetOperationTypeTest)
{
	OperationType test_operation_type;
	Operation op;
	OperationTypeValue operation_type_value(op);
	ASSERT_EQ(operation_type_value.GetOperationType(), nullptr);
	OperationTypeValue operation_type_value_with_type(op, &test_operation_type);
	ASSERT_EQ(operation_type_value_with_type.GetOperationType(), &test_operation_type);
}

TEST(OperationTypeValue, DynamicCastTest)
{
	Operation op;
	OperationTypeValue operation_type_value(op);
	ASSERT_TRUE(OperationTypeValue::IsInstance(operation_type_value));
	Value& base_ref = operation_type_value;
	ASSERT_EQ(DynamicCast<OperationTypeValue>(base_ref), &operation_type_value);
}

TEST(BlockValue, GetOperationsTest)
{
	Operation op;
	BlockValue block_value(op);
	ASSERT_EQ(block_value.GetOperations().GetSize(), 0);
	Operation op1, op2;
	List<Operation> operations;
	operations.PushBack(op1);
	operations.PushBack(op2);
	BlockValue block_value_with_ops(op, std::move(operations));
	ASSERT_EQ(block_value_with_ops.GetOperations().GetSize(), 2);
	ASSERT_EQ(*block_value_with_ops.GetOperations().GetBegin(), op1);
	ASSERT_EQ(*(block_value_with_ops.GetOperations().GetBegin() + 1), op2);
}

TEST(BlockValue, DynamicCastTest)
{
	Operation op;
	BlockValue block_value(op);
	ASSERT_TRUE(BlockValue::IsInstance(block_value));
	Value& base_ref = block_value;
	ASSERT_EQ(DynamicCast<BlockValue>(base_ref), &block_value);
}

TEST(IntegerValue, SetValueTest)
{
	Operation op;
	IntegerValue int_value(op);
	int_value.SetValue(42);
	ASSERT_EQ(int_value.GetValue(), 42);
	int_value.SetValue(-1);
	ASSERT_EQ(int_value.GetValue(), -1);
}

TEST(IntegerValue, GetValueTest)
{
	Operation op;
	IntegerValue int_value(op, 100);
	ASSERT_EQ(int_value.GetValue(), 100);
	int_value.SetValue(-50);
	ASSERT_EQ(int_value.GetValue(), -50);
}

TEST(IntegerValue, DynamicCastTest)
{
	Operation op;
	IntegerValue int_value(op);
	ASSERT_TRUE(IntegerValue::IsInstance(int_value));
	Value& base_ref = int_value;
	ASSERT_EQ(DynamicCast<IntegerValue>(base_ref), &int_value);
}

TEST(FloatValue, SetValueTest)
{
	Operation op;
	FloatValue float_value(op);
	float_value.SetValue(3.14f);
	ASSERT_FLOAT_EQ(float_value.GetValue(), 3.14f);
	float_value.SetValue(-2.71f);
	ASSERT_FLOAT_EQ(float_value.GetValue(), -2.71f);
}

TEST(FloatValue, GetValueTest)
{
	Operation op;
	FloatValue float_value(op, 1.618f);
	ASSERT_FLOAT_EQ(float_value.GetValue(), 1.618f);
	float_value.SetValue(-0.577f);
	ASSERT_FLOAT_EQ(float_value.GetValue(), -0.577f);
}

TEST(FloatValue, DynamicCastTest)
{
	Operation op;
	FloatValue float_value(op);
	ASSERT_TRUE(FloatValue::IsInstance(float_value));
	Value& base_ref = float_value;
	ASSERT_EQ(DynamicCast<FloatValue>(base_ref), &float_value);
}

TEST(DoubleValue, SetValueTest)
{
	Operation op;
	DoubleValue double_value(op);
	double_value.SetValue(3.141592653589793);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), 3.141592653589793);
	double_value.SetValue(-2.718281828459045);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), -2.718281828459045);
}

TEST(DoubleValue, GetValueTest)
{
	Operation op;
	DoubleValue double_value(op, 1.4142135623730951);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), 1.4142135623730951);
	double_value.SetValue(-0.6931471805599453);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), -0.6931471805599453);
}

TEST(DoubleValue, DynamicCastTest)
{
	Operation op;
	DoubleValue double_value(op);
	ASSERT_TRUE(DoubleValue::IsInstance(double_value));
	Value& base_ref = double_value;
	ASSERT_EQ(DynamicCast<DoubleValue>(base_ref), &double_value);
}

TEST(BooleanValue, SetValueTest)
{
	Operation op;
	BooleanValue bool_value(op);
	bool_value.SetValue(true);
	ASSERT_TRUE(bool_value.GetValue());
	bool_value.SetValue(false);
	ASSERT_FALSE(bool_value.GetValue());
}

TEST(BooleanValue, GetValueTest)
{
	Operation op;
	BooleanValue bool_value(op, true);
	ASSERT_TRUE(bool_value.GetValue());
	bool_value.SetValue(false);
	ASSERT_FALSE(bool_value.GetValue());
}

TEST(BooleanValue, DynamicCastTest)
{
	Operation op;
	BooleanValue bool_value(op);
	ASSERT_TRUE(BooleanValue::IsInstance(bool_value));
	Value& base_ref = bool_value;
	ASSERT_EQ(DynamicCast<BooleanValue>(base_ref), &bool_value);
}

TEST(StringValue, SetValueTest)
{
	Operation op;
	StringValue string_value(op);
	String test_string(SGE_STR("Hello, World!"));
	string_value.SetValue(test_string);
	ASSERT_EQ(string_value.GetValue(), SGE_STR("Hello, World!"));
	string_value.SetValue(SGE_STR("Test"));
	ASSERT_EQ(string_value.GetValue(), SGE_STR("Test"));
}

TEST(StringValue, GetValueTest)
{
	Operation op;
	StringValue string_value(op, SGE_STR("Initial Value"));
	ASSERT_EQ(string_value.GetValue(), SGE_STR("Initial Value"));
	string_value.SetValue(SGE_STR("Changed Value"));
	ASSERT_EQ(string_value.GetValue(), SGE_STR("Changed Value"));
	String test_string(SGE_STR("Another Value"));
	StringValue string_value2(op, test_string);
	ASSERT_EQ(string_value2.GetValue(), SGE_STR("Another Value"));
}

TEST(StringValue, DynamicCastTest)
{
	Operation op;
	StringValue string_value(op);
	ASSERT_TRUE(StringValue::IsInstance(string_value));
	Value& base_ref = string_value;
	ASSERT_EQ(DynamicCast<StringValue>(base_ref), &string_value);
}

TEST(ListValue, AddValueTest)
{
	Operation op;
	ListValue list_value(op);
	Value& new_value = list_value.AddValue<DoubleValue>(op, 3.14);
	ASSERT_TRUE(DoubleValue::IsInstance(new_value));
	DoubleValue& double_value = static_cast<DoubleValue&>(new_value);
	ASSERT_DOUBLE_EQ(double_value.GetValue(), 3.14);
	ASSERT_EQ(list_value.GetValues().GetSize(), 1);
	ASSERT_EQ(list_value.GetValues()[0], &double_value);
}

TEST(ListValue, RemoveValueTest)
{
	Operation op;
	ListValue list_value(op);
	Value& new_value1 = list_value.AddValue<DoubleValue>(op, 3.14);
	Value& new_value2 = list_value.AddValue<BooleanValue>(op, true);
	ASSERT_EQ(list_value.GetValues().GetSize(), 2);
	ASSERT_TRUE(list_value.RemoveValue(new_value1));
	ASSERT_EQ(list_value.GetValues().GetSize(), 1);
	ASSERT_EQ(list_value.GetValues()[0], &new_value2);
	ASSERT_FALSE(list_value.RemoveValue(new_value1));	 // Already removed
}

TEST(ListValue, GetValuesTest)
{
	Operation op;
	ListValue list_value(op);
	Value& new_value1 = list_value.AddValue<DoubleValue>(op, 3.14);
	Value& new_value2 = list_value.AddValue<BooleanValue>(op, true);
	Value& new_value3 = list_value.AddValue<StringValue>(op, SGE_STR("Test"));
	const Vector<const Value*>& values = list_value.GetValues();
	ASSERT_EQ(values.GetSize(), 3);
	ASSERT_EQ(values[0], &new_value1);
	ASSERT_EQ(values[1], &new_value2);
	ASSERT_EQ(values[2], &new_value3);
}

TEST(ListValue, DynamicCastTest)
{
	Operation op;
	ListValue list_value(op);
	ASSERT_TRUE(ListValue::IsInstance(list_value));
	Value& base_ref = list_value;
	ASSERT_EQ(DynamicCast<ListValue>(base_ref), &list_value);
}