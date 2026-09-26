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
#include "Utility/TypeId.hpp"
#include "Utility/Utility.hpp"
#include "Error.h"
#include "Type.h"
#include "Operation.h"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class COMMON_INTERMEDIATE_REPRESENTATION_API Value : public DynamicCastHelperForBase<Value>, public UncopyableAndUnmovable
	{
	public:
		Value() = default;
		virtual ~Value() = default;

		using DynamicCastHelperForBase<Value>::IsInstance;
	};

	class ReferenceValue;

	class COMMON_INTERMEDIATE_REPRESENTATION_API ResultValue : public Value, public DynamicCastHelperForDerived<ResultValue, Value>
	{
	public:
		friend class ReferenceValue;

		ResultValue();
		virtual ~ResultValue();

		ResultValue(const Type* type);

		const Type* GetType() const;
		const ReferenceValue* GetFirstReference() const;

		using DynamicCastHelperForDerived<ResultValue, Value>::IsInstance;

	private:
		void AddReference(ReferenceValue& ref);
		bool RemoveReference(ReferenceValue& ref);

	private:
		const Type* m_Type;
		ReferenceValue* m_FirstReference;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API ReferenceValue : public Value, public DynamicCastHelperForDerived<ReferenceValue, Value>
	{
	public:
		friend class ResultValue;

		ReferenceValue(ResultValue& resultValue);
		virtual ~ReferenceValue();

		const ResultValue* GetResultValue() const;
		const ReferenceValue* GetPrevious() const;
		const ReferenceValue* GetNext() const;

		using DynamicCastHelperForDerived<ReferenceValue, Value>::IsInstance;

	private:
		ResultValue* m_ResultValue;
		ReferenceValue* m_Previous;
		ReferenceValue* m_Next;
	};

	struct ReferenceNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The reference was not found in result's reference list.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API TypeValue : public Value, public DynamicCastHelperForDerived<TypeValue, Value>
	{
	public:
		TypeValue();

		virtual ~TypeValue();

		TypeValue(const Type* type);

		const Type* GetType() const;

		using DynamicCastHelperForDerived<TypeValue, Value>::IsInstance;

	private:
		const Type* m_Type;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API OperationTypeValue : public Value, public DynamicCastHelperForDerived<OperationTypeValue, Value>
	{
	public:
		OperationTypeValue();

		virtual ~OperationTypeValue();

		OperationTypeValue(const OperationType* operation_type);

		const OperationType* GetOperationType() const;

		using DynamicCastHelperForDerived<OperationTypeValue, Value>::IsInstance;

	private:
		const OperationType* m_OperationType;
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Value);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ResultValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::TypeValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationTypeValue);
/*!
@}
*/