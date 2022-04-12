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
#include "Lexer.h"
#include "Log.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::Lexer;

SpaceGameEngine::SpaceLanguage::Lexer::Token::Token()
	: m_Type(TokenType::Unknown)
{
}

SpaceGameEngine::SpaceLanguage::Lexer::Token::Token(TokenType token_type, const String& str)
	: m_Type(token_type), m_Content(str)
{
}

SpaceGameEngine::SpaceLanguage::Lexer::SymbolSet::SymbolSet()
	: m_Content({Pair<const Char, TokenType>(SGE_STR('!'), TokenType::Exclamation),
				 Pair<const Char, TokenType>(SGE_STR('#'), TokenType::Hash),
				 Pair<const Char, TokenType>(SGE_STR('$'), TokenType::Dollar),
				 Pair<const Char, TokenType>(SGE_STR('%'), TokenType::Mod),
				 Pair<const Char, TokenType>(SGE_STR('&'), TokenType::And),
				 Pair<const Char, TokenType>(SGE_STR('('), TokenType::LeftBracket),
				 Pair<const Char, TokenType>(SGE_STR(')'), TokenType::RightBracket),
				 Pair<const Char, TokenType>(SGE_STR('*'), TokenType::Multiply),
				 Pair<const Char, TokenType>(SGE_STR('+'), TokenType::Add),
				 Pair<const Char, TokenType>(SGE_STR(','), TokenType::Comma),
				 Pair<const Char, TokenType>(SGE_STR('-'), TokenType::Subtract),
				 Pair<const Char, TokenType>(SGE_STR('.'), TokenType::Dot),
				 Pair<const Char, TokenType>(SGE_STR('/'), TokenType::Slash),
				 Pair<const Char, TokenType>(SGE_STR(':'), TokenType::Colon),
				 Pair<const Char, TokenType>(SGE_STR(';'), TokenType::Semicolon),
				 Pair<const Char, TokenType>(SGE_STR('<'), TokenType::Less),
				 Pair<const Char, TokenType>(SGE_STR('='), TokenType::Equal),
				 Pair<const Char, TokenType>(SGE_STR('>'), TokenType::Greater),
				 Pair<const Char, TokenType>(SGE_STR('?'), TokenType::Question),
				 Pair<const Char, TokenType>(SGE_STR('@'), TokenType::At),
				 Pair<const Char, TokenType>(SGE_STR('['), TokenType::LeftSquareBracket),
				 Pair<const Char, TokenType>(SGE_STR('\\'), TokenType::Backslash),
				 Pair<const Char, TokenType>(SGE_STR(']'), TokenType::RightSquareBracket),
				 Pair<const Char, TokenType>(SGE_STR('^'), TokenType::Caret),
				 Pair<const Char, TokenType>(SGE_STR('{'), TokenType::LeftCurlyBracket),
				 Pair<const Char, TokenType>(SGE_STR('|'), TokenType::Vertical),
				 Pair<const Char, TokenType>(SGE_STR('}'), TokenType::RightCurlyBracket),
				 Pair<const Char, TokenType>(SGE_STR('~'), TokenType::Tilde)})
{
}

SpaceGameEngine::SpaceLanguage::Lexer::TokenType SpaceGameEngine::SpaceLanguage::Lexer::SymbolSet::Get(Char c) const
{
	auto iter = m_Content.Find(c);
	if (iter != m_Content.GetConstEnd())
		return iter->m_Second;
	else
		return TokenType::Unknown;
}

bool SpaceGameEngine::SpaceLanguage::Lexer::SymbolSet::IsSymbol(Char c) const
{
	if (m_Content.Find(c) != m_Content.GetConstEnd())
		return true;
	else
		return false;
}

bool SpaceGameEngine::SpaceLanguage::Lexer::InvalidSourceStringError::Judge(const String& src_str)
{
	return false;
}
