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
#include "VirtualMachine/Instruction.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

SpaceGameEngine::SpaceLanguage::InstructionType::InstructionType()
	: m_Index(0), m_Name(), m_Size(0), m_pFunction(nullptr)
{
}

SpaceGameEngine::SpaceLanguage::InstructionType::InstructionType(UInt8 index, const String& name, UInt8 size, InstructionFunctionType pfunc)
	: m_Index(index), m_Name(name), m_Size(size), m_pFunction(pfunc)
{
	SGE_ASSERT(InvalidValueError, index, 0, InstructionSet::sm_Size - 1);
	SGE_ASSERT(String::EmptyStringCoreError, name.GetSize());
	SGE_ASSERT(InvalidValueError, size, 1, InstructionSet::sm_MaxInstructionSize);
	SGE_ASSERT(NullPointerError, pfunc);
}

SpaceGameEngine::SpaceLanguage::InstructionSet::InstructionSet()
{
}

void SpaceGameEngine::SpaceLanguage::InstructionSet::ExternalCall(Registers& regs, void* pargs, const ExternalCaller& ext_caller)
{
}
