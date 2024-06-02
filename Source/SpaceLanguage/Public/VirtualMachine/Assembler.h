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
#include "Instruction.h"
#include "Container/HashMap.hpp"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage
{
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
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The instruction name is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str);
	};

	class SPACE_LANGUAGE_API RegisterNameSet : public UncopyableAndUnmovable, public Singleton<RegisterNameSet>
	{
	private:
		RegisterNameSet();

	public:
		friend DefaultAllocator;

		UInt8 Get(const String& register_name) const;
		bool IsRegisterName(const String& str) const;

	private:
		HashMap<String, UInt8> m_Content;
	};

	struct InvalidRegisterNameError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The register name is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str);
	};

	/*!
	@note include checking for whether source string is valid for getting tokens or not.
	*/
	struct InvalidAssemblerSourceStringError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Assembler source string is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str, const String& error_info_formatter, const HashMap<String, Pair<UInt32, HashMap<String, UInt32>>>& module_functions);
	};

	struct AssemblerExternalCallerModuleAlreadyExistError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The ExternalCallerModule in Assembler has already existed.");
		static SPACE_LANGUAGE_API bool Judge(const String& module_name, const HashMap<String, Pair<UInt32, HashMap<String, UInt32>>>& module_functions);
	};

	class SPACE_LANGUAGE_API Assembler
	{
	public:
		void RegisterExternalCallerModule(const String& module_name, UInt32 module_id, const HashMap<String, UInt32>& functions);

		/*!
		@param error_info_formatter format string which likes "In line:{} column:{}, {}"
		*/
		Vector<UInt8> Compile(const String& str, const String& error_info_formatter) const;

	private:
		HashMap<String, Pair<UInt32, HashMap<String, UInt32>>> m_ModuleFunctions;
	};

}

/*!
@}
*/