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
#include "Container/List.hpp"
#include "Operation.h"
#include "SGEString.hpp"
#include "Container/Vector.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class COMMON_INTERMEDIATE_REPRESENTATION_API Value : public DynamicCastHelperForBase<Value>, public UncopyableAndUnmovable
	{
	public:
		Value(Operation& operation);
		virtual ~Value() = default;

		const Operation* GetBelongedOperation() const;

		using DynamicCastHelperForBase<Value>::IsInstance;

	private:
		const Operation* m_BelongedOperation;
	};

	class ReferenceValue;

	class COMMON_INTERMEDIATE_REPRESENTATION_API ResultValue : public Value, public DynamicCastHelperForDerived<ResultValue, Value>
	{
	public:
		friend class ReferenceValue;

		ResultValue(Operation& operation);
		virtual ~ResultValue();

		ResultValue(Operation& operation, const Type* type);

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

		ReferenceValue(Operation& operation, ResultValue& resultValue);
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
		TypeValue(Operation& operation);
		virtual ~TypeValue();

		TypeValue(Operation& operation, const Type* type);

		void SetType(const Type* type);
		const Type* GetType() const;

		using DynamicCastHelperForDerived<TypeValue, Value>::IsInstance;

	private:
		const Type* m_Type;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API OperationTypeValue : public Value, public DynamicCastHelperForDerived<OperationTypeValue, Value>
	{
	public:
		OperationTypeValue(Operation& operation);
		virtual ~OperationTypeValue();

		OperationTypeValue(Operation& operation, const OperationType* operation_type);

		void SetOperationType(const OperationType* operation_type);
		const OperationType* GetOperationType() const;

		using DynamicCastHelperForDerived<OperationTypeValue, Value>::IsInstance;

	private:
		const OperationType* m_OperationType;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API BlockValue : public Value, public DynamicCastHelperForDerived<BlockValue, Value>
	{
	public:
		BlockValue(Operation& operation);
		virtual ~BlockValue();

		BlockValue(Operation& operation, List<Operation>&& operations);

		List<Operation>& GetOperations();
		const List<Operation>& GetOperations() const;

		using DynamicCastHelperForDerived<BlockValue, Value>::IsInstance;

	private:
		List<Operation> m_Operations;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API IntegerValue : public Value, public DynamicCastHelperForDerived<IntegerValue, Value>
	{
	public:
		IntegerValue(Operation& operation);
		virtual ~IntegerValue();

		IntegerValue(Operation& operation, UInt64 value);

		void SetValue(UInt64 value);
		UInt64 GetValue() const;

		using DynamicCastHelperForDerived<IntegerValue, Value>::IsInstance;

	private:
		UInt64 m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API FloatValue : public Value, public DynamicCastHelperForDerived<FloatValue, Value>
	{
	public:
		FloatValue(Operation& operation);
		virtual ~FloatValue();

		FloatValue(Operation& operation, float value);

		void SetValue(float value);
		float GetValue() const;

		using DynamicCastHelperForDerived<FloatValue, Value>::IsInstance;

	private:
		float m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API DoubleValue : public Value, public DynamicCastHelperForDerived<DoubleValue, Value>
	{
	public:
		DoubleValue(Operation& operation);
		virtual ~DoubleValue();

		DoubleValue(Operation& operation, double value);

		void SetValue(double value);
		double GetValue() const;

		using DynamicCastHelperForDerived<DoubleValue, Value>::IsInstance;

	private:
		double m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API BooleanValue : public Value, public DynamicCastHelperForDerived<BooleanValue, Value>
	{
	public:
		BooleanValue(Operation& operation);
		virtual ~BooleanValue();

		BooleanValue(Operation& operation, bool value);

		void SetValue(bool value);
		bool GetValue() const;

		using DynamicCastHelperForDerived<BooleanValue, Value>::IsInstance;

	private:
		bool m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API StringValue : public Value, public DynamicCastHelperForDerived<StringValue, Value>
	{
	public:
		StringValue(Operation& operation);
		virtual ~StringValue();

		StringValue(Operation& operation, const String& value);
		StringValue(Operation& operation, String&& value);

		void SetValue(const String& value);
		void SetValue(String&& value);
		const String& GetValue() const;

		using DynamicCastHelperForDerived<StringValue, Value>::IsInstance;

	private:
		String m_Value;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API ListValue : public Value, public DynamicCastHelperForDerived<ListValue, Value>
	{
	public:
		ListValue(Operation& operation);
		virtual ~ListValue();

		template<typename T, typename... Args>
			requires std::derived_from<T, Value>
		inline Value& AddValue(Args&&... args)
		{
			T* new_value = DefaultAllocator::New<T>(std::forward<Args>(args)...);
			m_Content.PushBack(new_value);
			return *new_value;
		}

		bool RemoveValue(const Value& value);

		const Vector<const Value*>& GetValues() const;

		using DynamicCastHelperForDerived<ListValue, Value>::IsInstance;

	private:
		Vector<const Value*> m_Content;
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Value);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ResultValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ReferenceValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::TypeValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationTypeValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BlockValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::IntegerValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::FloatValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::DoubleValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::BooleanValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::StringValue);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::ListValue);
/*!
@}
*/