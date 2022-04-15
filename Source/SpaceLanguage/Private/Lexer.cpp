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
		state = State::CharacterEnd;
		++iter;
		return false;
	};

	//CharacterEnd
	m_States[State::CharacterEnd].Insert(SGE_STR('\''), State::Start);

	m_OtherCharacterJudgeFunctions[State::CharacterEnd] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need \' here"))));
		return true;
	};

	//EscapeCharacter
	m_States[State::EscapeCharacter].Insert({Pair<const Char, StateType>(SGE_STR('n'), State::CharacterEnd),
											 Pair<const Char, StateType>(SGE_STR('r'), State::CharacterEnd),
											 Pair<const Char, StateType>(SGE_STR('t'), State::CharacterEnd),
											 Pair<const Char, StateType>(SGE_STR('\''), State::CharacterEnd),
											 Pair<const Char, StateType>(SGE_STR('0'), State::CharacterEnd)});

	m_OtherCharacterJudgeFunctions[State::EscapeCharacter] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unsupported escape character"))));
		return true;
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
	m_States[State::StringEscapeCharacter].Insert({Pair<const Char, StateType>(SGE_STR('n'), State::String),
												   Pair<const Char, StateType>(SGE_STR('r'), State::String),
												   Pair<const Char, StateType>(SGE_STR('t'), State::String),
												   Pair<const Char, StateType>(SGE_STR('"'), State::String),
												   Pair<const Char, StateType>(SGE_STR('0'), State::String)});

	m_OtherCharacterJudgeFunctions[State::StringEscapeCharacter] = [](String::ConstIterator& iter, StateType& state, const String& error_info_formatter, SizeType line, SizeType col) -> bool {
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unsupported escape character"))));
		return true;
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

bool SpaceGameEngine::SpaceLanguage::Lexer::InvalidSourceStringError::Judge(const String& src_str, const String& error_info_formatter)
{
	return StateMachineForJudge::GetSingleton().Judge(src_str, error_info_formatter);
}
