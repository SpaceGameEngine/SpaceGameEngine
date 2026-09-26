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
#include "Value.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

Value::Value(Operation& operation)
	: m_BelongedOperation(&operation)
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::Operation* Value::GetBelongedOperation() const
{
	return m_BelongedOperation;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Value);

ResultValue::ResultValue(Operation& operation)
	: Value(operation), m_Type(nullptr), m_FirstReference(nullptr)
{
}

ResultValue::~ResultValue()
{
}

ResultValue::ResultValue(Operation& operation, const Type* type)
	: Value(operation), m_Type(type), m_FirstReference(nullptr)
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::Type* ResultValue::GetType() const
{
	return m_Type;
}

const SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue* ResultValue::GetFirstReference() const
{
	return m_FirstReference;
}

void ResultValue::AddReference(ReferenceValue& ref)
{
	if (!m_FirstReference)
	{
		m_FirstReference = &ref;
		m_FirstReference->m_Next = m_FirstReference;
		m_FirstReference->m_Previous = m_FirstReference;
	}
	else
	{
		auto tail = m_FirstReference->m_Previous;
		tail->m_Next = &ref;
		m_FirstReference->m_Previous = &ref;
		ref.m_Next = m_FirstReference;
		ref.m_Previous = tail;
	}
	ref.m_ResultValue = this;
}

bool ResultValue::RemoveReference(ReferenceValue& ref)
{
	auto cur = m_FirstReference;
	do
	{
		if (cur == &ref)
			break;
		else
			cur = cur->m_Next;
	} while (cur != m_FirstReference);
	if (cur == &ref)
	{
		if (cur->m_Next == cur)	   // only one reference
		{
			m_FirstReference = nullptr;
		}
		else
		{
			cur->m_Previous->m_Next = cur->m_Next;
			cur->m_Next->m_Previous = cur->m_Previous;
			if (m_FirstReference == cur)
				m_FirstReference = cur->m_Next;
		}
		ref.m_ResultValue = nullptr;
		return true;
	}
	else
		return false;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ResultValue);

ReferenceValue::ReferenceValue(Operation& operation, ResultValue& resultValue)
	: Value(operation)
{
	resultValue.AddReference(*this);
}

ReferenceValue::~ReferenceValue()
{
	SGE_CHECK(ReferenceNotFoundError, m_ResultValue->RemoveReference(*this));
}

const SpaceGameEngine::CommonIntermediateRepresentation::ResultValue* ReferenceValue::GetResultValue() const
{
	return m_ResultValue;
}

const SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue* ReferenceValue::GetPrevious() const
{
	return m_Previous;
}

const SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue* ReferenceValue::GetNext() const
{
	return m_Next;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue);

COMMON_INTERMEDIATE_REPRESENTATION_API bool ReferenceNotFoundError::Judge(bool found)
{
	return !found;
}

TypeValue::TypeValue(Operation& operation)
	: Value(operation), m_Type(nullptr)
{
}

TypeValue::~TypeValue()
{
}

TypeValue::TypeValue(Operation& operation, const Type* type)
	: Value(operation), m_Type(type)
{
}

void TypeValue::SetType(const Type* type)
{
	m_Type = type;
}

const SpaceGameEngine::CommonIntermediateRepresentation::Type* TypeValue::GetType() const
{
	return m_Type;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::TypeValue);

OperationTypeValue::OperationTypeValue(Operation& operation)
	: Value(operation), m_OperationType(nullptr)
{
}

OperationTypeValue::~OperationTypeValue()
{
}

OperationTypeValue::OperationTypeValue(Operation& operation, const OperationType* operation_type)
	: Value(operation), m_OperationType(operation_type)
{
}

void OperationTypeValue::SetOperationType(const OperationType* operation_type)
{
	m_OperationType = operation_type;
}

const SpaceGameEngine::CommonIntermediateRepresentation::OperationType* OperationTypeValue::GetOperationType() const
{
	return m_OperationType;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationTypeValue);

BlockValue::BlockValue(Operation& operation)
	: Value(operation)
{
}

BlockValue::~BlockValue()
{
}

BlockValue::BlockValue(Operation& operation, List<Operation>&& operations)
	: Value(operation), m_Operations(std::move(operations))
{
}

List<Operation>& BlockValue::GetOperations()
{
	return m_Operations;
}

const List<Operation>& BlockValue::GetOperations() const
{
	return m_Operations;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BlockValue);

IntegerValue::IntegerValue(Operation& operation)
	: Value(operation), m_Value(0)
{
}

IntegerValue::~IntegerValue()
{
}

IntegerValue::IntegerValue(Operation& operation, UInt64 value)
	: Value(operation), m_Value(value)
{
}

void IntegerValue::SetValue(UInt64 value)
{
	m_Value = value;
}

UInt64 IntegerValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::IntegerValue);

FloatValue::FloatValue(Operation& operation)
	: Value(operation), m_Value(0.0f)
{
}

FloatValue::~FloatValue()
{
}

FloatValue::FloatValue(Operation& operation, float value)
	: Value(operation), m_Value(value)
{
}

void FloatValue::SetValue(float value)
{
	m_Value = value;
}

float FloatValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::FloatValue);

DoubleValue::DoubleValue(Operation& operation)
	: Value(operation), m_Value(0.0)
{
}

DoubleValue::~DoubleValue()
{
}

DoubleValue::DoubleValue(Operation& operation, double value)
	: Value(operation), m_Value(value)
{
}

void DoubleValue::SetValue(double value)
{
	m_Value = value;
}

double DoubleValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DoubleValue);

BooleanValue::BooleanValue(Operation& operation)
	: Value(operation), m_Value(false)
{
}

BooleanValue::~BooleanValue()
{
}

BooleanValue::BooleanValue(Operation& operation, bool value)
	: Value(operation), m_Value(value)
{
}

void BooleanValue::SetValue(bool value)
{
	m_Value = value;
}

bool BooleanValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BooleanValue);

StringValue::StringValue(Operation& operation)
	: Value(operation)
{
}

StringValue::~StringValue()
{
}

StringValue::StringValue(Operation& operation, const String& value)
	: Value(operation), m_Value(value)
{
}

StringValue::StringValue(Operation& operation, String&& value)
	: Value(operation), m_Value(std::move(value))
{
}

void StringValue::SetValue(const String& value)
{
	m_Value = value;
}

void StringValue::SetValue(String&& value)
{
	m_Value = std::move(value);
}

const String& StringValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::StringValue);

ListValue::ListValue(Operation& operation)
	: Value(operation)
{
}

ListValue::~ListValue()
{
	for (auto iter = m_Content.GetBegin(); iter != m_Content.GetEnd(); ++iter)
	{
		DefaultAllocator::Delete(const_cast<Value*>(*iter));
	}
}

bool ListValue::RemoveValue(const Value& value)
{
	auto iter = m_Content.Find(&value);
	if (iter != m_Content.GetEnd())
	{
		m_Content.Remove(iter);
		return true;
	}
	return false;
}

const Vector<const Value*>& ListValue::GetValues() const
{
	return m_Content;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ListValue);