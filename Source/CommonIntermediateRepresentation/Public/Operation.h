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
#include "Interface.h"
#include "Attribute.h"
#include "Utility/DynamicCast.hpp"
#include "Container/Vector.hpp"
#include "Utility/Optional.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class Dialect;
	class Value;
	class ResultValue;
	class Type;

	class COMMON_INTERMEDIATE_REPRESENTATION_API OperationType : public DynamicCastHelperForBase<OperationType>, public InterfaceContainer<OperationType>
	{
	public:
		virtual ~OperationType();

		const String& GetName() const;
		Dialect& GetBelongedDialect();
		const Dialect& GetBelongedDialect() const;

		virtual bool VerifyArguments(const Vector<const Value*>& arguments) const;
		virtual bool VerifyResults(const Vector<const ResultValue*>& results) const;

		using DynamicCastHelperForBase<OperationType>::IsInstance;

	protected:
		OperationType(const String& name, Dialect& dialect);	// call by derived only

	private:
		String m_Name;
		Dialect& m_BelongedDialect;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API Operation
	{
	private:
		Operation(const OperationType& operation_type, Vector<const ResultValue*>&& results, Vector<const Value*>&& arguments, DictionaryAttribute&& attributes);

	public:
		friend class OperationBuilder;

		~Operation();
		Operation(const Operation&) = delete;
		Operation(Operation&&);
		Operation& operator=(const Operation&) = delete;
		Operation& operator=(Operation&&);

		SizeType GetArgumentSize() const;
		const Value& GetArgument(SizeType index) const;

		SizeType GetResultSize() const;
		const ResultValue& GetResult(SizeType index) const;

		bool Verify() const;

		// todo

		bool operator==(const Operation& other) const;

	private:
		void LinkValues();

	private:
		const OperationType* m_OperationType;
		Vector<const ResultValue*> m_Results;
		Vector<const Value*> m_Arguments;
		DictionaryAttribute m_Attributes;
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API InferReturnTypeOperationTypeInterface
	{
	public:
		virtual Optional<Vector<const Type*>> InferReturnTypes(const Vector<const Value*>& arguments) const = 0;
	};

	struct OperationBuilderBuildTwiceError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The operation builder can only build once.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool built);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API OperationBuilder
	{
	public:
		OperationBuilder(OperationType& operation_type);

		template<typename ValueType, typename... Args>
			requires std::derived_from<ValueType, Value>
		inline OperationBuilder& AddArgument(Args&&... args)
		{
			ValueType* new_argument = DefaultAllocator::New<ValueType>(std::forward<Args>(args)...);
			m_Arguments.PushBack(new_argument);
			return *this;
		}

		OperationBuilder& AddResult(const Type& type);

		// todo: AddAttribute

		OperationBuilder& InferReturnValues();

		Operation Build();

	private:
		bool m_Built;
		const OperationType* m_OperationType;
		Vector<const ResultValue*> m_Results;
		Vector<const Value*> m_Arguments;
		DictionaryAttribute m_Attributes;
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::OperationType);
SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::InferReturnTypeOperationTypeInterface);

/*!
@}
*/