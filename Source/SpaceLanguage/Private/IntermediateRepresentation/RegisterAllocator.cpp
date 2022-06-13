/*
Copyright 2022 creatorlxd

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
#include "IntermediateRepresentation/RegisterAllocator.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidRegisterAllocationStateError::Judge(RegisterAllocationState state)
{
	return (UInt8)state > 2;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationStateUnknownError::Judge(RegisterAllocationState state)
{
	return state == RegisterAllocationState::Unknown;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationStateNotUnknownError::Judge(RegisterAllocationState state)
{
	return state != RegisterAllocationState::Unknown;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResult::RegisterAllocationResult()
	: m_State(RegisterAllocationState::Unknown), m_CommonRegisterIndex(0)
{
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResult::RegisterAllocationResult(RegisterAllocationState state, UInt64 cr_idx)
	: m_State(state), m_CommonRegisterIndex(cr_idx)
{
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResult::operator==(const RegisterAllocationResult& result) const
{
	return m_State == result.m_State && m_CommonRegisterIndex == result.m_CommonRegisterIndex;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResult::operator!=(const RegisterAllocationResult& result) const
{
	return m_State != result.m_State || m_CommonRegisterIndex != result.m_CommonRegisterIndex;
}

void SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::AddRegisterAllocationRequest(UInt64 virtual_register_id, UInt64 operation_id)
{
	Map<UInt64, bool>& map = m_Content[virtual_register_id];
	SGE_ASSERT(RegisterAllocationRequestAlreadyExistError, map.Find(operation_id), map.GetConstEnd());
	map.Insert(operation_id, true);
}

const HashMap<UInt64, Map<UInt64, bool>>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::GetRequests() const
{
	return m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequestAlreadyExistError::Judge(const Map<UInt64, bool>::ConstIterator& citer, const Map<UInt64, bool>::ConstIterator& cend)
{
	return citer != cend;
}

void SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResults::AddRegisterAllocationResult(UInt64 virtual_register_id, UInt64 operation_id, const RegisterAllocationResult& result)
{
	SGE_ASSERT(RegisterAllocationStateUnknownError, result.m_State);
	Map<UInt64, RegisterAllocationResult>& map = m_Content[virtual_register_id];
	SGE_ASSERT(RegisterAllocationResultAlreadyExistError, map.Find(operation_id), map.GetConstEnd());
	map.Insert(operation_id, result);
}

const HashMap<UInt64, Map<UInt64, RegisterAllocationResult>>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResults::GetResults() const
{
	return m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationResultAlreadyExistError::Judge(const Map<UInt64, RegisterAllocationResult>::ConstIterator& citer, const Map<UInt64, RegisterAllocationResult>::ConstIterator& cend)
{
	return citer != cend;
}
