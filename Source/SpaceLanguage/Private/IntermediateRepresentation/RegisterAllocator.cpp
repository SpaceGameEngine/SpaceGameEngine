/*
Copyright 2024 creatorlxd

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

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidFunctionCallTypeError::Judge(FunctionCallType fc_type)
{
	return (UInt8)fc_type > 2;
}

void SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::AddRegisterAllocationRequest(UInt64 func_id, UInt64 operation_id, UInt64 virtual_register_id)
{
	Map<UInt64, bool>& map = m_Content[func_id][virtual_register_id];
	SGE_ASSERT(RegisterAllocationRequestAlreadyExistError, map.Find(operation_id), map.GetConstEnd());
	map.Insert(operation_id, true);
}

const HashMap<UInt64, HashMap<UInt64, Map<UInt64, bool>>>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::GetRequests() const
{
	return m_Content;
}

void SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::AddFunctionCall(UInt64 func_id, UInt64 operation_id, FunctionCallType fc_type, UInt64 target_func_id)
{
	Map<UInt64, Pair<FunctionCallType, UInt64>>& map = m_FunctionCallRecords[func_id];
	SGE_ASSERT(FunctionCallRecordAlreadyExistError, map.Find(operation_id), map.GetConstEnd());
	map.Insert(operation_id, Pair<FunctionCallType, UInt64>(fc_type, target_func_id));
	if (fc_type == FunctionCallType::Internal)
	{
		HashMap<UInt64, bool>& relation_map = m_InternalFunctionCallRelationships[func_id];
		if (relation_map.Find(target_func_id) == relation_map.GetConstEnd())
			relation_map.Insert(target_func_id, true);
	}
}

const HashMap<UInt64, Map<UInt64, Pair<FunctionCallType, UInt64>>>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::GetFunctionCallRecords() const
{
	return m_FunctionCallRecords;
}

const HashMap<UInt64, HashMap<UInt64, bool>>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequests::GetInternalFunctionCallRelationships() const
{
	return m_InternalFunctionCallRelationships;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::RegisterAllocationRequestAlreadyExistError::Judge(const Map<UInt64, bool>::ConstIterator& citer, const Map<UInt64, bool>::ConstIterator& cend)
{
	return citer != cend;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::FunctionCallRecordAlreadyExistError::Judge(const Map<UInt64, Pair<FunctionCallType, UInt64>>::ConstIterator& citer, const Map<UInt64, Pair<FunctionCallType, UInt64>>::ConstIterator& cend)
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

RegisterAllocationResults SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::SimpleRegisterAllocate(const RegisterAllocationRequests& reqs)
{
	RegisterAllocationResults re;
	// todo
	return re;
}
