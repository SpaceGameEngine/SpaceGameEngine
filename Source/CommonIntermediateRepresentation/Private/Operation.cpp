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

Operation::Operation(const OperationType& operation_type, Vector<const ResultValue*>&& results, Vector<const Value*>&& arguments, DictionaryAttribute&& attributes)
	: m_OperationType(&operation_type), m_Results(std::move(results)), m_Arguments(std::move(arguments)), m_Attributes(std::move(attributes))
{
	LinkValues();
}

Operation::~Operation()
{
	if (m_OperationType)
	{
		for (auto iter = m_Arguments.GetBegin(); iter != m_Arguments.GetEnd(); ++iter)
			DefaultAllocator::Delete(const_cast<Value*>(*iter));
		for (auto iter = m_Results.GetBegin(); iter != m_Results.GetEnd(); ++iter)
			DefaultAllocator::Delete(const_cast<ResultValue*>(*iter));
	}
}

Operation::Operation(Operation&& other)
	: m_OperationType(other.m_OperationType), m_Results(std::move(other.m_Results)), m_Arguments(std::move(other.m_Arguments)), m_Attributes(std::move(other.m_Attributes))
{
	LinkValues();
	other.m_OperationType = nullptr;
}

SpaceGameEngine::CommonIntermediateRepresentation::Operation& Operation::operator=(Operation&& other)
{
	SGE_ASSERT(SelfAssignmentError, this, &other);
	m_OperationType = other.m_OperationType;
	m_Results = std::move(other.m_Results);
	m_Arguments = std::move(other.m_Arguments);
	m_Attributes = std::move(other.m_Attributes);
	LinkValues();
	other.m_OperationType = nullptr;
	return *this;
}

const SpaceGameEngine::CommonIntermediateRepresentation::OperationType& Operation::GetOperationType() const
{
	return *m_OperationType;
}

SpaceGameEngine::SizeType Operation::GetArgumentSize() const
{
	return m_Arguments.GetSize();
}

const Value& Operation::GetArgument(SizeType index) const
{
	SGE_ASSERT(InvalidValueError, index, 0, m_Arguments.GetSize() - 1);
	return *m_Arguments[index];
}

SpaceGameEngine::SizeType Operation::GetResultSize() const
{
	return m_Results.GetSize();
}

const ResultValue& Operation::GetResult(SizeType index) const
{
	SGE_ASSERT(InvalidValueError, index, 0, m_Results.GetSize() - 1);
	return *m_Results[index];
}

bool Operation::Verify() const
{
	return m_OperationType->VerifyArguments(m_Arguments) && m_OperationType->VerifyResults(m_Results);
}

const SpaceGameEngine::CommonIntermediateRepresentation::DictionaryAttribute& Operation::GetAttributes() const
{
	return m_Attributes;
}

void Operation::LinkValues()
{
	for (auto iter = m_Arguments.GetBegin(); iter != m_Arguments.GetEnd(); ++iter)
		const_cast<Value*>(*iter)->SetBelongedOperation(*this);
	for (auto iter = m_Results.GetBegin(); iter != m_Results.GetEnd(); ++iter)
		const_cast<ResultValue*>(*iter)->SetBelongedOperation(*this);
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::InferReturnTypeOperationTypeInterface);

bool OperationBuilderBuildTwiceError::Judge(bool built)
{
	return built;
}

OperationBuilder::OperationBuilder(OperationType& operation_type)
	: m_Built(false), m_OperationType(&operation_type)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::OperationBuilder& OperationBuilder::AddResult(const Type& type)
{
	ResultValue* new_result = DefaultAllocator::New<ResultValue>(type);
	m_Results.PushBack(new_result);
	return *this;
}

SpaceGameEngine::CommonIntermediateRepresentation::OperationBuilder& OperationBuilder::InferReturnValues()
{
	auto infer_result_type_interface = m_OperationType->QueryInterface<InferReturnTypeOperationTypeInterface>();
	if (infer_result_type_interface)
	{
		auto return_types = infer_result_type_interface->InferReturnTypes(m_Arguments);
		if (return_types.HasValue())
		{
			const auto& return_types_result = return_types.Get();
			for (auto iter = return_types_result.GetConstBegin(); iter != return_types_result.GetConstEnd(); ++iter)
				AddResult(**iter);
		}
	}
	return *this;
}

SpaceGameEngine::CommonIntermediateRepresentation::Operation OperationBuilder::Build()
{
	SGE_ASSERT(OperationBuilderBuildTwiceError, m_Built);
	m_Built = true;
	return Operation(*m_OperationType, std::move(m_Results), std::move(m_Arguments), std::move(m_Attributes));
}
