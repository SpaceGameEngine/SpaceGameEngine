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
#include "IntermediateRepresentation/Variable.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidStorageTypeError::Judge(StorageType st)
{
	return st != StorageType::Constant && st != StorageType::Global && st != StorageType::Local && st != StorageType::Reference && st != StorageType::Function;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::Variable(const Type& type, StorageType st, UInt64 idx)
	: m_pType(&type), m_StorageType(st), m_Index(idx)
{
	SGE_ASSERT(InvalidStorageTypeError, st);
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetType() const
{
	return *m_pType;
}

StorageType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetStorageType() const
{
	return m_StorageType;
}

UInt64 SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetIndex() const
{
	return m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator==(const Variable& v) const
{
	return *m_pType == *(v.m_pType) && m_StorageType == v.m_StorageType && m_Index == v.m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator!=(const Variable& v) const
{
	return *m_pType != *(v.m_pType) || m_StorageType != v.m_StorageType || m_Index != v.m_Index;
}