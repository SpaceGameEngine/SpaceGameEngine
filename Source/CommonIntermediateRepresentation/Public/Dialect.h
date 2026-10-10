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
#include "SGEString.hpp"
#include "Utility/DynamicCast.hpp"
#include "Container/HashMap.hpp"
#include "Operation.h"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class Context;

	struct OperationTypeAlreadyExistsError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The operation type already exists in the dialect.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct OperationTypeNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The operation type does not exist in the dialect.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct OperationTypeNameAlreadyExistsError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The operation type name already exists in the dialect.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	struct OperationTypeNameNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The operation type name does not exist in the dialect.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API Dialect : public DynamicCastHelperForBase<Dialect>
	{
	public:
		virtual ~Dialect();

		const String& GetName() const;
		Context& GetBelongedContext();
		const Context& GetBelongedContext() const;

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline T& AddOperationType()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			SGE_CHECK(OperationTypeAlreadyExistsError, m_OperationTypes.Contains(type_id));
			T* new_operation_type = DefaultAllocator::New<T>(*this);
			SGE_CHECK(OperationTypeNameAlreadyExistsError, m_OperationTypesByName.Contains(new_operation_type->GetName()));
			m_OperationTypes.Insert(type_id, new_operation_type);
			m_OperationTypesByName.Insert(new_operation_type->GetName(), new_operation_type);
			return *new_operation_type;
		}

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline bool HasOperationType() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			return m_OperationTypes.Contains(type_id);
		}

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline T& GetOperationType()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			auto iter = m_OperationTypes.Find(type_id);
			SGE_CHECK(OperationTypeNotFoundError, iter != m_OperationTypes.GetEnd());
			return *DynamicCast<T>(*iter->m_Second);
		}

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline const T& GetOperationType() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			auto iter = m_OperationTypes.Find(type_id);
			SGE_CHECK(OperationTypeNotFoundError, iter != m_OperationTypes.GetConstEnd());
			return *DynamicCast<const T>(*iter->m_Second);
		}

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline T* QueryOperationType()
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			auto iter = m_OperationTypes.Find(type_id);
			if (iter != m_OperationTypes.GetEnd())
				return DynamicCast<T>(*iter->m_Second);
			else
				return nullptr;
		}

		template<typename T>
			requires std::derived_from<T, OperationType>
		inline const T* QueryOperationType() const
		{
			UInt64 type_id = SpaceGameEngine::GetTypeId<T>();
			auto iter = m_OperationTypes.Find(type_id);
			if (iter != m_OperationTypes.GetConstEnd())
				return DynamicCast<const T>(*iter->m_Second);
			else
				return nullptr;
		}

		bool HasOperationTypeByName(const String& name) const;
		OperationType& GetOperationTypeByName(const String& name);
		const OperationType& GetOperationTypeByName(const String& name) const;
		OperationType* QueryOperationTypeByName(const String& name);
		const OperationType* QueryOperationTypeByName(const String& name) const;

		using DynamicCastHelperForBase<Dialect>::IsInstance;

	protected:
		Dialect(const String& name, Context& context);	  // call by derived only

	private:
		String m_Name;
		Context& m_BelongedContext;
		HashMap<UInt64, OperationType*> m_OperationTypes;		   // type id -> operation type, own operation type
		HashMap<String, OperationType*> m_OperationTypesByName;	   // name -> operation type
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Dialect);

/*!
@}
*/