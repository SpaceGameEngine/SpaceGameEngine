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
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterAllocationState is invalid.");
		static SPACE_LANGUAGE_API bool Judge(RegisterAllocationState state);
	};

	struct RegisterAllocationStateUnknownError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterAllocationState is Unknown.");
		static SPACE_LANGUAGE_API bool Judge(RegisterAllocationState state);
	};

	struct RegisterAllocationStateNotUnknownError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterAllocationState is not Unknown.");
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

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API MapImplement::RedBlackTree<UInt64, bool>;
	template class SPACE_LANGUAGE_API Map<UInt64, bool>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, Map<UInt64, bool>>;
#endif

	class SPACE_LANGUAGE_API RegisterAllocationRequests
	{
	public:
		void AddRegisterAllocationRequest(UInt64 virtual_register_id, UInt64 operation_id);
		const HashMap<UInt64, Map<UInt64, bool>>& GetRequests() const;

	private:
		HashMap<UInt64, Map<UInt64, bool>> m_Content;
	};

	struct RegisterAllocationRequestAlreadyExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterAllocationRequest has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const Map<UInt64, bool>::ConstIterator& citer, const Map<UInt64, bool>::ConstIterator& cend);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API MapImplement::RedBlackTree<UInt64, RegisterAllocationResult>;
	template class SPACE_LANGUAGE_API Map<UInt64, RegisterAllocationResult>;
	template class SPACE_LANGUAGE_API HashMap<UInt64, Map<UInt64, RegisterAllocationResult>>;
#endif

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
		inline static const TChar sm_pContent[] = SGE_TSTR("The RegisterAllocationResult has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const Map<UInt64, RegisterAllocationResult>::ConstIterator& citer, const Map<UInt64, RegisterAllocationResult>::ConstIterator& cend);
	};

	SPACE_LANGUAGE_API RegisterAllocationResults SimpleRegisterAllocate(const RegisterAllocationRequests& reqs);
}

/*!
@}
*/