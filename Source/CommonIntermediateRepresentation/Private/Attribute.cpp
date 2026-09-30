/*
Copyright 2026 creatorlxd

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
#include "Attribute.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Attribute);

IntegerAttribute::IntegerAttribute(UInt64 value)
	: m_Value(value)
{
}

IntegerAttribute::~IntegerAttribute()
{
}

void IntegerAttribute::SetValue(UInt64 value)
{
	m_Value = value;
}

UInt64 IntegerAttribute::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::IntegerAttribute);

FloatAttribute::FloatAttribute(float value)
	: m_Value(value)
{
}

FloatAttribute::~FloatAttribute()
{
}

void FloatAttribute::SetValue(float value)
{
	m_Value = value;
}

float FloatAttribute::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::FloatAttribute);

DoubleAttribute::DoubleAttribute(double value)
	: m_Value(value)
{
}

DoubleAttribute::~DoubleAttribute()
{
}

void DoubleAttribute::SetValue(double value)
{
	m_Value = value;
}

double DoubleAttribute::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DoubleAttribute);

BooleanAttribute::BooleanAttribute(bool value)
	: m_Value(value)
{
}

BooleanAttribute::~BooleanAttribute()
{
}

void BooleanAttribute::SetValue(bool value)
{
	m_Value = value;
}

bool BooleanAttribute::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BooleanAttribute);

StringAttribute::StringAttribute(const String& value)
	: m_Value(value)
{
}

StringAttribute::StringAttribute(String&& value)
	: m_Value(std::move(value))
{
}

StringAttribute::~StringAttribute()
{
}

void StringAttribute::SetValue(const String& value)
{
	m_Value = value;
}

void StringAttribute::SetValue(String&& value)
{
	m_Value = std::move(value);
}

const String& StringAttribute::GetValue() const
{
	return m_Value;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::StringAttribute);

DictionaryAttribute::~DictionaryAttribute()
{
	for (auto iter = m_Attributes.GetBegin(); iter != m_Attributes.GetEnd(); ++iter)
	{
		DefaultAllocator::Delete(iter->m_Second);
	}
}

bool DictionaryAttribute::RemoveAttribute(const String& key)
{
	auto iter = m_Attributes.Find(key);
	if (iter != m_Attributes.GetEnd())
	{
		DefaultAllocator::Delete(iter->m_Second);
		m_Attributes.Remove(iter);
		return true;
	}
	else
		return false;
}

SpaceGameEngine::CommonIntermediateRepresentation::Attribute* DictionaryAttribute::GetAttribute(const String& key)
{
	auto iter = m_Attributes.Find(key);
	if (iter != m_Attributes.GetEnd())
		return iter->m_Second;
	else
		return nullptr;
}

const SpaceGameEngine::CommonIntermediateRepresentation::Attribute* DictionaryAttribute::GetAttribute(const String& key) const
{
	auto iter = m_Attributes.Find(key);
	if (iter != m_Attributes.GetConstEnd())
		return iter->m_Second;
	else
		return nullptr;
}

SGE_DEFINE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DictionaryAttribute);