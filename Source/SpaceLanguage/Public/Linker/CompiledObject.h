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
#include "VirtualMachine/Instruction.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage
{
	class SPACE_LANGUAGE_API CompiledGlobalVariable
	{
	public:
		CompiledGlobalVariable() = delete;
		CompiledGlobalVariable(SizeType size, SizeType alignment);

		SizeType GetSize() const;
		SizeType GetAlignment() const;

		bool operator==(const CompiledGlobalVariable& cgv) const;
		bool operator!=(const CompiledGlobalVariable& cgv) const;

	private:
		SizeType m_Size;
		SizeType m_Alignment;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<UInt64>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, Vector<UInt64>>;
#endif

	class SPACE_LANGUAGE_API CompiledFunction
	{
	public:
		CompiledFunction() = delete;
		CompiledFunction(const Vector<UInt8>& instrs, const HashMap<UInt64, Vector<UInt64>>& gv_reqs, const HashMap<UInt64, Vector<UInt64>>& func_reqs);

		const Vector<UInt8>& GetInstructions() const;
		const HashMap<UInt64, Vector<UInt64>>& GetGlobalVariableRequests() const;
		const HashMap<UInt64, Vector<UInt64>>& GetFunctionRequests() const;

		bool operator==(const CompiledFunction& cfunc) const;
		bool operator!=(const CompiledFunction& cfunc) const;

	private:
		Vector<UInt8> m_Instructions;
		HashMap<UInt64, Vector<UInt64>> m_GlobalVariableRequests;
		HashMap<UInt64, Vector<UInt64>> m_FunctionRequests;
	};

	struct InvalidCompiledFunctionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The CompiledFunction is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const CompiledFunction& cfunc);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<UInt64, CompiledGlobalVariable>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, CompiledFunction>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, UInt64>;
#endif

	class SPACE_LANGUAGE_API CompiledObject
	{
	public:
		void AddCompiledGlobalVariable(UInt64 id, const CompiledGlobalVariable& cgv);
		void AddCompiledFunction(UInt64 id, const CompiledFunction& cfunc);

		const HashMap<UInt64, CompiledGlobalVariable>& GetCompiledGlobalVariables() const;
		const HashMap<UInt64, CompiledFunction>& GetCompiledFunctions() const;

		void Replace(const CompiledObject& cobj, const HashMap<UInt64, UInt64>& cgv_tran, const HashMap<UInt64, UInt64>& cfunc_tran);

	private:
		HashMap<UInt64, CompiledGlobalVariable> m_CompiledGlobalVariables;
		HashMap<UInt64, CompiledFunction> m_CompiledFunctions;
	};

	struct CompiledGlobalVariableAlreadyExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The CompiledGlobalVariable has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, CompiledGlobalVariable>::ConstIterator& citer, const HashMap<UInt64, CompiledGlobalVariable>::ConstIterator& cend);
	};

	struct CompiledFunctionAlreadyExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The CompiledFunction has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, CompiledFunction>::ConstIterator& citer, const HashMap<UInt64, CompiledFunction>::ConstIterator& cend);
	};

	struct InvalidCompiledObjectReplacementError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The CompiledObject's replacement is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const CompiledObject& self, const CompiledObject& cobj, const HashMap<UInt64, UInt64>& cgv_tran, const HashMap<UInt64, UInt64>& cfunc_tran);
	};

	SPACE_LANGUAGE_API bool IsValidCompiledObject(const CompiledObject& cobj);

	struct InvalidCompiledObjectError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The CompiledObject is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const CompiledObject& cobj);
	};
}

/*!
@}
*/