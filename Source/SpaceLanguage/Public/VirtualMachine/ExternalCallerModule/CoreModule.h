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
			inline constexpr const UInt32 Int8Load = 2;
			inline constexpr const UInt32 Int8Store = 3;
			inline constexpr const UInt32 Int8Copy = 4;
			inline constexpr const UInt32 Int8Add = 5;
			inline constexpr const UInt32 Int8Subtract = 6;
			inline constexpr const UInt32 Int8Multiply = 7;
			inline constexpr const UInt32 Int8Divide = 8;
			inline constexpr const UInt32 Int8Mod = 9;
			inline constexpr const UInt32 Int8And = 10;
			inline constexpr const UInt32 Int8Or = 11;
			inline constexpr const UInt32 Int8Xor = 12;
			inline constexpr const UInt32 Int8Not = 13;
			inline constexpr const UInt32 Int8ShiftLeft = 14;
			inline constexpr const UInt32 Int8ShiftRight = 15;
			inline constexpr const UInt32 Int8Equal = 16;
			inline constexpr const UInt32 Int8NotEqual = 17;
			inline constexpr const UInt32 Int8Less = 18;
			inline constexpr const UInt32 Int8LessEqual = 19;
			inline constexpr const UInt32 Int8Greater = 20;
			inline constexpr const UInt32 Int8GreaterEqual = 21;
		}

		SPACE_LANGUAGE_API void LoadIntoExternalCaller(ExternalCaller& ec);
		SPACE_LANGUAGE_API void LoadIntoAssembler(Assembler& assembler);
	}

	/*!
	@}
	*/
}