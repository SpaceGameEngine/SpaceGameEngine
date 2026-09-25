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
#include "Type.h"
#include "Error.h"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class COMMON_INTERMEDIATE_REPRESENTATION_API Value
	{
	public:
		Value();
		virtual ~Value() = default;

		static bool IsInstance(const Value& value);

	public:
		UInt64 GetTypeId() const;

	protected:
		void SetTypeId(UInt64 typeId);

	private:
		UInt64 m_TypeId = 0;
	};

	class ReferenceValue;

	class COMMON_INTERMEDIATE_REPRESENTATION_API ResultValue : public Value
	{
	public:
		friend class ReferenceValue;

		ResultValue();
		virtual ~ResultValue();

		ResultValue(Type* type);

		Type* GetType() const;
		ReferenceValue* GetFirstReference() const;

		static bool IsInstance(const Value& value);

	private:
		void AddReference(ReferenceValue& ref);
		bool RemoveReference(ReferenceValue& ref);

	private:
		Type* m_Type;
		ReferenceValue* m_FirstReference;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API ReferenceValue : public Value
	{
	public:
		friend class ResultValue;

		ReferenceValue(ResultValue& resultValue);
		virtual ~ReferenceValue();

		ResultValue* GetResultValue() const;
		ReferenceValue* GetPrevious() const;
		ReferenceValue* GetNext() const;

		static bool IsInstance(const Value& value);

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
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Value);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ResultValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue);
/*!
@}
*/