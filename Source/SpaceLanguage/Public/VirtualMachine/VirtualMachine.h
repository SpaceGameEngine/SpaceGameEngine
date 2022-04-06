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

namespace SpaceGameEngine::SpaceLanguage
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

	struct InvalidInstructionsError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The Instructions is invalid.");
		static SPACE_LANGUAGE_API bool Judge(void* ptr, SizeType size);
	};

	class SPACE_LANGUAGE_API VirtualMachine : public UncopyableAndUnmovable
	{
	public:
		void Run(void* ptr, SizeType size);
		ExternalCaller& GetExternalCaller();
		const ExternalCaller& GetExternalCaller() const;

	private:
		Registers m_Registers;
		ExternalCaller m_ExternalCaller;
	};

	/*!
	@}
	*/
}