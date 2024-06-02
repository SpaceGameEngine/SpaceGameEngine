/*
Copyright 2024 creatorlxd

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
#include "IntermediateRepresentation/BasicBlock.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	class SPACE_LANGUAGE_API Function
	{
	public:
		Function() = delete;

		/*!
		@brief create a external function without operations;
		*/
		Function(const Vector<const Type*>& parameter_types, const Type& result_type, UInt64 idx);

		/*!
		@brief create a internal function with operations;
		*/
		Function(const Vector<const Type*>& parameter_types, const Type& result_type, UInt64 idx, const Vector<Operation>& operations);

		const Vector<const Type*>& GetParameterTypes() const;
		const Type& GetResultType() const;
		UInt64 GetIndex() const;
		const Vector<Operation>& GetOperations() const;
		bool IsExternal() const;

		Variable ToVariable() const;
		explicit operator Variable() const;

		bool operator==(const IntermediateRepresentation::Function& func) const;
		bool operator!=(const IntermediateRepresentation::Function& func) const;

	private:
		Vector<const Type*> m_ParameterTypes;
		const Type* m_pResultType;
		UInt64 m_Index;
		Vector<Operation> m_Operations;
		bool m_IsExternal;
	};
}

/*!
@}
*/