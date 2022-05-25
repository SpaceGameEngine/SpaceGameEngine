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
#include "IntermediateRepresentation/Translator.h"
#include "VirtualMachine/Instruction.h"
#include "../Logger.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::TranslateUnit()
{
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::~TranslateUnit()
{
	for (auto iter = m_Types.GetBegin(); iter != m_Types.GetEnd(); ++iter)
		DefaultAllocator::Delete(*iter);
	for (auto iter = m_GlobalVariables.GetBegin(); iter != m_GlobalVariables.GetEnd(); ++iter)
		DefaultAllocator::Delete(iter->m_Second);
	for (auto iter = m_Functions.GetBegin(); iter != m_Functions.GetEnd(); ++iter)
		DefaultAllocator::Delete(iter->m_Second);
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewType(std::initializer_list<BaseType> bts)
{
	return *(m_Types.EmplaceBack(DefaultAllocator::New<Type>(bts)));
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewType(const Vector<BaseType>& bts)
{
	return *(m_Types.EmplaceBack(DefaultAllocator::New<Type>(bts)));
}

const Variable& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewGlobalVariable(const Type& type, SizeType idx)
{
	SGE_ASSERT(GlobalVariableAlreadyExistError, m_GlobalVariables.Find(idx), m_GlobalVariables.GetConstEnd());
	return *(m_GlobalVariables.Insert(idx, DefaultAllocator::New<Variable>(type, StorageType::Global, idx)).m_First->m_Second);
}

const IntermediateRepresentation::Function& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewFunction(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations)
{
	SGE_ASSERT(FunctionAlreadyExistError, m_Functions.Find(idx), m_Functions.GetConstEnd());
	return *(m_Functions.Insert(idx, DefaultAllocator::New<IntermediateRepresentation::Function>(parameter_types, result_type, idx, operations)).m_First->m_Second);
}

const Variable& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::GetGlobalVariable(UInt64 idx) const
{
	SGE_ASSERT(GlobalVariableNotExistError, m_GlobalVariables.Find(idx), m_GlobalVariables.GetConstEnd());
	return *(m_GlobalVariables.Find(idx)->m_Second);
}

const IntermediateRepresentation::Function& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::GetFunction(UInt64 idx) const
{
	SGE_ASSERT(FunctionNotExistError, m_Functions.Find(idx), m_Functions.GetConstEnd());
	return *(m_Functions.Find(idx)->m_Second);
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::GlobalVariableAlreadyExistError::Judge(const HashMap<UInt64, Variable*>::ConstIterator& citer, const HashMap<UInt64, Variable*>::ConstIterator& cend)
{
	return citer != cend;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::GlobalVariableNotExistError::Judge(const HashMap<UInt64, Variable*>::ConstIterator& citer, const HashMap<UInt64, Variable*>::ConstIterator& cend)
{
	return citer == cend;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::FunctionAlreadyExistError::Judge(const HashMap<UInt64, Function*>::ConstIterator& citer, const HashMap<UInt64, Function*>::ConstIterator& cend)
{
	return citer != cend;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::FunctionNotExistError::Judge(const HashMap<UInt64, Function*>::ConstIterator& citer, const HashMap<UInt64, Function*>::ConstIterator& cend)
{
	return citer == cend;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::IsValidTranslateUnit(const TranslateUnit& tu)
{
	//todo
	for (auto fiter = tu.m_Functions.GetConstBegin(); fiter != tu.m_Functions.GetConstEnd(); ++fiter)
	{
		HashMap<UInt64, Variable> local_map;
		for (auto oiter = fiter->m_Second->GetOperations().GetConstBegin(); oiter != fiter->m_Second->GetOperations().GetConstEnd(); ++oiter)
		{
			if (oiter->GetType() == OperationType::NewLocal)
			{
				if (local_map.Find(oiter->GetArguments()[0].GetIndex()) != local_map.GetConstEnd())
					return false;
				local_map.Insert(oiter->GetArguments()[0].GetIndex(), oiter->GetArguments()[0]);
			}
			else if (oiter->GetType() == OperationType::DeleteLocal)
			{
				auto lviter = local_map.Find(oiter->GetArguments()[0].GetIndex());
				if (lviter == local_map.GetConstEnd())
					return false;
				local_map.Remove(lviter);
			}
			else
			{
				for (auto aiter = oiter->GetArguments().GetConstBegin(); aiter != oiter->GetArguments().GetConstEnd(); ++aiter)
				{
					if (aiter->GetStorageType() == StorageType::Global)
					{
						auto gviter = tu.m_GlobalVariables.Find(aiter->GetIndex());
						if ((gviter == tu.m_GlobalVariables.GetConstEnd()) || (aiter->GetType().GetSize() > gviter->m_Second->GetType().GetSize()))
							return false;
					}
					else if (aiter->GetStorageType() == StorageType::Local)
					{
						auto lviter = local_map.Find(aiter->GetIndex());
						if ((lviter == local_map.GetConstEnd()) || (aiter->GetType().GetSize() > lviter->m_Second.GetType().GetSize()))
							return false;
					}
				}
			}
		}
		if (local_map.GetSize())
			return false;
	}

	return true;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidTranslateUnitError::Judge(const TranslateUnit& tu)
{
	return !IsValidTranslateUnit(tu);
}

Vector<UInt8> SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateToInstructions(const TranslateUnit& tu, UInt64 entry_function_index, bool has_entry, SizeType stack_size)
{
	SGE_ASSERT(InvalidTranslateUnitError, tu);

	Vector<UInt8> re;
	//todo
	return re;
}
