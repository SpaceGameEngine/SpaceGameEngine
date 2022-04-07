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
#include "VirtualMachine/ExternalCaller.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

void SpaceGameEngine::SpaceLanguage::ExternalCaller::AddExternalCallFunction(UInt64 index, ExternalCallFunctionType pfunc)
{
	SGE_ASSERT(NullPointerError, pfunc);
	SGE_ASSERT(ExternalCallFunctionAlreadyExistError, m_Functions.Find(index), m_Functions);
	m_Functions.Insert(index, pfunc);
}

ExternalCallFunctionType SpaceGameEngine::SpaceLanguage::ExternalCaller::GetExternalCallFunction(UInt64 index) const
{
	auto iter = m_Functions.Find(index);
	SGE_ASSERT(ExternalCallFunctionNotFoundError, iter, m_Functions);
	return iter->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCaller::IsHasExternalCallFunction(UInt64 index) const
{
	return m_Functions.Find(index) != m_Functions.GetConstEnd();
}

void SpaceGameEngine::SpaceLanguage::ExternalCaller::AddExternalCallFunction(UInt32 module_index, UInt32 func_index, ExternalCallFunctionType pfunc)
{
	SGE_ASSERT(NullPointerError, pfunc);
	UInt64 index = GetIndex(module_index, func_index);
	SGE_ASSERT(ExternalCallFunctionAlreadyExistError, m_Functions.Find(index), m_Functions);
	m_Functions.Insert(index, pfunc);
}

ExternalCallFunctionType SpaceGameEngine::SpaceLanguage::ExternalCaller::GetExternalCallFunction(UInt32 module_index, UInt32 func_index) const
{
	auto iter = m_Functions.Find(GetIndex(module_index, func_index));
	SGE_ASSERT(ExternalCallFunctionNotFoundError, iter, m_Functions);
	return iter->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCaller::IsHasExternalCallFunction(UInt32 module_index, UInt32 func_index) const
{
	return m_Functions.Find(GetIndex(module_index, func_index)) != m_Functions.GetConstEnd();
}

UInt64 SpaceGameEngine::SpaceLanguage::ExternalCaller::GetIndex(UInt32 module_index, UInt32 func_index)
{
	return ((UInt64)(module_index) << 32) | func_index;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCallFunctionAlreadyExistError::Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map)
{
	return iter != hash_map.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::ExternalCallFunctionNotFoundError::Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map)
{
	return iter == hash_map.GetConstEnd();
}
