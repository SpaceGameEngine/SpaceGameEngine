/*
Copyright 2025 creatorlxd

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
#include "Lexer.h"
#include "Container/Set.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Lexer
{
	COMMON_PARSER_API Vector<Token> CombineToken(const Vector<Token>& tokens, const Vector<TokenType>& token_type_pattern, TokenType result_token_type, bool repeat = false);

	COMMON_PARSER_API Vector<Token> RemoveToken(const Vector<Token>& tokens, const Set<TokenType>& remove_token_type_set);
}

/*!
@}
*/