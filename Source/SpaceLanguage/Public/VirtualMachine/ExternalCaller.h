/*
Copyright 2023 creatorlxd

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
#include "Register.h"
#include "Container/HashMap.hpp"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage
{

	using ExternalCallFunctionType = RegisterType (*)(Registers&);

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<UInt64, ExternalCallFunctionType>;
#endif

	class SPACE_LANGUAGE_API ExternalCaller : public UncopyableAndUnmovable
	{
	public:
		void AddExternalCallFunction(UInt64 id, ExternalCallFunctionType pfunc);
		ExternalCallFunctionType GetExternalCallFunction(UInt64 id) const;
		bool HasExternalCallFunction(UInt64 id) const;

		void AddExternalCallFunction(UInt32 module_id, UInt32 func_id, ExternalCallFunctionType pfunc);
		ExternalCallFunctionType GetExternalCallFunction(UInt32 module_id, UInt32 func_id) const;
		bool HasExternalCallFunction(UInt32 module_id, UInt32 func_id) const;

		static UInt64 GetIndex(UInt32 module_id, UInt32 func_id);

	private:
		HashMap<UInt64, ExternalCallFunctionType> m_Functions;
	};

	struct ExternalCallFunctionAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The ExternalCallFunction has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map);
	};

	struct ExternalCallFunctionNotFoundError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The ExternalCallFunction can not be found.");
		static SPACE_LANGUAGE_API bool Judge(const HashMap<UInt64, ExternalCallFunctionType>::ConstIterator& iter, const HashMap<UInt64, ExternalCallFunctionType>& hash_map);
	};

}

/*!
@}
*/