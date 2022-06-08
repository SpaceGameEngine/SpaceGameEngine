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

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeInformation::BaseTypeInformation(SizeType size, SizeType alignment, const String& name)
	: m_Size(size), m_Alignment(alignment), m_Name(name)
{
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeInformation::operator==(const BaseTypeInformation& bti) const
{
	return m_Size == bti.m_Size && m_Alignment == bti.m_Alignment && m_Name == bti.m_Name;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeInformation::operator!=(const BaseTypeInformation& bti) const
{
	return m_Size != bti.m_Size || m_Alignment != bti.m_Alignment || m_Name != bti.m_Name;
}

using Float = float;
using Double = double;

#define BASE_TYPE(type) Pair<const BaseType, BaseTypeInformation>(BaseType::type, BaseTypeInformation(sizeof(type), alignof(type), SGE_STR(#type)))

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::BaseTypeSet()
	: m_Content({Pair<const BaseType, BaseTypeInformation>(BaseType::Void, BaseTypeInformation(0, 0, SGE_STR("Void"))),
				 BASE_TYPE(Int8),
				 BASE_TYPE(UInt8),
				 BASE_TYPE(Int16),
				 BASE_TYPE(UInt16),
				 BASE_TYPE(Int32),
				 BASE_TYPE(UInt32),
				 BASE_TYPE(Int64),
				 BASE_TYPE(UInt64),
				 BASE_TYPE(Float),
				 BASE_TYPE(Double)})
{
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::GetSize(BaseType bt) const
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
	return m_Content.Find(bt)->m_Second.m_Size;
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::GetAlignment(BaseType bt) const
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
	return m_Content.Find(bt)->m_Second.m_Alignment;
}

const String& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::GetName(BaseType bt) const
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
	return m_Content.Find(bt)->m_Second.m_Name;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidBaseTypeError::Judge(BaseType bt)
{
	return (UInt8)bt >= BaseTypeSize;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(BaseType bt)
	: m_Content(1, bt), m_Size(BaseTypeSet::GetSingleton().GetSize(bt)), m_Alignment(BaseTypeSet::GetSingleton().GetAlignment(bt))
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(std::initializer_list<BaseType> bts)
	: m_Content(bts), m_Size(0), m_Alignment(0)
{
	for (auto i : bts)
	{
		SGE_ASSERT(InvalidBaseTypeError, i);
		m_Size += BaseTypeSet::GetSingleton().GetSize(i);
		m_Alignment = Max(m_Alignment, BaseTypeSet::GetSingleton().GetAlignment(i));
	}
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(const Vector<BaseType>& bts)
	: m_Content(bts), m_Size(0), m_Alignment(0)
{
	for (auto iter = bts.GetConstBegin(); iter != bts.GetConstEnd(); ++iter)
	{
		SGE_ASSERT(InvalidBaseTypeError, *iter);
		m_Size += BaseTypeSet::GetSingleton().GetSize(*iter);
		m_Alignment = Max(m_Alignment, BaseTypeSet::GetSingleton().GetAlignment(*iter));
	}
}

const Vector<BaseType>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::GetContent() const
{
	return m_Content;
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::GetSize() const
{
	return m_Size;
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::GetAlignment() const
{
	return m_Alignment;
}

Type SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator+(const Type& t) const
{
	Type re(*this);
	for (auto iter = t.m_Content.GetConstBegin(); iter != t.m_Content.GetConstEnd(); ++iter)
		re.m_Content.EmplaceBack(*iter);
	re.m_Size += t.m_Size;
	re.m_Alignment = Max(re.m_Alignment, t.m_Alignment);
	return re;
}

Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator+=(const Type& t)
{
	for (auto iter = t.m_Content.GetConstBegin(); iter != t.m_Content.GetConstEnd(); ++iter)
		m_Content.EmplaceBack(*iter);
	m_Size += t.m_Size;
	m_Alignment = Max(m_Alignment, t.m_Alignment);
	return *this;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator==(const Type& t) const
{
	return m_Content == t.m_Content && m_Size == t.m_Size && m_Alignment == t.m_Alignment;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator!=(const Type& t) const
{
	return m_Content != t.m_Content || m_Size != t.m_Size || m_Alignment != t.m_Alignment;
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetVoidType()
{
	static GlobalVariable<Type> g_VoidType(BaseType::Void);
	return g_VoidType.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetInt8Type()
{
	static GlobalVariable<Type> g_Int8Type(BaseType::Int8);
	return g_Int8Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetUInt8Type()
{
	static GlobalVariable<Type> g_UInt8Type(BaseType::UInt8);
	return g_UInt8Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetInt16Type()
{
	static GlobalVariable<Type> g_Int16Type(BaseType::Int16);
	return g_Int16Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetUInt16Type()
{
	static GlobalVariable<Type> g_UInt16Type(BaseType::UInt16);
	return g_UInt16Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetInt32Type()
{
	static GlobalVariable<Type> g_Int32Type(BaseType::Int32);
	return g_Int32Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetUInt32Type()
{
	static GlobalVariable<Type> g_UInt32Type(BaseType::UInt32);
	return g_UInt32Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetInt64Type()
{
	static GlobalVariable<Type> g_Int64Type(BaseType::Int64);
	return g_Int64Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetUInt64Type()
{
	static GlobalVariable<Type> g_UInt64Type(BaseType::UInt64);
	return g_UInt64Type.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetFloatType()
{
	static GlobalVariable<Type> g_FloatType(BaseType::Float);
	return g_FloatType.Get();
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypes::GetDoubleType()
{
	static GlobalVariable<Type> g_DoubleType(BaseType::Double);
	return g_DoubleType.Get();
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidStorageTypeError::Judge(StorageType st)
{
	return st != StorageType::Constant && st != StorageType::Global && st != StorageType::Local && st != StorageType::Function;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::Variable(const Type& type, StorageType st, SizeType idx)
	: m_pType(&type), m_StorageType(st), m_Index(idx)
{
	SGE_ASSERT(InvalidStorageTypeError, st);
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetType() const
{
	return *m_pType;
}

StorageType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetStorageType() const
{
	return m_StorageType;
}

UInt64 SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetIndex() const
{
	return m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator==(const Variable& v) const
{
	return *m_pType == *(v.m_pType) && m_StorageType == v.m_StorageType && m_Index == v.m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator!=(const Variable& v) const
{
	return *m_pType != *(v.m_pType) || m_StorageType != v.m_StorageType || m_Index != v.m_Index;
}

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
		  OPERATION_TYPE(Return, StorageTypeMasks::Variable),
		  OPERATION_TYPE(ExternalCallArgument, StorageTypeMasks::Constant, StorageTypeMasks::Variable),
		  OPERATION_TYPE(ExternalCall, StorageTypeMasks::Constant, StorageTypeMasks::Constant),
		  OPERATION_TYPE(GetReturnValue, StorageTypeMasks::Variable),
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

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::Function(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations)
	: m_ParameterTypes(parameter_types), m_pResultType(&result_type), m_Index(idx), m_Operations(operations)
{
	for (auto iter = m_ParameterTypes.GetConstBegin(); iter != m_ParameterTypes.GetConstEnd(); ++iter)
		SGE_ASSERT(NullPointerError, *iter);
	for (auto iter = m_Operations.GetConstBegin(); iter != m_Operations.GetConstEnd(); ++iter)
		SGE_ASSERT(InvalidOperationError, *iter);
}

const SpaceGameEngine::Vector<const Type*>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetParameterTypes() const
{
	return m_ParameterTypes;
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetResultType() const
{
	return *m_pResultType;
}

UInt64 SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetIndex() const
{
	return m_Index;
}

const Vector<Operation>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetOperations() const
{
	return m_Operations;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::ToVariable() const
{
	return Variable(BaseTypes::GetUInt64Type(), StorageType::Function, m_Index);
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator Variable() const
{
	return Variable(BaseTypes::GetUInt64Type(), StorageType::Function, m_Index);
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator==(const IntermediateRepresentation::Function& func) const
{
	return m_ParameterTypes == func.m_ParameterTypes && *m_pResultType == *(func.m_pResultType) && m_Index == func.m_Index && m_Operations == func.m_Operations;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator!=(const IntermediateRepresentation::Function& func) const
{
	return m_ParameterTypes != func.m_ParameterTypes || *m_pResultType != *(func.m_pResultType) || m_Index != func.m_Index || m_Operations != func.m_Operations;
}
