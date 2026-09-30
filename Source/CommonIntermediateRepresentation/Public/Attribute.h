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
#pragma once
#include "CommonIntermediateRepresentationAPI.h"
#include "Utility/DynamicCast.hpp"
#include "Utility/Utility.hpp"
#include "SGEString.hpp"
#include "Container/Map.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class COMMON_INTERMEDIATE_REPRESENTATION_API Attribute : public DynamicCastHelperForBase<Attribute>, UncopyableAndUnmovable
	{
	public:
		virtual ~Attribute() = default;

		using DynamicCastHelperForBase<Attribute>::IsInstance;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API IntegerAttribute : public Attribute, public DynamicCastHelperForDerived<IntegerAttribute, Attribute>
	{
	public:
		IntegerAttribute(UInt64 value);
		virtual ~IntegerAttribute();

		void SetValue(UInt64 value);
		UInt64 GetValue() const;

		using DynamicCastHelperForDerived<IntegerAttribute, Attribute>::IsInstance;

	private:
		UInt64 m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API FloatAttribute : public Attribute, public DynamicCastHelperForDerived<FloatAttribute, Attribute>
	{
	public:
		FloatAttribute(float value);
		virtual ~FloatAttribute();

		void SetValue(float value);
		float GetValue() const;

		using DynamicCastHelperForDerived<FloatAttribute, Attribute>::IsInstance;

	private:
		float m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API DoubleAttribute : public Attribute, public DynamicCastHelperForDerived<DoubleAttribute, Attribute>
	{
	public:
		DoubleAttribute(double value);
		virtual ~DoubleAttribute();

		void SetValue(double value);
		double GetValue() const;

		using DynamicCastHelperForDerived<DoubleAttribute, Attribute>::IsInstance;

	private:
		double m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API BooleanAttribute : public Attribute, public DynamicCastHelperForDerived<BooleanAttribute, Attribute>
	{
	public:
		BooleanAttribute(bool value);
		virtual ~BooleanAttribute();

		void SetValue(bool value);
		bool GetValue() const;

		using DynamicCastHelperForDerived<BooleanAttribute, Attribute>::IsInstance;

	private:
		bool m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API StringAttribute : public Attribute, public DynamicCastHelperForDerived<StringAttribute, Attribute>
	{
	public:
		StringAttribute(const String& value);
		StringAttribute(String&& value);
		virtual ~StringAttribute();

		void SetValue(const String& value);
		void SetValue(String&& value);
		const String& GetValue() const;

		using DynamicCastHelperForDerived<StringAttribute, Attribute>::IsInstance;

	private:
		String m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API DictionaryAttribute : public Attribute, public DynamicCastHelperForDerived<DictionaryAttribute, Attribute>
	{
	public:
		DictionaryAttribute() = default;
		virtual ~DictionaryAttribute();

		template<typename T, typename... Args>
			requires std::derived_from<T, Attribute>
		inline bool UpsertAttribute(const String& key, Args&&... args)
		{
			auto iter = m_Attributes.Find(key);
			if (iter != m_Attributes.GetEnd())
			{
				DefaultAllocator::Delete(iter->m_Second);
				iter->m_Second = DefaultAllocator::New<T>(std::forward<Args>(args)...);
				return true;
			}
			else
			{
				m_Attributes.Insert(key, DefaultAllocator::New<T>(std::forward<Args>(args)...));
				return false;
			}
		}

		bool RemoveAttribute(const String& key);

		Attribute* GetAttribute(const String& key);
		const Attribute* GetAttribute(const String& key) const;

		using DynamicCastHelperForDerived<DictionaryAttribute, Attribute>::IsInstance;

	private:
		Map<String, Attribute*> m_Attributes;
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Attribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::IntegerAttribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::FloatAttribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DoubleAttribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BooleanAttribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::StringAttribute);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DictionaryAttribute);
/*!
@}
*/