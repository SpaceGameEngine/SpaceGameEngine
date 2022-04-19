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
#include "VirtualMachine/Assembler.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

SpaceGameEngine::SpaceLanguage::InstructionNameSet::InstructionNameSet()
{
	for (UInt8 i = 0; i < InstructionSetSize; ++i)
	{
		const InstructionType& instr = InstructionSet::GetSingleton().Get(i);
		m_Content.Insert(instr.m_Name, &instr);
	}
}

const InstructionType& SpaceGameEngine::SpaceLanguage::InstructionNameSet::Get(const String& instr_name) const
{
	SGE_ASSERT(InvalidInstructionNameError, instr_name);
	return *(m_Content.Find(instr_name)->m_Second);
}

bool SpaceGameEngine::SpaceLanguage::InstructionNameSet::IsInstructionName(const String& str) const
{
	return m_Content.Find(str) != m_Content.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::InvalidInstructionNameError::Judge(const String& str)
{
	return !InstructionNameSet::GetSingleton().IsInstructionName(str);
}

bool SpaceGameEngine::SpaceLanguage::InvalidAssemblerSourceStringError::Judge(const String& str, const String& error_info_formatter)
{
	return false;
}

InstructionsGenerator SpaceGameEngine::SpaceLanguage::Assembler::Compile(const String& str, const String& error_info_formatter) const
{
	SGE_ASSERT(InvalidAssemblerSourceStringError, str, error_info_formatter);

	InstructionsGenerator result;

	return result;
}
