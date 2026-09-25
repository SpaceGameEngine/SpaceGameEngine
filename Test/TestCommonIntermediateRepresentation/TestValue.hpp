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

	ASSERT_NE(value.GetTypeId(), 0);
	ASSERT_EQ(value.GetTypeId(), SpaceGameEngine::GetTypeId<Value>());
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

	ASSERT_NE(result.GetTypeId(), 0);
	ASSERT_EQ(result.GetTypeId(), SpaceGameEngine::GetTypeId<ResultValue>());
}

TEST(ReferenceValue, DynamicCastTest)
{
	ResultValue result;
	ReferenceValue ref(result);
	ASSERT_TRUE(ReferenceValue::IsInstance(ref));
	Value& base_ref = ref;
	ASSERT_EQ(DynamicCast<ReferenceValue>(base_ref), &ref);
	ASSERT_NE(ref.GetTypeId(), 0);
	ASSERT_EQ(ref.GetTypeId(), SpaceGameEngine::GetTypeId<ReferenceValue>());
}