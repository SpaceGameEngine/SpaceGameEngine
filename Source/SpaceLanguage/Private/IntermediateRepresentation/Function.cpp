/*
Copyright 2023 creatorlxd

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
#include "IntermediateRepresentation/Function.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::Function(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx)
	: m_ParameterTypes(parameter_types), m_pResultType(&result_type), m_Index(idx), m_IsExternal(true)
{
	for (auto iter = m_ParameterTypes.GetConstBegin(); iter != m_ParameterTypes.GetConstEnd(); ++iter)
		SGE_ASSERT(NullPointerError, *iter);
	for (auto iter = m_Operations.GetConstBegin(); iter != m_Operations.GetConstEnd(); ++iter)
		SGE_ASSERT(InvalidOperationError, *iter);
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::Function(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations)
	: m_ParameterTypes(parameter_types), m_pResultType(&result_type), m_Index(idx), m_Operations(operations), m_IsExternal(false)
{
	for (auto iter = m_ParameterTypes.GetConstBegin(); iter != m_ParameterTypes.GetConstEnd(); ++iter)
		SGE_ASSERT(NullPointerError, *iter);
	for (auto iter = m_Operations.GetConstBegin(); iter != m_Operations.GetConstEnd(); ++iter)
		SGE_ASSERT(InvalidOperationError, *iter);
}

const SpaceGameEngine::Vector<const Type*>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetParameterTypes() const
{
	return m_ParameterTypes;
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetResultType() const
{
	return *m_pResultType;
}

UInt64 SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetIndex() const
{
	return m_Index;
}

const Vector<Operation>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetOperations() const
{
	return m_Operations;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::IsExternal() const
{
	return m_IsExternal;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::ToVariable() const
{
	return Variable(BaseTypes::GetUInt64Type(), StorageType::Function, m_Index);
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator Variable() const
{
	return Variable(BaseTypes::GetUInt64Type(), StorageType::Function, m_Index);
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator==(const IntermediateRepresentation::Function& func) const
{
	return m_ParameterTypes == func.m_ParameterTypes && *m_pResultType == *(func.m_pResultType) && m_Index == func.m_Index && m_Operations == func.m_Operations && m_IsExternal == func.m_IsExternal;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator!=(const IntermediateRepresentation::Function& func) const
{
	return m_ParameterTypes != func.m_ParameterTypes || *m_pResultType != *(func.m_pResultType) || m_Index != func.m_Index || m_Operations != func.m_Operations || m_IsExternal != func.m_IsExternal;
}
