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

void SpaceGameEngine::SpaceLanguage::ExternalCaller::AddExternalCallFunction(UInt64 id, ExternalCallFunctionType pfunc)
{
	SGE_ASSERT(NullPointerError, pfunc);
	SGE_ASSERT(ExternalCallFunctionAlreadyExistError, m_Functions.Find(id), m_Functions);
	m_Functions.Insert(id, pfunc);
}

ExternalCallFunctionType SpaceGameEngine::SpaceLanguage::ExternalCaller::GetExternalCallFunction(UInt64 id) const
{
	auto iter = m_Functions.Find(id);
	SGE_ASSERT(ExternalCallFunctionNotFoundError, iter, m_Functions);
	return iter->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCaller::IsHasExternalCallFunction(UInt64 id) const
{
	return m_Functions.Find(id) != m_Functions.GetConstEnd();
}

void SpaceGameEngine::SpaceLanguage::ExternalCaller::AddExternalCallFunction(UInt32 module_id, UInt32 func_id, ExternalCallFunctionType pfunc)
{
	SGE_ASSERT(NullPointerError, pfunc);
	UInt64 id = GetIndex(module_id, func_id);
	SGE_ASSERT(ExternalCallFunctionAlreadyExistError, m_Functions.Find(id), m_Functions);
	m_Functions.Insert(id, pfunc);
}

ExternalCallFunctionType SpaceGameEngine::SpaceLanguage::ExternalCaller::GetExternalCallFunction(UInt32 module_id, UInt32 func_id) const
{
	auto iter = m_Functions.Find(GetIndex(module_id, func_id));
	SGE_ASSERT(ExternalCallFunctionNotFoundError, iter, m_Functions);
	return iter->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCaller::IsHasExternalCallFunction(UInt32 module_id, UInt32 func_id) const
{
	return m_Functions.Find(GetIndex(module_id, func_id)) != m_Functions.GetConstEnd();
}

UInt64 SpaceGameEngine::SpaceLanguage::ExternalCaller::GetIndex(UInt32 module_id, UInt32 func_id)
{
	return ((UInt64)(module_id) << 32) | func_id;
}

bool SpaceGameEngine::SpaceLanguage::ExternalCallFunctionAlreadyExistError::Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map)
{
	return iter != hash_map.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::ExternalCallFunctionNotFoundError::Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map)
{
	return iter == hash_map.GetConstEnd();
}
