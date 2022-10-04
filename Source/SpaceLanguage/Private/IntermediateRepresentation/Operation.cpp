﻿/*
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
#include "IntermediateRepresentation/Operation.h"
#include "VirtualMachine/Register.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidOperationTypeError::Judge(OperationType ot)
{
	return (UInt8)ot >= OperationTypeSetSize;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::IsTerminatorOperationType(OperationType ot)
{
	return ot == OperationType::Goto || ot == OperationType::If || ot == OperationType::Return;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::OperationTypeInformation(const Vector<UInt8>& argument_storage_type_masks, const String& name, OperationJudgementFunctionType judgement_function)
	: m_ArgumentStorageTypeMasks(argument_storage_type_masks), m_Name(name), m_JudgementFunction(judgement_function)
{
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::operator==(const OperationTypeInformation& ot_info) const
{
	return m_ArgumentStorageTypeMasks == ot_info.m_ArgumentStorageTypeMasks && m_Name == ot_info.m_Name && m_JudgementFunction == ot_info.m_JudgementFunction;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::operator!=(const OperationTypeInformation& ot_info) const
{
	return m_ArgumentStorageTypeMasks != ot_info.m_ArgumentStorageTypeMasks || m_Name != ot_info.m_Name || m_JudgementFunction != ot_info.m_JudgementFunction;
}

#define OPERATION_TYPE(type, argument_storage_type_masks, judgement_function) Pair<const OperationType, OperationTypeInformation>(OperationType::type, OperationTypeInformation(Vector<UInt8>(argument_storage_type_masks), SGE_STR(#type), judgement_function))
#define ARGUMENTS(...) \
	{                  \
		__VA_ARGS__    \
	}
#define JUDGEMENT [](const Vector<Variable>& operation_arguments) -> bool
#define EMPTY_JUDGEMENT \
	JUDGEMENT           \
	{                   \
		return false;   \
	}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::OperationTypeSet()
	: m_Content({
		  OPERATION_TYPE(
			  Set, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant),
			  JUDGEMENT {
				  return operation_arguments[1].GetIndex() >= operation_arguments[0].GetType().GetContent().GetSize();
			  }),
		  OPERATION_TYPE(NewLocal, ARGUMENTS(StorageTypeMasks::Local), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(DeleteLocal, ARGUMENTS(StorageTypeMasks::Local), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Push, ARGUMENTS(StorageTypeMasks::Value), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Pop, ARGUMENTS(StorageTypeMasks::Variable), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  Copy, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Variable),
			  JUDGEMENT {
				  return operation_arguments[0] == operation_arguments[1];
			  }),
		  OPERATION_TYPE(Goto, ARGUMENTS(StorageTypeMasks::Constant), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(If, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Call, ARGUMENTS(StorageTypeMasks::Constant), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(CallFunctionPointer, ARGUMENTS(StorageTypeMasks::Variable), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Return, ARGUMENTS(StorageTypeMasks::Value), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  ExternalCallArgument, ARGUMENTS(StorageTypeMasks::Constant, StorageTypeMasks::Value),
			  JUDGEMENT {
				  if (operation_arguments[0].GetIndex() >= ArgumentRegistersSize)
					  return true;
				  return operation_arguments[1].GetType().GetSize() > sizeof(RegisterType);
			  }),
		  OPERATION_TYPE(ExternalCall, ARGUMENTS(StorageTypeMasks::Constant, StorageTypeMasks::Constant), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(GetReturnValue, ARGUMENTS(StorageTypeMasks::Variable), EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  MakeReference, ARGUMENTS(StorageTypeMasks::Reference, StorageTypeMasks::Variable),
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[1].GetType(), operation_arguments[0].GetType());
			  }),
		  OPERATION_TYPE(
			  GetAddress, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Variable),
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[0].GetType(), BaseTypes::GetUInt64Type());
			  }),
		  OPERATION_TYPE(
			  GetReference, ARGUMENTS(StorageTypeMasks::Reference, StorageTypeMasks::Variable),
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[1].GetType(), BaseTypes::GetUInt64Type());
			  }),
		  OPERATION_TYPE(ReleaseReference, ARGUMENTS(StorageTypeMasks::Reference), EMPTY_JUDGEMENT),
		  //todo
	  })
{
}

const Vector<UInt8>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetArguments(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_ArgumentStorageTypeMasks;
}

const String& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetName(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_Name;
}

OperationJudgementFunctionType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetJudgementFunction(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_JudgementFunction;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Operation::Operation(OperationType type, const Vector<Variable>& arguments)
	: m_Type(type), m_Arguments(arguments)
{
	SGE_ASSERT(InvalidOperationError, *this);
}

OperationType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Operation::GetType() const
{
	return m_Type;
}

const Vector<Variable>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Operation::GetArguments() const
{
	return m_Arguments;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Operation::operator==(const Operation& o) const
{
	return m_Type == o.m_Type && m_Arguments == o.m_Arguments;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Operation::operator!=(const Operation& o) const
{
	return m_Type != o.m_Type || m_Arguments != o.m_Arguments;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidOperationError::Judge(const Operation& o)
{
	if ((UInt8)o.GetType() >= OperationTypeSetSize)
		return true;
	const Vector<UInt8>& args = OperationTypeSet::GetSingleton().GetArguments(o.GetType());
	if (o.GetArguments().GetSize() != args.GetSize())
		return true;
	auto oaiter = o.GetArguments().GetConstBegin();
	for (auto aiter = args.GetConstBegin(); aiter != args.GetConstEnd(); ++aiter, ++oaiter)
	{
		if (!((UInt8)(oaiter->GetStorageType()) & (*aiter)))
			return true;
	}
	return OperationTypeSet::GetSingleton().GetJudgementFunction(o.GetType())(o.GetArguments());
}
