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

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type()
	: m_Content(1, BaseType::Void), m_Size(0)
{
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(BaseType bt)
	: m_Content(1, bt), m_Size(BaseTypeSet::GetSingleton().GetSize(bt))
{
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Type::Type(std::initializer_list<BaseType> bts)
	: m_Content(bts), m_Size(0)
{
	for (auto i : bts)
		m_Size += BaseTypeSet::GetSingleton().GetSize(i);
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
	: m_Type(type), m_StorageType(st), m_Index(idx)
{
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetType() const
{
	return m_Type;
}

StorageType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetStorageType() const
{
	return m_StorageType;
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::GetIndex() const
{
	return m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator==(const Variable& v) const
{
	return m_Type == v.m_Type && m_StorageType == v.m_StorageType && m_Index == v.m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Variable::operator!=(const Variable& v) const
{
	return m_Type != v.m_Type || m_StorageType != v.m_StorageType || m_Index != v.m_Index;
}

SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::Function(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx)
	: m_ParameterTypes(parameter_types), m_ResultType(result_type), m_Index(idx)
{
	for (auto iter = m_ParameterTypes.GetConstBegin(); iter != m_ParameterTypes.GetConstEnd(); ++iter)
		SGE_ASSERT(NullPointerError, *iter);
}

const SpaceGameEngine::Vector<const Type*>& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetParameterTypes() const
{
	return m_ParameterTypes;
}

const Type& SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetResultType() const
{
	return m_ResultType;
}

SizeType SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::GetIndex() const
{
	return m_Index;
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
	return m_ParameterTypes == func.m_ParameterTypes && m_ResultType == func.m_ResultType && m_Index == func.m_Index;
}

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::Function::operator!=(const IntermediateRepresentation::Function& func) const
{
	return m_ParameterTypes != func.m_ParameterTypes || m_ResultType != func.m_ResultType || m_Index != func.m_Index;
}
