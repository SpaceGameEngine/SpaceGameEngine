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

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::OperationTypeInformation(const Vector<UInt8>& argument_storage_type_masks, const String& name, SizeType variable_registers_size, SizeType additional_registers_size, OperationJudgementFunctionType judgement_function)
	: m_ArgumentStorageTypeMasks(argument_storage_type_masks), m_Name(name), m_VariableRegistersSize(variable_registers_size), m_AdditionalRegistersSize(additional_registers_size), m_JudgementFunction(judgement_function)
{
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::operator==(const OperationTypeInformation& ot_info) const
{
	return m_ArgumentStorageTypeMasks == ot_info.m_ArgumentStorageTypeMasks && m_Name == ot_info.m_Name && m_VariableRegistersSize == ot_info.m_VariableRegistersSize && m_AdditionalRegistersSize == ot_info.m_AdditionalRegistersSize && m_JudgementFunction == ot_info.m_JudgementFunction;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeInformation::operator!=(const OperationTypeInformation& ot_info) const
{
	return m_ArgumentStorageTypeMasks != ot_info.m_ArgumentStorageTypeMasks || m_Name != ot_info.m_Name || m_VariableRegistersSize != ot_info.m_VariableRegistersSize || m_AdditionalRegistersSize != ot_info.m_AdditionalRegistersSize || m_JudgementFunction != ot_info.m_JudgementFunction;
}

#define OPERATION_TYPE(type, argument_storage_type_masks, variable_registers_size, additional_registers_size, judgement_function) Pair<const OperationType, OperationTypeInformation>(OperationType::type, OperationTypeInformation(Vector<UInt8>(argument_storage_type_masks), SGE_STR(#type), variable_registers_size, additional_registers_size, judgement_function))
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
			  Set, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant), 1, 2,
			  JUDGEMENT {
				  return operation_arguments[1].GetIndex() >= operation_arguments[0].GetType().GetContent().GetSize();
			  }),
		  OPERATION_TYPE(NewLocal, ARGUMENTS(StorageTypeMasks::Local), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(DeleteLocal, ARGUMENTS(StorageTypeMasks::Local), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Push, ARGUMENTS(StorageTypeMasks::Value), 1, 1, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Pop, ARGUMENTS(StorageTypeMasks::Variable), 1, 1, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  Copy, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Variable), 2, 0,
			  JUDGEMENT {
				  return operation_arguments[0] == operation_arguments[1];
			  }),
		  OPERATION_TYPE(Goto, ARGUMENTS(StorageTypeMasks::Constant), 0, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(If, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Call, ARGUMENTS(StorageTypeMasks::Constant), 0, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(CallFunctionPointer, ARGUMENTS(StorageTypeMasks::Variable), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(Return, ARGUMENTS(StorageTypeMasks::Value), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  ExternalCallArgument, ARGUMENTS(StorageTypeMasks::Constant, StorageTypeMasks::Value), 1, 0,
			  JUDGEMENT {
				  if (operation_arguments[0].GetIndex() >= ArgumentRegistersSize)
					  return true;
				  return operation_arguments[1].GetType().GetSize() > sizeof(RegisterType);
			  }),
		  OPERATION_TYPE(ExternalCall, ARGUMENTS(StorageTypeMasks::Constant, StorageTypeMasks::Constant), 0, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(GetReturnValue, ARGUMENTS(StorageTypeMasks::Variable), 1, 0, EMPTY_JUDGEMENT),
		  OPERATION_TYPE(
			  MakeReference, ARGUMENTS(StorageTypeMasks::Reference, StorageTypeMasks::Variable), 2, 0,
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[1].GetType(), operation_arguments[0].GetType());
			  }),
		  OPERATION_TYPE(
			  GetAddress, ARGUMENTS(StorageTypeMasks::Variable, StorageTypeMasks::Variable), 2, 0,
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[0].GetType(), BaseTypes::GetUInt64Type());
			  }),
		  OPERATION_TYPE(
			  GetReference, ARGUMENTS(StorageTypeMasks::Reference, StorageTypeMasks::Variable), 2, 0,
			  JUDGEMENT {
				  return !CanConvert(operation_arguments[1].GetType(), BaseTypes::GetUInt64Type());
			  }),
		  OPERATION_TYPE(ReleaseReference, ARGUMENTS(StorageTypeMasks::Reference), 1, 0, EMPTY_JUDGEMENT),
		  // todo
	  })
{
}

const OperationTypeInformation& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::Get(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second;
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
	const OperationTypeInformation& otype = OperationTypeSet::GetSingleton().Get(o.GetType());
	const Vector<UInt8>& args = otype.m_ArgumentStorageTypeMasks;
	if (o.GetArguments().GetSize() != args.GetSize())
		return true;
	auto oaiter = o.GetArguments().GetConstBegin();
	for (auto aiter = args.GetConstBegin(); aiter != args.GetConstEnd(); ++aiter, ++oaiter)
	{
		if (!((UInt8)(oaiter->GetStorageType()) & (*aiter)))
			return true;
	}
	return otype.m_JudgementFunction(o.GetArguments());
}
