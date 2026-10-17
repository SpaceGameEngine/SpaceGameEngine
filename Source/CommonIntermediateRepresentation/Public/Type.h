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
#include "Utility/DynamicCast.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	class Dialect;

	class COMMON_INTERMEDIATE_REPRESENTATION_API Type : public DynamicCastHelperForBase<Type>, InterfaceContainer<Type>
	{
	public:
		virtual ~Type();

		const String& GetName() const;
		Dialect& GetBelongedDialect();
		const Dialect& GetBelongedDialect() const;

		using DynamicCastHelperForBase<Type>::IsInstance;

	protected:
		Type(const String& name, Dialect& dialect);	   // call by derived only

	private:
		String m_Name;
		Dialect& m_BelongedDialect;
	};
}

SGE_DECLARE_TYPE_ID(COMMON_INTERMEDIATE_REPRESENTATION_API, SpaceGameEngine::CommonIntermediateRepresentation::Type);

/*!
@}
*/