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
#include "SGEString.hpp"

namespace SpaceGameEngine::SpaceLanguage
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

	enum class TokenType : UInt8
	{
		Identifier = 0,
		IntegerLiteral = 1,
		FloatLiteral = 2,
		DoubleLiteral = 3,
		CharacterLiteral = 4,
		StringLiteral = 5,
		LineSeparator = 6,
		WordSeparator = 7
	};

	struct Token
	{
		TokenType m_Type;
		String m_Content;
	};

	/*!
	@}
	*/
}