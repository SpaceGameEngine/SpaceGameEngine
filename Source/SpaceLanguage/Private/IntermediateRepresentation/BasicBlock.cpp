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
#include "IntermediateRepresentation/BasicBlock.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::BasicBlock(UInt64 idx, const Vector<Operation>& operations)
	: m_Index(idx), m_Content(operations)
{
	SGE_ASSERT(InvalidBasicBlockError, *this);
	const Operation& last_operation = *m_Content.GetConstReverseBegin();
	if (last_operation.GetType() == OperationType::Goto)
		m_ToIndices.EmplaceBack(last_operation.GetArguments()[0].GetIndex());
	else if (last_operation.GetType() == OperationType::If)
	{
		m_ToIndices.EmplaceBack(last_operation.GetArguments()[1].GetIndex());
		m_ToIndices.EmplaceBack(last_operation.GetArguments()[2].GetIndex());
	}
}

UInt64 SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::GetIndex() const
{
	return m_Index;
}

const Vector<UInt64>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::GetToIndices() const
{
	return m_ToIndices;
}

const Vector<Operation>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::GetContent() const
{
	return m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::operator==(const BasicBlock& bb) const
{
	return m_Index == bb.m_Index && m_ToIndices == bb.m_ToIndices && m_Content == bb.m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::operator!=(const BasicBlock& bb) const
{
	return m_Index != bb.m_Index || m_ToIndices != bb.m_ToIndices || m_Content != bb.m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidBasicBlockError::Judge(const BasicBlock& bb)
{
	return bb.GetContent().GetSize() == 0 || !IsTerminatorOperationType(bb.GetContent().GetConstReverseBegin()->GetType());
}
