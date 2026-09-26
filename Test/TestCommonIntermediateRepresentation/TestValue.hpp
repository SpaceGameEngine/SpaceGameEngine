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

TEST(Value, DynamicCastTest)
{
	Value value;
	ASSERT_TRUE(Value::IsInstance(value));
	ASSERT_EQ(DynamicCast<Value>(value), &value);
}

TEST(ResultValue, GetTypeTest)
{
	ResultValue result;
	ASSERT_EQ(result.GetType(), nullptr);

	Type test_type;
	ResultValue result_with_type(&test_type);
	ASSERT_EQ(result_with_type.GetType(), &test_type);
}

TEST(ResultValue, ReferenceManagementTest)
{
	ResultValue result;
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
	ResultValue result;
	ASSERT_TRUE(ResultValue::IsInstance(result));

	Value& base_ref = result;
	ASSERT_EQ(DynamicCast<ResultValue>(base_ref), &result);
}

TEST(ReferenceValue, DynamicCastTest)
{
	ResultValue result;
	ReferenceValue ref(result);
	ASSERT_TRUE(ReferenceValue::IsInstance(ref));
	Value& base_ref = ref;
	ASSERT_EQ(DynamicCast<ReferenceValue>(base_ref), &ref);
}

TEST(TypeValue, GetTypeTest)
{
	Type test_type;
	TypeValue type_value;
	ASSERT_EQ(type_value.GetType(), nullptr);
	TypeValue type_value_with_type(&test_type);
	ASSERT_EQ(type_value_with_type.GetType(), &test_type);
}

TEST(TypeValue, DynamicCastTest)
{
	TypeValue type_value;
	ASSERT_TRUE(TypeValue::IsInstance(type_value));
	Value& base_ref = type_value;
	ASSERT_EQ(DynamicCast<TypeValue>(base_ref), &type_value);
}

TEST(OperationTypeValue, GetOperationTypeTest)
{
	OperationType test_operation_type;
	OperationTypeValue operation_type_value;
	ASSERT_EQ(operation_type_value.GetOperationType(), nullptr);
	OperationTypeValue operation_type_value_with_type(&test_operation_type);
	ASSERT_EQ(operation_type_value_with_type.GetOperationType(), &test_operation_type);
}

TEST(OperationTypeValue, DynamicCastTest)
{
	OperationTypeValue operation_type_value;
	ASSERT_TRUE(OperationTypeValue::IsInstance(operation_type_value));
	Value& base_ref = operation_type_value;
	ASSERT_EQ(DynamicCast<OperationTypeValue>(base_ref), &operation_type_value);
}