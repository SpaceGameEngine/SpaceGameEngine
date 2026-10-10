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
#include "Dialect.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

bool OperationTypeAlreadyExistsError::Judge(bool found)
{
	return found;
}

bool OperationTypeNotFoundError::Judge(bool found)
{
	return !found;
}

bool OperationTypeNameAlreadyExistsError::Judge(bool found)
{
	return found;
}

bool OperationTypeNameNotFoundError::Judge(bool found)
{
	return !found;
}

Dialect::Dialect(const String& name, Context& context)
	: m_Name(name), m_BelongedContext(context)
{
}

Dialect::~Dialect()
{
	for (auto iter = m_OperationTypes.GetBegin(); iter != m_OperationTypes.GetEnd(); ++iter)
	{
		DefaultAllocator::Delete(iter->m_Second);
	}
}

const SpaceGameEngine::String& Dialect::GetName() const
{
	return m_Name;
}

SpaceGameEngine::CommonIntermediateRepresentation::Context& Dialect::GetBelongedContext()
{
	return m_BelongedContext;
}

const SpaceGameEngine::CommonIntermediateRepresentation::Context& Dialect::GetBelongedContext() const
{
	return m_BelongedContext;
}

bool Dialect::HasOperationTypeByName(const String& name) const
{
	return m_OperationTypesByName.Contains(name);
}

SpaceGameEngine::CommonIntermediateRepresentation::OperationType& Dialect::GetOperationTypeByName(const String& name)
{
	auto iter = m_OperationTypesByName.Find(name);
	SGE_CHECK(OperationTypeNameNotFoundError, iter != m_OperationTypesByName.GetEnd());
	return *iter->m_Second;
}

const SpaceGameEngine::CommonIntermediateRepresentation::OperationType& Dialect::GetOperationTypeByName(const String& name) const
{
	auto iter = m_OperationTypesByName.Find(name);
	SGE_CHECK(OperationTypeNameNotFoundError, iter != m_OperationTypesByName.GetConstEnd());
	return *iter->m_Second;
}

SpaceGameEngine::CommonIntermediateRepresentation::OperationType* Dialect::QueryOperationTypeByName(const String& name)
{
	auto iter = m_OperationTypesByName.Find(name);
	if (iter != m_OperationTypesByName.GetEnd())
		return iter->m_Second;
	else
		return nullptr;
}

const SpaceGameEngine::CommonIntermediateRepresentation::OperationType* Dialect::QueryOperationTypeByName(const String& name) const
{
	auto iter = m_OperationTypesByName.Find(name);
	if (iter != m_OperationTypesByName.GetConstEnd())
		return iter->m_Second;
	else
		return nullptr;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Dialect);
