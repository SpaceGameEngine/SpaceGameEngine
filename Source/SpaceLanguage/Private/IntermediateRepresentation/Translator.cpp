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
#include "IntermediateRepresentation/Translator.h"
#include "VirtualMachine/Instruction.h"

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

const IntermediateRepresentation::Function& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewInternalFunction(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations)
{
	SGE_ASSERT(FunctionAlreadyExistError, m_Functions.Find(idx), m_Functions.GetConstEnd());
	return *(m_Functions.Insert(idx, DefaultAllocator::New<IntermediateRepresentation::Function>(parameter_types, result_type, idx, operations)).m_First->m_Second);
}

const IntermediateRepresentation::Function& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateUnit::NewExternalFunction(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx)
{
	SGE_ASSERT(FunctionAlreadyExistError, m_Functions.Find(idx), m_Functions.GetConstEnd());
	return *(m_Functions.Insert(idx, DefaultAllocator::New<IntermediateRepresentation::Function>(parameter_types, result_type, idx)).m_First->m_Second);
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
	// todo
	for (auto fiter = tu.m_Functions.GetConstBegin(); fiter != tu.m_Functions.GetConstEnd(); ++fiter)
	{
		HashMap<UInt64, Type> local_map;
		HashMap<UInt64, bool> label_requests;
		HashMap<UInt64, Type> reference_map;
		for (auto oiter = fiter->m_Second->GetOperations().GetConstBegin(); oiter != fiter->m_Second->GetOperations().GetConstEnd(); ++oiter)
		{
			const auto& operation_arguments = oiter->GetArguments();
			if (oiter->GetType() == OperationType::NewLocal)
			{
				if (local_map.Find(operation_arguments[0].GetIndex()) != local_map.GetConstEnd())
					return false;
				local_map.Insert(operation_arguments[0].GetIndex(), operation_arguments[0].GetType());
			}
			else if (oiter->GetType() == OperationType::DeleteLocal)
			{
				auto lviter = local_map.Find(operation_arguments[0].GetIndex());
				if ((lviter == local_map.GetConstEnd()) || (operation_arguments[0].GetType() != lviter->m_Second))
					return false;
				local_map.Remove(lviter);
			}
			else if (oiter->GetType() == OperationType::MakeReference)
			{
				if (reference_map.Find(operation_arguments[0].GetIndex()) != reference_map.GetConstEnd())
					return false;
				if (operation_arguments[1].GetStorageType() == StorageType::Reference)
				{
					if (reference_map.Find(operation_arguments[1].GetIndex()) == reference_map.GetConstEnd())
						return false;
				}
				reference_map.Insert(operation_arguments[0].GetIndex(), operation_arguments[0].GetType());
			}
			else if (oiter->GetType() == OperationType::GetReference)
			{
				if (reference_map.Find(operation_arguments[0].GetIndex()) != reference_map.GetConstEnd())
					return false;
				if (operation_arguments[1].GetStorageType() == StorageType::Reference)
				{
					if (reference_map.Find(operation_arguments[1].GetIndex()) == reference_map.GetConstEnd())
						return false;
				}
				reference_map.Insert(operation_arguments[0].GetIndex(), operation_arguments[0].GetType());
			}
			else if (oiter->GetType() == OperationType::ReleaseReference)
			{
				auto riter = reference_map.Find(operation_arguments[0].GetIndex());
				if ((riter == reference_map.GetConstEnd()) || (operation_arguments[0].GetType() != riter->m_Second))
					return false;
				reference_map.Remove(riter);
			}
			else
			{
				for (auto aiter = operation_arguments.GetConstBegin(); aiter != operation_arguments.GetConstEnd(); ++aiter)
				{
					if (aiter->GetStorageType() == StorageType::Global)
					{
						auto gviter = tu.m_GlobalVariables.Find(aiter->GetIndex());
						if ((gviter == tu.m_GlobalVariables.GetConstEnd()) || (!CanConvert(gviter->m_Second->GetType(), aiter->GetType())))
							return false;
					}
					else if (aiter->GetStorageType() == StorageType::Local)
					{
						auto lviter = local_map.Find(aiter->GetIndex());
						if ((lviter == local_map.GetConstEnd()) || (!CanConvert(lviter->m_Second, aiter->GetType())))
							return false;
					}
					else if (aiter->GetStorageType() == StorageType::Reference)
					{
						auto refiter = reference_map.Find(aiter->GetIndex());
						if ((refiter == reference_map.GetConstEnd()) || (!CanConvert(refiter->m_Second, aiter->GetType())))
							return false;
					}
				}
			}

			switch (oiter->GetType())
			{
			case OperationType::Call: {
				if (tu.m_Functions.Find(operation_arguments[0].GetIndex()) == tu.m_Functions.GetConstEnd())
					return false;
				break;
			}
				// todo
			default: {
				break;
			}
			}
		}
		if (local_map.GetSize())
			return false;
		if (reference_map.GetSize())
			return false;
	}

	return true;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidTranslateUnitError::Judge(const TranslateUnit& tu)
{
	return !IsValidTranslateUnit(tu);
}

SpaceGameEngine::SpaceLanguage::CompiledObject SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::TranslateToCompiledObject(const TranslateUnit& tu)
{
	SGE_ASSERT(InvalidTranslateUnitError, tu);

	CompiledObject cobj;
	// todo
	return cobj;
}
