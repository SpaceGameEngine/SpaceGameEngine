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

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidOperationTypeError::Judge(OperationType ot)
{
	return (UInt8)ot >= OperationTypeSetSize;
}

#define OPERATION_TYPE(type, ...) Pair<const OperationType, Pair<Vector<UInt8>, String>>(OperationType::type, Pair<Vector<UInt8>, String>(Vector<UInt8>({__VA_ARGS__}), SGE_STR(#type)))

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::OperationTypeSet()
	: m_Content({
		  OPERATION_TYPE(Set, StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant),
		  OPERATION_TYPE(NewLocal, StorageTypeMasks::Local),
		  OPERATION_TYPE(DeleteLocal, StorageTypeMasks::Local),
		  OPERATION_TYPE(Push, StorageTypeMasks::Variable),
		  OPERATION_TYPE(Pop, StorageTypeMasks::Variable),
		  OPERATION_TYPE(Copy, StorageTypeMasks::Variable, StorageTypeMasks::Variable),
		  OPERATION_TYPE(Label, StorageTypeMasks::Constant),
		  OPERATION_TYPE(Goto, StorageTypeMasks::Constant),
		  OPERATION_TYPE(If, StorageTypeMasks::Variable, StorageTypeMasks::Constant),
		  OPERATION_TYPE(Call, StorageTypeMasks::Constant),
		  OPERATION_TYPE(CallFunctionPointer, StorageTypeMasks::Variable),
		  OPERATION_TYPE(Return, StorageTypeMasks::Variable),
		  OPERATION_TYPE(ExternalCallArgument, StorageTypeMasks::Constant, StorageTypeMasks::Variable),
		  OPERATION_TYPE(ExternalCall, StorageTypeMasks::Constant, StorageTypeMasks::Constant),
		  OPERATION_TYPE(GetReturnValue, StorageTypeMasks::Variable),
		  OPERATION_TYPE(MakeReference, StorageTypeMasks::Reference, StorageTypeMasks::Variable),
		  OPERATION_TYPE(GetAddress, StorageTypeMasks::Variable, StorageTypeMasks::Variable),
		  OPERATION_TYPE(GetReference, StorageTypeMasks::Reference, StorageTypeMasks::Variable),
		  OPERATION_TYPE(ReleaseReference, StorageTypeMasks::Reference),
		  //todo
	  })
{
}

const Vector<UInt8>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetArguments(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_First;
}

const String& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetName(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_Second;
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
	return false;
}
