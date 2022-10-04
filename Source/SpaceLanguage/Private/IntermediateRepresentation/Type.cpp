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
#include "IntermediateRepresentation/Type.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation;

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::InvalidBaseTypeError::Judge(BaseType bt)
{
	return (UInt8)bt >= BaseTypeSize;
}

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

bool SpaceGameEngine::SpaceLanguage::IntermediateRepresentation::CanConvert(const Type& from, const Type& to)
{
	return (from.GetSize() == to.GetSize()) && (from.GetAlignment() % to.GetAlignment() == 0);
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