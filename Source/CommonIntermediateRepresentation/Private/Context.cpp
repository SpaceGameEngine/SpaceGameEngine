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
#include "Context.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

bool DialectAlreadyExistsError::Judge(bool found)
{
	return found;
}

bool DialectNotFoundError::Judge(bool found)
{
	return !found;
}

bool DialectNameAlreadyExistsError::Judge(bool found)
{
	return found;
}

bool DialectNameNotFoundError::Judge(bool found)
{
	return !found;
}

Context::~Context()
{
	for (auto iter = m_Dialects.GetBegin(); iter != m_Dialects.GetEnd(); ++iter)
	{
		DefaultAllocator::Delete(iter->m_Second);
	}
}

bool Context::HasDialectByName(const String& name) const
{
	return m_DialectsByName.Contains(name);
}

SpaceGameEngine::CommonIntermediateRepresentation::Dialect& Context::GetDialectByName(const String& name)
{
	auto iter = m_DialectsByName.Find(name);
	SGE_CHECK(DialectNameNotFoundError, iter != m_DialectsByName.GetEnd());
	return *iter->m_Second;
}

const SpaceGameEngine::CommonIntermediateRepresentation::Dialect& Context::GetDialectByName(const String& name) const
{
	auto iter = m_DialectsByName.Find(name);
	SGE_CHECK(DialectNameNotFoundError, iter != m_DialectsByName.GetConstEnd());
	return *iter->m_Second;
}

SpaceGameEngine::CommonIntermediateRepresentation::Dialect* Context::QueryDialectByName(const String& name)
{
	auto iter = m_DialectsByName.Find(name);
	if (iter != m_DialectsByName.GetEnd())
		return iter->m_Second;
	else
		return nullptr;
}

const SpaceGameEngine::CommonIntermediateRepresentation::Dialect* Context::QueryDialectByName(const String& name) const
{
	auto iter = m_DialectsByName.Find(name);
	if (iter != m_DialectsByName.GetConstEnd())
		return iter->m_Second;
	else
		return nullptr;
}
