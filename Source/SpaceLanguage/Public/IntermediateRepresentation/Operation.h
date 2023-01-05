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
		Goto = 6,
		If = 7,
		Call = 8,
		CallFunctionPointer = 9,
		Return = 10,
		ExternalCallArgument = 11,
		ExternalCall = 12,
		GetReturnValue = 13,
		MakeReference = 14,
		GetAddress = 15,
		GetReference = 16,
		ReleaseReference = 17,
		// todo
	};

	inline constexpr const SizeType OperationTypeSetSize = 18;

	struct InvalidOperationTypeError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The OperationType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(OperationType ot);
	};

	SPACE_LANGUAGE_API bool IsTerminatorOperationType(OperationType ot);

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<Variable>;
#endif

	using OperationJudgementFunctionType = bool (*)(const Vector<Variable>&);

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<UInt8>;
#endif

	struct SPACE_LANGUAGE_API OperationTypeInformation
	{
		Vector<UInt8> m_ArgumentStorageTypeMasks;
		String m_Name;
		SizeType m_VariableRegistersSize;
		SizeType m_AdditionalRegistersSize;
		OperationJudgementFunctionType m_JudgementFunction;

		OperationTypeInformation() = delete;
		OperationTypeInformation(const Vector<UInt8>& argument_storage_type_masks, const String& name, SizeType variable_registers_size, SizeType additional_registers_size, OperationJudgementFunctionType judgement_function);

		bool operator==(const OperationTypeInformation& ot_info) const;
		bool operator!=(const OperationTypeInformation& ot_info) const;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<OperationType, OperationTypeInformation>;
#endif

	class SPACE_LANGUAGE_API OperationTypeSet : public UncopyableAndUnmovable, public Singleton<OperationTypeSet>
	{
	private:
		OperationTypeSet();

	public:
		friend DefaultAllocator;

		const OperationTypeInformation& Get(OperationType type) const;

	private:
		HashMap<OperationType, OperationTypeInformation> m_Content;
	};

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
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Operation is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const Operation& o);
	};
}

/*!
@}
*/