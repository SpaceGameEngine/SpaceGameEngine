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
#include "Dialect.h"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	struct DialectAlreadyExistsError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The dialect already exists in the context.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct DialectNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The dialect does not exist in the context.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct DialectNameAlreadyExistsError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The dialect name already exists in the context.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct DialectNameNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The dialect name does not exist in the context.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API Context : public UncopyableAndUnmovable
	{
	public:
		~Context();

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline DialectType& AddDialect()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			SGE_CHECK(DialectAlreadyExistsError, m_Dialects.Contains(type_id));
			DialectType* new_dialect = DefaultAllocator::New<DialectType>(*this);
			SGE_CHECK(DialectNameAlreadyExistsError, m_DialectsByName.Contains(new_dialect->GetName()));
			m_Dialects.Insert(type_id, new_dialect);
			m_DialectsByName.Insert(new_dialect->GetName(), new_dialect);
			return *new_dialect;
		}

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline bool HasDialect() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			return m_Dialects.Contains(type_id);
		}

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline DialectType& GetDialect()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			auto iter = m_Dialects.Find(type_id);
			SGE_CHECK(DialectNotFoundError, iter != m_Dialects.GetEnd());
			return *DynamicCast<DialectType>(*iter->m_Second);
		}

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline const DialectType& GetDialect() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			auto iter = m_Dialects.Find(type_id);
			SGE_CHECK(DialectNotFoundError, iter != m_Dialects.GetConstEnd());
			return *DynamicCast<const DialectType>(*iter->m_Second);
		}

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline DialectType* QueryDialect()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			auto iter = m_Dialects.Find(type_id);
			if (iter != m_Dialects.GetEnd())
				return DynamicCast<DialectType>(*iter->m_Second);
			else
				return nullptr;
		}

		template<typename DialectType>
			requires std::derived_from<DialectType, Dialect>
		inline const DialectType* QueryDialect() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<DialectType>();
			auto iter = m_Dialects.Find(type_id);
			if (iter != m_Dialects.GetConstEnd())
				return DynamicCast<const DialectType>(*iter->m_Second);
			else
				return nullptr;
		}

		bool HasDialectByName(const String& name) const;
		Dialect& GetDialectByName(const String& name);
		const Dialect& GetDialectByName(const String& name) const;
		Dialect* QueryDialectByName(const String& name);
		const Dialect* QueryDialectByName(const String& name) const;

	private:
		HashMap<UInt64, Dialect*> m_Dialects;		   // type id -> dialect, own dialect
		HashMap<String, Dialect*> m_DialectsByName;	   // name -> dialect
	};
}

/*!
@}
*/