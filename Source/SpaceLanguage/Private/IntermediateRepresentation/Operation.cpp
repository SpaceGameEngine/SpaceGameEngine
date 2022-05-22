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

using Float = float;
using Double = double;

#define BASE_TYPE(type) Pair<const BaseType, Pair<SizeType, String>>(BaseType::type, Pair<SizeType, String>(sizeof(type), SGE_STR(#type)))

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::BaseTypeSet()
	: m_Content({Pair<const BaseType, Pair<SizeType, String>>(BaseType::Void, Pair<SizeType, String>(0, SGE_STR("Void"))),
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
	return m_Content.Find(bt)->m_Second.m_First;
}

const String& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::BaseTypeSet::GetName(BaseType bt) const
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
	return m_Content.Find(bt)->m_Second.m_Second;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidBaseTypeError::Judge(BaseType bt)
{
	return (UInt8)bt >= BaseTypeSize;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(BaseType bt)
	: m_Content(1, bt), m_Size(BaseTypeSet::GetSingleton().GetSize(bt))
{
	SGE_ASSERT(InvalidBaseTypeError, bt);
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(std::initializer_list<BaseType> bts)
	: m_Content(bts), m_Size(0)
{
	for (auto i : bts)
	{
		SGE_ASSERT(InvalidBaseTypeError, i);
		m_Size += BaseTypeSet::GetSingleton().GetSize(i);
	}
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(const Vector<BaseType>& bts)
	: m_Content(bts), m_Size(0)
{
	for (auto iter = bts.GetConstBegin(); iter != bts.GetConstEnd(); ++iter)
	{
		SGE_ASSERT(InvalidBaseTypeError, *iter);
		m_Size += BaseTypeSet::GetSingleton().GetSize(*iter);
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

Type SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator+(const Type& t) const
{
	Type re(*this);
	for (auto iter = t.m_Content.GetConstBegin(); iter != t.m_Content.GetConstEnd(); ++iter)
		re.m_Content.EmplaceBack(*iter);
	re.m_Size += t.m_Size;
	return re;
}

Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator+=(const Type& t)
{
	for (auto iter = t.m_Content.GetConstBegin(); iter != t.m_Content.GetConstEnd(); ++iter)
		m_Content.EmplaceBack(*iter);
	m_Size += t.m_Size;
	return *this;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator==(const Type& t) const
{
	return m_Content == t.m_Content && m_Size == t.m_Size;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::operator!=(const Type& t) const
{
	return m_Content != t.m_Content || m_Size != t.m_Size;
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

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::Variable(const Type& type, StorageType st, SizeType idx)
	: m_pType(&type), m_StorageType(st), m_Index(idx)
{
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

#define OPERATION_TYPE(type, arg_size) Pair<const OperationType, Pair<SizeType, String>>(OperationType::type, Pair<SizeType, String>(arg_size, SGE_STR(#type)))

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::OperationTypeSet()
	: m_Content({
		  OPERATION_TYPE(NewLocal, 1),
		  OPERATION_TYPE(DeleteLocal, 1),
		  OPERATION_TYPE(Push, 1),
		  OPERATION_TYPE(Pop, 1),
		  OPERATION_TYPE(Copy, 2),
		  //todo
	  })
{
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetArgumentsSize(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_First;
}

const String& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::OperationTypeSet::GetName(OperationType type) const
{
	SGE_ASSERT(InvalidOperationTypeError, type);
	return m_Content.Find(type)->m_Second.m_Second;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidOperationTypeError::Judge(OperationType ot)
{
	return (UInt8)ot >= OperationTypeSetSize;
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
	return ((UInt8)o.GetType() >= OperationTypeSetSize) || (o.GetArguments().GetSize() != OperationTypeSet::GetSingleton().GetArgumentsSize(o.GetType()));
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
