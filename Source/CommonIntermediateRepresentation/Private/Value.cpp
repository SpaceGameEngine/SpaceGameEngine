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

Value::Value()
	: m_BelongedOperation(nullptr)
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::Operation* Value::GetBelongedOperation() const
{
	return m_BelongedOperation;
}

void Value::SetBelongedOperation(Operation& operation)
{
	m_BelongedOperation = &operation;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Value);

ResultValue::ResultValue(const Type& type)
	: m_Type(&type), m_FirstReference(nullptr)
{
}

ResultValue::~ResultValue()
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::Type& ResultValue::GetType() const
{
	return *m_Type;
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

ReferenceValue::ReferenceValue(ResultValue& resultValue)
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

bool ReferenceNotFoundError::Judge(bool found)
{
	return !found;
}

TypeValue::TypeValue(Type& type)
	: m_Type(&type)
{
}

TypeValue::~TypeValue()
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::Type& TypeValue::GetType() const
{
	return *m_Type;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::TypeValue);

OperationTypeValue::OperationTypeValue(OperationType& operation_type)
	: m_OperationType(&operation_type)
{
}

OperationTypeValue::~OperationTypeValue()
{
}

const SpaceGameEngine::CommonIntermediateRepresentation::OperationType& OperationTypeValue::GetOperationType() const
{
	return *m_OperationType;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationTypeValue);

BlockValue::BlockValue(List<Operation>&& operations)
	: m_Operations(std::move(operations))
{
}

BlockValue::~BlockValue()
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

IntegerValue::IntegerValue(UInt64 value)
	: m_Value(value)
{
}

IntegerValue::~IntegerValue()
{
}

UInt64 IntegerValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::IntegerValue);

FloatValue::FloatValue(float value)
	: m_Value(value)
{
}

FloatValue::~FloatValue()
{
}

float FloatValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::FloatValue);

DoubleValue::DoubleValue(double value)
	: m_Value(value)
{
}

DoubleValue::~DoubleValue()
{
}

double DoubleValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DoubleValue);

BooleanValue::BooleanValue(bool value)
	: m_Value(value)
{
}

BooleanValue::~BooleanValue()
{
}

bool BooleanValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BooleanValue);

StringValue::StringValue(const String& value)
	: m_Value(value)
{
}

StringValue::StringValue(String&& value)
	: m_Value(std::move(value))
{
}
StringValue::~StringValue()
{
}

const String& StringValue::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::StringValue);

ListValue::ListValue(Vector<const Value*>&& values)
	: m_Content(std::move(values))
{
}

ListValue::~ListValue()
{
	for (auto iter = m_Content.GetBegin(); iter != m_Content.GetEnd(); ++iter)
		DefaultAllocator::Delete(const_cast<Value*>(*iter));
}

const Vector<const Value*>& ListValue::GetValues() const
{
	return m_Content;
}

void ListValue::SetBelongedOperation(Operation& operation)
{
	Value::SetBelongedOperation(operation);
	for (auto iter = m_Content.GetBegin(); iter != m_Content.GetEnd(); ++iter)
		const_cast<Value*>(*iter)->SetBelongedOperation(operation);
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ListValue);