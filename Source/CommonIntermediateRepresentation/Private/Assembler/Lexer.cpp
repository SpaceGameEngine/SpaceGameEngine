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
#include "Assembler/Lexer.h"
#include "LexerUtility.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;
using namespace SpaceGameEngine::CommonParser::Lexer;

Vector<CommonParser::Lexer::Token> SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(const String& str)
{
	return RemoveToken(
		CombineToken(
			TransformToken(SpaceGameEngine::CommonParser::Lexer::GetTokens(str, SGE_STR("line:{} column:{}, {}")),
						   [=](const Token& token) {
							   if (token.GetType() == SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier)
							   {
								   if (token.GetContent() == SGE_STR("true") || token.GetContent() == SGE_STR("false"))
								   {
									   return Token(SpaceGameEngine::CommonIntermediateRepresentation::Assembler::TokenTypes::BooleanLiteral, token.GetContent(), token.GetLine(), token.GetColumn());
								   }
							   }
							   return token;
						   }),
			{SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier, SpaceGameEngine::CommonParser::Lexer::TokenTypes::Dot, SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier}, SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier),
		{SpaceGameEngine::CommonParser::Lexer::TokenTypes::WordSeparator, SpaceGameEngine::CommonParser::Lexer::TokenTypes::LineSeparator, SpaceGameEngine::CommonParser::Lexer::TokenTypes::CommentLine, SpaceGameEngine::CommonParser::Lexer::TokenTypes::CommentBlock});
}
