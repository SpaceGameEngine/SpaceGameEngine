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
#include "VirtualMachine/Register.h"
#include <cstring>

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

SpaceGameEngine::SpaceLanguage::Registers::Registers()
{
	memset(m_Registers, 0, sizeof(m_Registers));
}

RegisterType& SpaceGameEngine::SpaceLanguage::Registers::Get(SizeType index)
{
	SGE_ASSERT(InvalidRegisterIndexError, index);
	return m_Registers[index];
}

const RegisterType& SpaceGameEngine::SpaceLanguage::Registers::Get(SizeType index) const
{
	SGE_ASSERT(InvalidRegisterIndexError, index);
	return m_Registers[index];
}

bool SpaceGameEngine::SpaceLanguage::InvalidRegisterIndexError::Judge(SizeType reg_index)
{
	return reg_index >= Registers::sm_Size;
}
