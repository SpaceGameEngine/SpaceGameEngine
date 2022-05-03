﻿/*
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
#include "VirtualMachine/ExternalCaller.h"
#include "VirtualMachine/Assembler.h"

namespace SpaceGameEngine::SpaceLanguage::ExternalCallerModule
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

	namespace CoreModule
	{
		inline constexpr const UInt32 Id = 0;

		namespace Functions
		{
			inline constexpr const UInt32 Allocate = 0;
			inline constexpr const UInt32 Free = 1;
		}

		SPACE_LANGUAGE_API void LoadIntoExternalCaller(ExternalCaller& ec);
		SPACE_LANGUAGE_API void LoadIntoAssembler(Assembler& assembler);
	}

	/*!
	@}
	*/
}