/*
Copyright 2022 creatorlxd

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
#include "IntermediateRepresentation/Function.h"
#include "RegisterAllocator.h"
#include "Linker/CompiledObject.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<Type*>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, Variable*>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, Function*>;
#endif

	class TranslateUnit;

	SPACE_LANGUAGE_API bool IsValidTranslateUnit(const TranslateUnit& tu);

	class SPACE_LANGUAGE_API TranslateUnit : public Uncopyable
	{
	public:
		friend SPACE_LANGUAGE_API bool IsValidTranslateUnit(const TranslateUnit& tu);

		TranslateUnit();
		~TranslateUnit();

		const Type& NewType(std::initializer_list<BaseType> bts);
		const Type& NewType(const Vector<BaseType>& bts);
		const Variable& NewGlobalVariable(const Type& type, SizeType idx);
		const Function& NewInternalFunction(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations);
		const Function& NewExternalFunction(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx);

		const Variable& GetGlobalVariable(UInt64 idx) const;
		const Function& GetFunction(UInt64 idx) const;

	private:
		Vector<Type*> m_Types;
		HashMap<UInt64, Variable*> m_GlobalVariables;
		HashMap<UInt64, Function*> m_Functions;
	};

	struct GlobalVariableAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The GlobalVariable has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, Variable*>::ConstIterator& citer, const HashMap<UInt64, Variable*>::ConstIterator& cend);
	};

	struct GlobalVariableNotExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The GlobalVariable has not existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, Variable*>::ConstIterator& citer, const HashMap<UInt64, Variable*>::ConstIterator& cend);
	};

	struct FunctionAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Function has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, Function*>::ConstIterator& citer, const HashMap<UInt64, Function*>::ConstIterator& cend);
	};

	struct FunctionNotExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Function has not existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, Function*>::ConstIterator& citer, const HashMap<UInt64, Function*>::ConstIterator& cend);
	};

	struct InvalidTranslateUnitError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The TranslateUnit is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const TranslateUnit& tu);
	};

	SPACE_LANGUAGE_API CompiledObject TranslateToCompiledObject(const TranslateUnit& tu);
}

/*!
@}
*/