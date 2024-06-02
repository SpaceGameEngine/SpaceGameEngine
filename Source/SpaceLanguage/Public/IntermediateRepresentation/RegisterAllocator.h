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
#include "Operation.h"
#include "Container/Map.hpp"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	enum class RegisterAllocationState : UInt8
	{
		Unknown = 0,
		Active = 1,
		Inactive = 2
	};

	struct InvalidRegisterAllocationStateError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The RegisterAllocationState is invalid.");
		static SPACE_LANGUAGE_API bool Judge(RegisterAllocationState state);
	};

	struct RegisterAllocationStateUnknownError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The RegisterAllocationState is Unknown.");
		static SPACE_LANGUAGE_API bool Judge(RegisterAllocationState state);
	};

	struct RegisterAllocationStateNotUnknownError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The RegisterAllocationState is not Unknown.");
		static SPACE_LANGUAGE_API bool Judge(RegisterAllocationState state);
	};

	struct SPACE_LANGUAGE_API RegisterAllocationResult
	{
		RegisterAllocationResult() = delete;
		RegisterAllocationResult(RegisterAllocationState state, UInt64 cr_idx);

		bool operator==(const RegisterAllocationResult& result) const;
		bool operator!=(const RegisterAllocationResult& result) const;

		RegisterAllocationState m_State;
		UInt64 m_CommonRegisterIndex;
	};

	enum class FunctionCallType : UInt8
	{
		Internal = 0,
		External = 1,
		FunctionPointer = 2
	};

	struct InvalidFunctionCallTypeError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The FunctionCallType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(FunctionCallType fc_type);
	};

	class SPACE_LANGUAGE_API RegisterAllocationRequests
	{
	public:
		void AddRegisterAllocationRequest(UInt64 func_id, UInt64 operation_id, UInt64 virtual_register_id);
		const HashMap<UInt64, HashMap<UInt64, Map<UInt64, bool>>>& GetRequests() const;

		/*!
		@note when fc_type is FunctionCallType::FunctionPointer, the parameter target_func_id is useless.
		*/
		void AddFunctionCall(UInt64 func_id, UInt64 operation_id, FunctionCallType fc_type, UInt64 target_func_id);
		const HashMap<UInt64, Map<UInt64, Pair<FunctionCallType, UInt64>>>& GetFunctionCallRecords() const;
		const HashMap<UInt64, HashMap<UInt64, bool>>& GetInternalFunctionCallRelationships() const;

	private:
		HashMap<UInt64, HashMap<UInt64, Map<UInt64, bool>>> m_Content;						   // func_id, virtual_register_id, operation_id
		HashMap<UInt64, Map<UInt64, Pair<FunctionCallType, UInt64>>> m_FunctionCallRecords;	   // func_id, operation_id, <func_call_type, func_id(target)>
		HashMap<UInt64, HashMap<UInt64, bool>> m_InternalFunctionCallRelationships;			   // func_id, func_id(target)
	};

	struct RegisterAllocationRequestAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The RegisterAllocationRequest has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const Map<UInt64, bool>::ConstIterator& citer, const Map<UInt64, bool>::ConstIterator& cend);
	};

	struct FunctionCallRecordAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The FunctionCallRecord has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const Map<UInt64, Pair<FunctionCallType, UInt64>>::ConstIterator& citer, const Map<UInt64, Pair<FunctionCallType, UInt64>>::ConstIterator& cend);
	};

	class SPACE_LANGUAGE_API RegisterAllocationResults
	{
	public:
		void AddRegisterAllocationResult(UInt64 virtual_register_id, UInt64 operation_id, const RegisterAllocationResult& result);
		const HashMap<UInt64, Map<UInt64, RegisterAllocationResult>>& GetResults() const;

	private:
		HashMap<UInt64, Map<UInt64, RegisterAllocationResult>> m_Content;
	};

	struct RegisterAllocationResultAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The RegisterAllocationResult has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const Map<UInt64, RegisterAllocationResult>::ConstIterator& citer, const Map<UInt64, RegisterAllocationResult>::ConstIterator& cend);
	};

	SPACE_LANGUAGE_API RegisterAllocationResults SimpleRegisterAllocate(const RegisterAllocationRequests& reqs);
}

/*!
@}
*/