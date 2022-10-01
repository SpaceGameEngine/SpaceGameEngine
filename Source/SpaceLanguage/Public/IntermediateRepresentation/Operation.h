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
#include "IntermediateRepresentation/Variable.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	enum class OperationType : UInt8
	{
		Set = 0,
		NewLocal = 1,
		DeleteLocal = 2,
		Push = 3,
		Pop = 4,
		Copy = 5,
		Label = 6,
		Goto = 7,
		If = 8,
		Call = 9,
		CallFunctionPointer = 10,
		Return = 11,
		ExternalCallArgument = 12,
		ExternalCall = 13,
		GetReturnValue = 14,
		MakeReference = 15,
		GetAddress = 16,
		GetReference = 17,
		ReleaseReference = 18,
		//todo
	};

	inline constexpr const SizeType OperationTypeSetSize = 19;

	struct InvalidOperationTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The OperationType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(OperationType ot);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<UInt8>;
	template struct SPACE_LANGUAGE_API Pair<Vector<UInt8>, String>;
	template class SPACE_LANGUAGE_API HashMap<OperationType, Pair<Vector<UInt8>, String>>;
#endif

	class SPACE_LANGUAGE_API OperationTypeSet : public UncopyableAndUnmovable, public Singleton<OperationTypeSet>
	{
	private:
		OperationTypeSet();

	public:
		friend DefaultAllocator;

		const Vector<UInt8>& GetArguments(OperationType type) const;
		const String& GetName(OperationType type) const;

	private:
		HashMap<OperationType, Pair<Vector<UInt8>, String>> m_Content;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<Variable>;
#endif

	class SPACE_LANGUAGE_API Operation
	{
	public:
		Operation() = delete;
		Operation(OperationType type, const Vector<Variable>& arguments);

		OperationType GetType() const;
		const Vector<Variable>& GetArguments() const;

		bool operator==(const Operation& o) const;
		bool operator!=(const Operation& o) const;

	private:
		OperationType m_Type;
		Vector<Variable> m_Arguments;
	};

	struct InvalidOperationError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The Operation is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const Operation& o);
	};

}

/*!
@}
*/