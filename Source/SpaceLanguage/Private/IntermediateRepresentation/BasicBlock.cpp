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
#include "IntermediateRepresentation/BasicBlock.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::BasicBlock(const Vector<Operation>& operations)
	: m_Content(operations)
{
	SGE_ASSERT(InvalidBasicBlockError, *this);
}

const Vector<Operation>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::GetContent() const
{
	return m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::operator==(const BasicBlock& bb) const
{
	return m_Content == bb.m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BasicBlock::operator!=(const BasicBlock& bb) const
{
	return m_Content != bb.m_Content;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidBasicBlockError::Judge(const BasicBlock& bb)
{
	return !IsTerminatorOperationType(bb.GetContent().GetConstReverseBegin()->GetType());
}
