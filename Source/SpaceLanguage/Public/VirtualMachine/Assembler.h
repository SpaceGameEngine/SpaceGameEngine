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
#include "Instruction.h"
#include "Container/HashMap.hpp"

namespace SpaceGameEngine::SpaceLanguage
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<String, const InstructionType*>;
#endif

	class SPACE_LANGUAGE_API InstructionNameSet : public UncopyableAndUnmovable, public Singleton<InstructionNameSet>
	{
	private:
		InstructionNameSet();

	public:
		friend DefaultAllocator;

		const InstructionType& Get(const String& instr_name) const;
		bool IsInstructionName(const String& str) const;

	private:
		HashMap<String, const InstructionType*> m_Content;
	};

	struct InvalidInstructionNameError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The instruction name is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<String, UInt32>;
#endif

	struct InvalidAssemblerSourceStringError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The Assembler source string is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str, const String& error_info_formatter, const HashMap<String, UInt32>& name_set);
	};

	class SPACE_LANGUAGE_API Assembler
	{
	public:
		/*!
		@param error_info_formatter format string which likes "In line:{} column:{}, {}"
		*/
		InstructionsGenerator Compile(const String& str, const String& error_info_formatter) const;

	private:
		HashMap<String, UInt32> m_NameSet;
	};

	/*!
	@}
	*/
}