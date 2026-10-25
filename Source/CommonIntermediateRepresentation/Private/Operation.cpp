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
#include "Operation.h"
#include "Type.h"
#include "Value.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

OperationType::OperationType(const String& name, Dialect& dialect)
	: m_Name(name), m_BelongedDialect(dialect)
{
}

OperationType::~OperationType()
{
}

const SpaceGameEngine::String& OperationType::GetName() const
{
	return m_Name;
}

Dialect& OperationType::GetBelongedDialect()
{
	return m_BelongedDialect;
}

const Dialect& OperationType::GetBelongedDialect() const
{
	return m_BelongedDialect;
}

bool OperationType::VerifyArguments(const Vector<const Value*>& arguments) const
{
	return true;
}

bool OperationType::VerifyResults(const Vector<const ResultValue*>& results) const
{
	return true;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationType);

Operation::Operation(OperationType& operation_type)
	: m_OperationType(&operation_type)
{
}

Operation::~Operation()
{
	for (auto iter = m_Arguments.GetBegin(); iter != m_Arguments.GetEnd(); ++iter)
		DefaultAllocator::Delete(const_cast<Value*>(*iter));
	for (auto iter = m_Results.GetBegin(); iter != m_Results.GetEnd(); ++iter)
		DefaultAllocator::Delete(const_cast<ResultValue*>(*iter));
}

SpaceGameEngine::CommonIntermediateRepresentation::ResultValue& Operation::AddResult(Type& type)
{
	ResultValue* new_result = DefaultAllocator::New<ResultValue>(*this, type);
	m_Results.PushBack(new_result);
	return *new_result;
}

bool Operation::Verify() const
{
	return m_OperationType->VerifyArguments(m_Arguments) && m_OperationType->VerifyResults(m_Results);
}

bool Operation::operator==(const Operation& other) const
{
	// todo
	return true;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::InferReturnTypeOperationTypeInterface);