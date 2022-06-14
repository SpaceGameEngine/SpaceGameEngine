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
#include "VirtualMachine/VirtualMachine.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

void SpaceGameEngine::SpaceLanguage::VirtualMachine::Run(const void* ptr, SizeType size)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidInstructionsError, ptr, size);

	Registers registers;
	RegisterType& pc = registers.Get(Register::ProgramCounter);
	InstructionSet& instr_set = InstructionSet::GetSingleton();
	while (pc < size)
	{
		UInt8* pnow = (UInt8*)ptr + pc;
		instr_set.Get(*pnow).m_pFunction(registers, pnow + 1, m_ExternalCaller);
	}
}

ExternalCaller& SpaceGameEngine::SpaceLanguage::VirtualMachine::GetExternalCaller()
{
	return m_ExternalCaller;
}

const ExternalCaller& SpaceGameEngine::SpaceLanguage::VirtualMachine::GetExternalCaller() const
{
	return m_ExternalCaller;
}
