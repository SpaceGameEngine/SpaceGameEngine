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
#include "Logger.h"

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

bool SpaceGameEngine::SpaceLanguage::Lexer::Token::operator==(const Token& token) const
{
	return m_Type == token.m_Type && m_Content == token.m_Content;
}

bool SpaceGameEngine::SpaceLanguage::Lexer::Token::operator!=(const Token& token) const
{
	return m_Type != token.m_Type || m_Content != token.m_Content;
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
				 Pair<const Char, TokenType>(SGE_STR('~'), TokenType::Tilde),
				 Pair<const Char, TokenType>(SGE_STR('`'), TokenType::Quote)})
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

SpaceGameEngine::SpaceLanguage::Lexer::EscapeCharacterSet::EscapeCharacterSet()
	: m_Content({Pair<const Char, Char>(SGE_STR('n'), SGE_STR('\n')),
				 Pair<const Char, Char>(SGE_STR('r'), SGE_STR('\r')),
				 Pair<const Char, Char>(SGE_STR('t'), SGE_STR('\t')),
				 Pair<const Char, Char>(SGE_STR('\''), SGE_STR('\'')),
				 Pair<const Char, Char>(SGE_STR('"'), SGE_STR('"')),
				 Pair<const Char, Char>(SGE_STR('0'), SGE_STR('\0'))})
{
}

Char SpaceGameEngine::SpaceLanguage::Lexer::EscapeCharacterSet::Translate(Char c) const
{
	SGE_ASSERT(InvalidEscapeCharacterError, c);
	return m_Content.Find(c)->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::Lexer::EscapeCharacterSet::IsEscapeCharacter(Char c) const
{
	return m_Content.Find(c) != m_Content.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::Lexer::InvalidEscapeCharacterError::Judge(Char c)
{
	return c != SGE_STR('n') && c != SGE_STR('r') && c != SGE_STR('t') && c != SGE_STR('\'') && c != SGE_STR('"') && c != SGE_STR('0');
}

bool SpaceGameEngine::SpaceLanguage::Lexer::StateMachineForJudge::Judge(const String& str, const String& error_info_formatter) const
{
	StateType state = State::Start;
	String::ConstIterator iter = str.GetConstBegin();
	FileLineBreak flb = FileLineBreak::Unknown;
	SizeType line = 1;
	SizeType col = 1;

	while (iter != str.GetConstEnd())
	{
		auto next = m_States[state].Find(*iter);
		if (next != m_States[state].GetConstEnd())
		{
			state = next->m_Second;
			if (state == State::LineSeparator)
			{
				if (flb == FileLineBreak::Unknown)
				{
					flb = GetFileLineBreak<String::CharType, String::ValueTrait>(*iter, *iter);
					if (flb != FileLineBreak::CR)
					{
						++line;
						col = 1;
					}
					else
					{
						//pre load next
						auto maybe_lf = iter + 1;
						if (maybe_lf != str.GetConstEnd())
						{
							if (GetFileLineBreak<String::CharType, String::ValueTrait>(*maybe_lf, *maybe_lf) == FileLineBreak::LF)
							{
								flb = FileLineBreak::CRLF;
								++iter;
							}
							++line;
							col = 1;
						}
					}
				}
				else
				{
					if (flb == FileLineBreak::CRLF)
					{
						//pre load next
						auto maybe_lf = iter + 1;
						if (maybe_lf == str.GetConstEnd())
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid file end"))));
							return true;
						}
						if (GetFileLineBreak<String::CharType, String::ValueTrait>(*iter, *maybe_lf) != FileLineBreak::CRLF)
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid file line break"))));
							return true;
						}
						++iter;
					}
					else
					{
						if (GetFileLineBreak<String::CharType, String::ValueTrait>(*iter, *iter) != flb)
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid file line break"))));
							return true;
						}
					}
					++line;
					col = 1;
				}
			}
			else
				++col;
			++iter;
		}
		else if (m_OtherCharacterJudgeFunctions[state](iter, state, error_info_formatter, line, col))
			return true;
	}

	if (state == State::DoubleDot)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete double decimal here"))));
		return true;
	}
	if (state == State::CharacterBegin || state == State::CharacterEnd)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete character here"))));
		return true;
	}
	if (state == State::EscapeCharacter)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete escape character here"))));
		return true;
	}
	if (state == State::String)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete string here"))));
		return true;
	}
	if (state == State::StringEscapeCharacter)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete escape character in string here"))));
		return true;
	}
	if (state == State::RawStringBegin || state == State::RawString || state == State::RawStringEnd)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete raw string here"))));
		return true;
	}
	if (state == State::CommentBlock || state == State::CommentBlockEnd)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete comment block here"))));
		return true;
	}

	return false;
}

SpaceGameEngine::SpaceLanguage::Lexer::StateMachineForJudge::StateMachineForJudge()
{
	//Start
	for (Char c = SGE_STR('a'); c <= SGE_STR('z'); ++c)
		m_States[State::Start].Insert(c, State::Identifier);
	for (Char c = SGE_STR('A'); c <= SGE_STR('Z'); ++c)
		if (c != SGE_STR('R'))
			m_States[State::Start].Insert(c, State::Identifier);
	for (Char c = SGE_STR('1'); c <= SGE_STR('9'); ++c)
		m_States[State::Start].Insert(c, State::DecimalInteger);

	m_States[State::Start].Insert({Pair<const Char, StateType>(SGE_STR('_'), State::Identifier),
								   Pair<const Char, StateType>(SGE_STR('0'), State::ZeroPrefix),
								   Pair<const Char, StateType>(SGE_STR('\''), State::CharacterBegin),
								   Pair<const Char, StateType>(SGE_STR('"'), State::String),
								   Pair<const Char, StateType>(SGE_STR('R'), State::RawPrefix),
								   Pair<const Char, StateType>(SGE_STR('\n'), State::LineSeparator),
								   Pair<const Char, StateType>(SGE_STR('\r'), State::LineSeparator),
								   Pair<const Char, StateType>(SGE_STR(' '), State::WordSeparator),
								   Pair<const Char, StateType>(SGE_STR('\t'), State::WordSeparator),
								   Pair<const Char, StateType>(SGE_STR('/'), State::SlashPrefix),
								   Pair<const Char, StateType>(SGE_STR('!'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('#'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('$'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('%'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('&'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('('), State::Start),
								   Pair<const Char, StateType>(SGE_STR(')'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('*'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('+'), State::Start),
								   Pair<const Char, StateType>(SGE_STR(','), State::Start),
								   Pair<const Char, StateType>(SGE_STR('-'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('.'), State::Start),
								   Pair<const Char, StateType>(SGE_STR(':'), State::Start),
								   Pair<const Char, StateType>(SGE_STR(';'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('<'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('='), State::Start),
								   Pair<const Char, StateType>(SGE_STR('>'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('?'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('@'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('['), State::Start),
								   Pair<const Char, StateType>(SGE_STR('\\'), State::Start),
								   Pair<const Char, StateType>(SGE_STR(']'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('^'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('{'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('|'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('}'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('~'), State::Start),
								   Pair<const Char, StateType>(SGE_STR('`'), State::Start)});

	m_OtherCharacterJudgeFunctions[State::Start] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unsupported character"))));
		return true;
	};

	//Identifier
	for (Char c = SGE_STR('a'); c <= SGE_STR('z'); ++c)
		m_States[State::Identifier].Insert(c, State::Identifier);
	for (Char c = SGE_STR('A'); c <= SGE_STR('Z'); ++c)
		m_States[State::Identifier].Insert(c, State::Identifier);
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::Identifier].Insert(c, State::Identifier);

	m_States[State::Identifier].Insert(SGE_STR('_'), State::Identifier);

	m_OtherCharacterJudgeFunctions[State::Identifier] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//LineSeparator
	m_States[State::LineSeparator].Insert({Pair<const Char, StateType>(SGE_STR('\r'), State::LineSeparator),
										   Pair<const Char, StateType>(SGE_STR('\n'), State::LineSeparator)});

	m_OtherCharacterJudgeFunctions[State::LineSeparator] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//WordSeparator
	m_States[State::WordSeparator].Insert({Pair<const Char, StateType>(SGE_STR(' '), State::WordSeparator),
										   Pair<const Char, StateType>(SGE_STR('\t'), State::WordSeparator)});

	m_OtherCharacterJudgeFunctions[State::WordSeparator] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//ZeroPrefix
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::ZeroPrefix].Insert(c, State::DecimalInteger);
	m_States[State::ZeroPrefix].Insert({Pair<const Char, StateType>(SGE_STR('b'), State::BinaryInteger),
										Pair<const Char, StateType>(SGE_STR('x'), State::HexInteger)});

	m_OtherCharacterJudgeFunctions[State::ZeroPrefix] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//DecimalInteger
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::DecimalInteger].Insert(c, State::DecimalInteger);
	m_States[State::DecimalInteger].Insert(SGE_STR('.'), State::DoubleDot);

	m_OtherCharacterJudgeFunctions[State::DecimalInteger] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//BinaryInteger
	m_States[State::BinaryInteger].Insert(SGE_STR('0'), State::BinaryInteger);
	m_States[State::BinaryInteger].Insert(SGE_STR('1'), State::BinaryInteger);

	m_OtherCharacterJudgeFunctions[State::BinaryInteger] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		Char c = *iter;
		if (c >= SGE_STR('2') && c <= SGE_STR('9'))
		{
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid number in binary integer"))));
			return true;
		}
		else
		{
			state = State::Start;
			return false;
		}
	};

	//HexInteger
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::HexInteger].Insert(c, State::HexInteger);
	for (Char c = SGE_STR('a'); c <= SGE_STR('f'); ++c)
		m_States[State::HexInteger].Insert(c, State::HexInteger);
	for (Char c = SGE_STR('A'); c <= SGE_STR('F'); ++c)
		m_States[State::HexInteger].Insert(c, State::HexInteger);

	m_OtherCharacterJudgeFunctions[State::HexInteger] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//DoubleDot
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::DoubleDot].Insert(c, State::Double);

	m_OtherCharacterJudgeFunctions[State::DoubleDot] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete double decimal here"))));
		return true;
	};

	//Double
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::Double].Insert(c, State::Double);
	m_States[State::Double].Insert(SGE_STR('f'), State::Start);

	m_OtherCharacterJudgeFunctions[State::Double] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//CharacterBegin
	m_States[State::CharacterBegin].Insert(SGE_STR('\\'), State::EscapeCharacter);

	m_OtherCharacterJudgeFunctions[State::CharacterBegin] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		if (*iter == SGE_STR('\''))
		{
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need character here"))));
			return true;
		}
		else
		{
			state = State::CharacterEnd;
			++iter;
			return false;
		}
	};

	//CharacterEnd
	m_States[State::CharacterEnd].Insert(SGE_STR('\''), State::Start);

	m_OtherCharacterJudgeFunctions[State::CharacterEnd] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need \' here"))));
		return true;
	};

	//EscapeCharacter
	m_OtherCharacterJudgeFunctions[State::EscapeCharacter] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		if (EscapeCharacterSet::GetSingleton().IsEscapeCharacter(*iter))
		{
			state = State::CharacterEnd;
			++iter;
			return false;
		}
		else
		{
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unsupported escape character"))));
			return true;
		}
	};

	//String
	m_States[State::String].Insert({
		Pair<const Char, StateType>(SGE_STR('\\'), State::StringEscapeCharacter),
		Pair<const Char, StateType>(SGE_STR('"'), State::Start),
	});

	m_OtherCharacterJudgeFunctions[State::String] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		Char c = *iter;
		if (c == SGE_STR('\r') || c == SGE_STR('\n'))
		{
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need \" here"))));
			return true;
		}
		else
		{
			//state not change
			++iter;
			return false;
		}
	};

	//StringEscapeCharacter
	m_OtherCharacterJudgeFunctions[State::StringEscapeCharacter] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		if (EscapeCharacterSet::GetSingleton().IsEscapeCharacter(*iter))
		{
			state = State::String;
			++iter;
			return false;
		}
		else
		{
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unsupported escape character"))));
			return true;
		}
	};

	//RawPrefix
	m_States[State::RawPrefix].Insert(SGE_STR('"'), State::RawStringBegin);

	m_OtherCharacterJudgeFunctions[State::RawPrefix] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Identifier;
		return false;
	};

	//RawStringBegin
	m_States[State::RawStringBegin].Insert(SGE_STR('('), State::RawString);

	m_OtherCharacterJudgeFunctions[State::RawStringBegin] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need ( here"))));
		return true;
	};

	//RawString
	m_States[State::RawString].Insert(SGE_STR(')'), State::RawStringEnd);

	m_OtherCharacterJudgeFunctions[State::RawString] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		//state not change
		++iter;
		return false;
	};

	//RawStringEnd
	m_States[State::RawStringEnd].Insert(SGE_STR('"'), State::Start);

	m_OtherCharacterJudgeFunctions[State::RawStringEnd] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::RawString;
		return false;
	};

	//SlashPrefix
	m_States[State::SlashPrefix].Insert({Pair<const Char, StateType>(SGE_STR('/'), State::CommentLine),
										 Pair<const Char, StateType>(SGE_STR('*'), State::CommentBlock)});

	m_OtherCharacterJudgeFunctions[State::SlashPrefix] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::Start;
		return false;
	};

	//CommentBlock
	m_States[State::CommentBlock].Insert(SGE_STR('*'), State::CommentBlockEnd);

	m_OtherCharacterJudgeFunctions[State::CommentBlock] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		//state not change
		++iter;
		return false;
	};

	//CommentBlockEnd
	m_States[State::CommentBlockEnd].Insert(SGE_STR('/'), State::Start);

	m_OtherCharacterJudgeFunctions[State::CommentBlockEnd] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		state = State::CommentBlock;
		return false;
	};

	//CommentLine
	m_States[State::CommentLine].Insert({Pair<const Char, StateType>(SGE_STR('\r'), State::LineSeparator),
										 Pair<const Char, StateType>(SGE_STR('\n'), State::LineSeparator)});

	m_OtherCharacterJudgeFunctions[State::CommentLine] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		//state not change
		++iter;
		return false;
	};
}

bool SpaceGameEngine::SpaceLanguage::Lexer::InvalidSourceStringError::Judge(const String& str, const String& error_info_formatter)
{
	return StateMachineForJudge::GetSingleton().Judge(str, error_info_formatter);
}

SpaceGameEngine::SpaceLanguage::Lexer::StateTransfer::StateTransfer()
	: m_NextState(State::Start), m_Signal(StateMachineControlSignal::Forward), m_TokenType(TokenType::Unknown)
{
}

SpaceGameEngine::SpaceLanguage::Lexer::StateTransfer::StateTransfer(StateType next_state, StateMachineControlSignal sign, TokenType token_type)
	: m_NextState(next_state), m_Signal(sign), m_TokenType(token_type)
{
}

Vector<Token> SpaceGameEngine::SpaceLanguage::Lexer::StateMachine::Run(const String& str) const
{
	Vector<Token> result;
	String str_buf;
	StateType state = State::Start;
	String::ConstIterator iter = str.GetConstBegin();
	FileLineBreak flb = FileLineBreak::Unknown;

	while (iter != str.GetConstEnd())
	{
		StateTransfer st;
		auto state_iter = m_States[state].Find(*iter);
		if (state_iter != m_States[state].GetConstEnd())
			st = state_iter->m_Second;
		else
			st = m_OtherCharacterStates[state];

		if (st.m_Signal == StateMachineControlSignal::Forward)
		{
			str_buf += *iter;
			++iter;
		}
		else if (st.m_Signal == StateMachineControlSignal::Skip)
			++iter;
		else if (st.m_Signal == StateMachineControlSignal::Submit)
		{
			result.EmplaceBack(st.m_TokenType, str_buf);
			str_buf.Clear();
		}
		else if (st.m_Signal == StateMachineControlSignal::SubmitSymbol)
		{
			result.EmplaceBack(st.m_TokenType, String(1, *iter));
			++iter;
		}
		else if (st.m_Signal == StateMachineControlSignal::SubmitLineSeparator)
		{
			if (str_buf.GetSize() == 1)
				result.EmplaceBack(st.m_TokenType, str_buf);
			else if (str_buf.GetSize() >= 2)
			{
				if ((str_buf.GetSize() % 2 == 0) && (GetFileLineBreak<String::CharType, String::ValueTrait>(str_buf[0], str_buf[1]) == FileLineBreak::CRLF))
				{
					String crlf = str_buf.Substring(str_buf.GetConstBegin(), 2);
					for (SizeType i = 0; i < str_buf.GetSize() / 2; ++i)
						result.EmplaceBack(st.m_TokenType, crlf);
				}
				else
				{
					String linebreak(1, str_buf[0]);
					for (SizeType i = 0; i < str_buf.GetSize(); ++i)
						result.EmplaceBack(st.m_TokenType, linebreak);
				}
			}
			str_buf.Clear();
		}
		else if (st.m_Signal == StateMachineControlSignal::SubmitSkip)
		{
			result.EmplaceBack(st.m_TokenType, str_buf);
			str_buf.Clear();
			++iter;
		}
		else if (st.m_Signal == StateMachineControlSignal::EscapeCharacter)
		{
			str_buf[str_buf.GetSize() - 1] = EscapeCharacterSet::GetSingleton().Translate(*iter);
			++iter;
		}
		else if (st.m_Signal == StateMachineControlSignal::Clear)
		{
			str_buf.Clear();
			++iter;
		}
		else if (st.m_Signal == StateMachineControlSignal::RawStringEndBack)
			str_buf += SGE_STR(')');
		else if (st.m_Signal == StateMachineControlSignal::CommentBlockEndBack)
			str_buf += SGE_STR('*');

		state = st.m_NextState;
	}

	if (state == State::CommentLine)
		result.EmplaceBack(TokenType::Comment, str_buf);
	else
	{
		while (state != State::Start)
		{
			StateTransfer st = m_OtherCharacterStates[state];

			if (st.m_Signal == StateMachineControlSignal::Submit)
				result.EmplaceBack(st.m_TokenType, str_buf);
			else if (st.m_Signal == StateMachineControlSignal::SubmitLineSeparator)
			{
				if (str_buf.GetSize() == 1)
					result.EmplaceBack(st.m_TokenType, str_buf);
				else if (str_buf.GetSize() >= 2)
				{
					if ((str_buf.GetSize() % 2 == 0) && (GetFileLineBreak<String::CharType, String::ValueTrait>(str_buf[0], str_buf[1]) == FileLineBreak::CRLF))
					{
						String crlf = str_buf.Substring(str_buf.GetConstBegin(), 2);
						for (SizeType i = 0; i < str_buf.GetSize() / 2; ++i)
							result.EmplaceBack(st.m_TokenType, crlf);
					}
					else
					{
						String linebreak(1, str_buf[0]);
						for (SizeType i = 0; i < str_buf.GetSize(); ++i)
							result.EmplaceBack(st.m_TokenType, linebreak);
					}
				}
			}

			state = st.m_NextState;
		}
	}

	return result;
}

SpaceGameEngine::SpaceLanguage::Lexer::StateMachine::StateMachine()
{
	//Start
	for (Char c = SGE_STR('1'); c <= SGE_STR('9'); ++c)
		m_States[State::Start].Insert(c, StateTransfer(State::DecimalInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));
	for (Char c = SGE_STR('a'); c <= SGE_STR('z'); ++c)
		m_States[State::Start].Insert(c, StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));
	for (Char c = SGE_STR('A'); c <= SGE_STR('Z'); ++c)
		if (c != SGE_STR('R'))
			m_States[State::Start].Insert(c, StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));

	m_States[State::Start].Insert({Pair<const Char, StateTransfer>(SGE_STR('0'), StateTransfer(State::ZeroPrefix, StateMachineControlSignal::Forward, TokenType::IntegerLiteral)),
								   Pair<const Char, StateTransfer>(SGE_STR('\''), StateTransfer(State::CharacterBegin, StateMachineControlSignal::Skip, TokenType::CharacterLiteral)),
								   Pair<const Char, StateTransfer>(SGE_STR('"'), StateTransfer(State::String, StateMachineControlSignal::Skip, TokenType::StringLiteral)),
								   Pair<const Char, StateTransfer>(SGE_STR('R'), StateTransfer(State::RawPrefix, StateMachineControlSignal::Forward, TokenType::StringLiteral)),
								   Pair<const Char, StateTransfer>(SGE_STR('_'), StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier)),
								   Pair<const Char, StateTransfer>(SGE_STR('/'), StateTransfer(State::SlashPrefix, StateMachineControlSignal::Forward, TokenType::Comment)),
								   Pair<const Char, StateTransfer>(SGE_STR('\r'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Forward, TokenType::LineSeparator)),
								   Pair<const Char, StateTransfer>(SGE_STR('\n'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Forward, TokenType::LineSeparator)),
								   Pair<const Char, StateTransfer>(SGE_STR(' '), StateTransfer(State::WordSeparator, StateMachineControlSignal::Forward, TokenType::WordSeparator)),
								   Pair<const Char, StateTransfer>(SGE_STR('\t'), StateTransfer(State::WordSeparator, StateMachineControlSignal::Forward, TokenType::WordSeparator)),
								   Pair<const Char, StateTransfer>(SGE_STR('!'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Exclamation)),
								   Pair<const Char, StateTransfer>(SGE_STR('#'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Hash)),
								   Pair<const Char, StateTransfer>(SGE_STR('$'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Dollar)),
								   Pair<const Char, StateTransfer>(SGE_STR('%'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Mod)),
								   Pair<const Char, StateTransfer>(SGE_STR('&'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::And)),
								   Pair<const Char, StateTransfer>(SGE_STR('('), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::LeftBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR(')'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::RightBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR('*'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Multiply)),
								   Pair<const Char, StateTransfer>(SGE_STR('+'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Add)),
								   Pair<const Char, StateTransfer>(SGE_STR(','), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Comma)),
								   Pair<const Char, StateTransfer>(SGE_STR('-'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Subtract)),
								   Pair<const Char, StateTransfer>(SGE_STR('.'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Dot)),
								   Pair<const Char, StateTransfer>(SGE_STR(':'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Colon)),
								   Pair<const Char, StateTransfer>(SGE_STR(';'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Semicolon)),
								   Pair<const Char, StateTransfer>(SGE_STR('<'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Less)),
								   Pair<const Char, StateTransfer>(SGE_STR('='), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Equal)),
								   Pair<const Char, StateTransfer>(SGE_STR('>'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Greater)),
								   Pair<const Char, StateTransfer>(SGE_STR('?'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Question)),
								   Pair<const Char, StateTransfer>(SGE_STR('@'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::At)),
								   Pair<const Char, StateTransfer>(SGE_STR('['), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::LeftSquareBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR('\\'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Backslash)),
								   Pair<const Char, StateTransfer>(SGE_STR(']'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::RightSquareBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR('^'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Caret)),
								   Pair<const Char, StateTransfer>(SGE_STR('{'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::LeftCurlyBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR('|'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Vertical)),
								   Pair<const Char, StateTransfer>(SGE_STR('}'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::RightCurlyBracket)),
								   Pair<const Char, StateTransfer>(SGE_STR('~'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Tilde)),
								   Pair<const Char, StateTransfer>(SGE_STR('`'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSymbol, TokenType::Quote))});

	//Identifier
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::Identifier].Insert(c, StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));
	for (Char c = SGE_STR('a'); c <= SGE_STR('z'); ++c)
		m_States[State::Identifier].Insert(c, StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));
	for (Char c = SGE_STR('A'); c <= SGE_STR('Z'); ++c)
		m_States[State::Identifier].Insert(c, StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));
	m_States[State::Identifier].Insert(SGE_STR('_'), StateTransfer(State::Identifier, StateMachineControlSignal::Forward, TokenType::Identifier));

	m_OtherCharacterStates[State::Identifier] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::Identifier);

	//LineSeparator
	m_States[State::LineSeparator].Insert({Pair<const Char, StateTransfer>(SGE_STR('\r'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Forward, TokenType::LineSeparator)),
										   Pair<const Char, StateTransfer>(SGE_STR('\n'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Forward, TokenType::LineSeparator))});

	m_OtherCharacterStates[State::LineSeparator] = StateTransfer(State::Start, StateMachineControlSignal::SubmitLineSeparator, TokenType::LineSeparator);

	//WordSeparator
	m_States[State::WordSeparator].Insert({Pair<const Char, StateTransfer>(SGE_STR(' '), StateTransfer(State::WordSeparator, StateMachineControlSignal::Forward, TokenType::WordSeparator)),
										   Pair<const Char, StateTransfer>(SGE_STR('\t'), StateTransfer(State::WordSeparator, StateMachineControlSignal::Forward, TokenType::WordSeparator))});

	m_OtherCharacterStates[State::WordSeparator] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::WordSeparator);

	//ZeroPrefix
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::ZeroPrefix].Insert(c, StateTransfer(State::DecimalInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));
	m_States[State::ZeroPrefix].Insert({Pair<const Char, StateTransfer>(SGE_STR('b'), StateTransfer(State::BinaryInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral)),
										Pair<const Char, StateTransfer>(SGE_STR('x'), StateTransfer(State::HexInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral))});

	m_OtherCharacterStates[State::ZeroPrefix] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::IntegerLiteral);

	//DecimalInteger
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::DecimalInteger].Insert(c, StateTransfer(State::DecimalInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));
	m_States[State::DecimalInteger].Insert(SGE_STR('.'), StateTransfer(State::DoubleDot, StateMachineControlSignal::Forward, TokenType::DoubleLiteral));

	m_OtherCharacterStates[State::DecimalInteger] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::IntegerLiteral);

	//BinaryInteger
	m_States[State::BinaryInteger].Insert({Pair<const Char, StateTransfer>(SGE_STR('0'), StateTransfer(State::BinaryInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral)),
										   Pair<const Char, StateTransfer>(SGE_STR('1'), StateTransfer(State::BinaryInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral))});

	m_OtherCharacterStates[State::BinaryInteger] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::IntegerLiteral);

	//HexInteger
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::HexInteger].Insert(c, StateTransfer(State::HexInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));
	for (Char c = SGE_STR('a'); c <= SGE_STR('f'); ++c)
		m_States[State::HexInteger].Insert(c, StateTransfer(State::HexInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));
	for (Char c = SGE_STR('A'); c <= SGE_STR('F'); ++c)
		m_States[State::HexInteger].Insert(c, StateTransfer(State::HexInteger, StateMachineControlSignal::Forward, TokenType::IntegerLiteral));

	m_OtherCharacterStates[State::HexInteger] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::IntegerLiteral);

	//DoubleDot
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::DoubleDot].Insert(c, StateTransfer(State::Double, StateMachineControlSignal::Forward, TokenType::DoubleLiteral));

	//Double
	for (Char c = SGE_STR('0'); c <= SGE_STR('9'); ++c)
		m_States[State::Double].Insert(c, StateTransfer(State::Double, StateMachineControlSignal::Forward, TokenType::DoubleLiteral));
	m_States[State::Double].Insert(SGE_STR('f'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSkip, TokenType::FloatLiteral));

	m_OtherCharacterStates[State::Double] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::DoubleLiteral);

	//CharacterBegin
	m_States[State::CharacterBegin].Insert(SGE_STR('\\'), StateTransfer(State::EscapeCharacter, StateMachineControlSignal::Forward, TokenType::CharacterLiteral));

	m_OtherCharacterStates[State::CharacterBegin] = StateTransfer(State::CharacterEnd, StateMachineControlSignal::Forward, TokenType::CharacterLiteral);

	//CharacterEnd
	m_States[State::CharacterEnd].Insert(SGE_STR('\''), StateTransfer(State::Start, StateMachineControlSignal::SubmitSkip, TokenType::CharacterLiteral));

	//EscapeCharacter
	m_OtherCharacterStates[State::EscapeCharacter] = StateTransfer(State::CharacterEnd, StateMachineControlSignal::EscapeCharacter, TokenType::CharacterLiteral);

	//String
	m_States[State::String].Insert({Pair<const Char, StateTransfer>(SGE_STR('\\'), StateTransfer(State::StringEscapeCharacter, StateMachineControlSignal::Forward, TokenType::StringLiteral)),
									Pair<const Char, StateTransfer>(SGE_STR('"'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSkip, TokenType::StringLiteral))});

	m_OtherCharacterStates[State::String] = StateTransfer(State::String, StateMachineControlSignal::Forward, TokenType::StringLiteral);

	//StringEscapeCharacter
	m_OtherCharacterStates[State::StringEscapeCharacter] = StateTransfer(State::String, StateMachineControlSignal::EscapeCharacter, TokenType::StringLiteral);

	//RawPrefix
	m_States[State::RawPrefix].Insert(SGE_STR('"'), StateTransfer(State::RawStringBegin, StateMachineControlSignal::Clear, TokenType::StringLiteral));

	m_OtherCharacterStates[State::RawPrefix] = StateTransfer(State::Identifier, StateMachineControlSignal::Stay, TokenType::Identifier);

	//RawStringBegin
	m_States[State::RawStringBegin].Insert(SGE_STR('('), StateTransfer(State::RawString, StateMachineControlSignal::Skip, TokenType::StringLiteral));

	//RawString
	m_States[State::RawString].Insert(SGE_STR(')'), StateTransfer(State::RawStringEnd, StateMachineControlSignal::Skip, TokenType::StringLiteral));

	m_OtherCharacterStates[State::RawString] = StateTransfer(State::RawString, StateMachineControlSignal::Forward, TokenType::StringLiteral);

	//RawStringEnd
	m_States[State::RawStringEnd].Insert(SGE_STR('"'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSkip, TokenType::StringLiteral));

	m_OtherCharacterStates[State::RawStringEnd] = StateTransfer(State::RawString, StateMachineControlSignal::RawStringEndBack, TokenType::StringLiteral);

	//SlashPrefix
	m_States[State::SlashPrefix].Insert({Pair<const Char, StateTransfer>(SGE_STR('/'), StateTransfer(State::CommentLine, StateMachineControlSignal::Clear, TokenType::Comment)),
										 Pair<const Char, StateTransfer>(SGE_STR('*'), StateTransfer(State::CommentBlock, StateMachineControlSignal::Clear, TokenType::Comment))});

	m_OtherCharacterStates[State::SlashPrefix] = StateTransfer(State::Start, StateMachineControlSignal::Submit, TokenType::Slash);

	//CommentBlock
	m_States[State::CommentBlock].Insert(SGE_STR('*'), StateTransfer(State::CommentBlockEnd, StateMachineControlSignal::Skip, TokenType::Comment));

	m_OtherCharacterStates[State::CommentBlock] = StateTransfer(State::CommentBlock, StateMachineControlSignal::Forward, TokenType::Comment);

	//CommentBlockEnd
	m_States[State::CommentBlockEnd].Insert(SGE_STR('/'), StateTransfer(State::Start, StateMachineControlSignal::SubmitSkip, TokenType::Comment));

	m_OtherCharacterStates[State::CommentBlockEnd] = StateTransfer(State::CommentBlock, StateMachineControlSignal::CommentBlockEndBack, TokenType::Comment);

	//CommentLine
	m_States[State::CommentLine].Insert({Pair<const Char, StateTransfer>(SGE_STR('\r'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Submit, TokenType::Comment)),
										 Pair<const Char, StateTransfer>(SGE_STR('\n'), StateTransfer(State::LineSeparator, StateMachineControlSignal::Submit, TokenType::Comment))});

	m_OtherCharacterStates[State::CommentLine] = StateTransfer(State::CommentLine, StateMachineControlSignal::Forward, TokenType::Comment);
}

Vector<Token> SpaceGameEngine::SpaceLanguage::Lexer::GetTokens(const String& str, const String& error_info_formatter)
{
	SGE_ASSERT(InvalidSourceStringError, str, error_info_formatter);
	return StateMachine::GetSingleton().Run(str);
}
