/*
Copyright 2026 creatorlxd

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
using namespace SpaceGameEngine::CommonParser;
using namespace SpaceGameEngine::CommonParser::Lexer;

bool SpaceGameEngine::CommonParser::Lexer::InvalidTokenTypeError::Judge(TokenType tt)
{
	return tt >= TokenTypes::DefaultTokenTypesCount;
}

SpaceGameEngine::CommonParser::Lexer::Token::Token()
	: m_Type(TokenTypes::Unknown), m_Line(1), m_Column(1)
{
}

SpaceGameEngine::CommonParser::Lexer::Token::Token(TokenType token_type, const String& str, SizeType line, SizeType column)
	: m_Type(token_type), m_Content(str), m_Line(line), m_Column(column)
{
	// SGE_ASSERT(InvalidTokenTypeError, token_type);	// allow custom token type
	SGE_ASSERT(InvalidValueError, line, 1, UINT64_MAX);
	SGE_ASSERT(InvalidValueError, column, 1, UINT64_MAX);
}

SpaceGameEngine::CommonParser::Lexer::TokenType SpaceGameEngine::CommonParser::Lexer::Token::GetType() const
{
	return m_Type;
}

const String& SpaceGameEngine::CommonParser::Lexer::Token::GetContent() const
{
	return m_Content;
}

SizeType SpaceGameEngine::CommonParser::Lexer::Token::GetLine() const
{
	return m_Line;
}

SizeType SpaceGameEngine::CommonParser::Lexer::Token::GetColumn() const
{
	return m_Column;
}

bool SpaceGameEngine::CommonParser::Lexer::Token::operator==(const Token& token) const
{
	return m_Type == token.m_Type && m_Content == token.m_Content && m_Line == token.m_Line && m_Column == token.m_Column;
}

bool SpaceGameEngine::CommonParser::Lexer::Token::operator!=(const Token& token) const
{
	return m_Type != token.m_Type || m_Content != token.m_Content || m_Line != token.m_Line || m_Column != token.m_Column;
}

SpaceGameEngine::CommonParser::Lexer::SymbolSet::SymbolSet()
	: m_Content({Pair<const Char, TokenType>(SGE_STR('!'), TokenTypes::Exclamation),
				 Pair<const Char, TokenType>(SGE_STR('#'), TokenTypes::Hash),
				 Pair<const Char, TokenType>(SGE_STR('$'), TokenTypes::Dollar),
				 Pair<const Char, TokenType>(SGE_STR('%'), TokenTypes::Mod),
				 Pair<const Char, TokenType>(SGE_STR('&'), TokenTypes::And),
				 Pair<const Char, TokenType>(SGE_STR('('), TokenTypes::LeftBracket),
				 Pair<const Char, TokenType>(SGE_STR(')'), TokenTypes::RightBracket),
				 Pair<const Char, TokenType>(SGE_STR('*'), TokenTypes::Multiply),
				 Pair<const Char, TokenType>(SGE_STR('+'), TokenTypes::Add),
				 Pair<const Char, TokenType>(SGE_STR(','), TokenTypes::Comma),
				 Pair<const Char, TokenType>(SGE_STR('-'), TokenTypes::Subtract),
				 Pair<const Char, TokenType>(SGE_STR('.'), TokenTypes::Dot),
				 Pair<const Char, TokenType>(SGE_STR('/'), TokenTypes::Slash),
				 Pair<const Char, TokenType>(SGE_STR(':'), TokenTypes::Colon),
				 Pair<const Char, TokenType>(SGE_STR(';'), TokenTypes::Semicolon),
				 Pair<const Char, TokenType>(SGE_STR('<'), TokenTypes::Less),
				 Pair<const Char, TokenType>(SGE_STR('='), TokenTypes::Equal),
				 Pair<const Char, TokenType>(SGE_STR('>'), TokenTypes::Greater),
				 Pair<const Char, TokenType>(SGE_STR('?'), TokenTypes::Question),
				 Pair<const Char, TokenType>(SGE_STR('@'), TokenTypes::At),
				 Pair<const Char, TokenType>(SGE_STR('['), TokenTypes::LeftSquareBracket),
				 Pair<const Char, TokenType>(SGE_STR('\\'), TokenTypes::Backslash),
				 Pair<const Char, TokenType>(SGE_STR(']'), TokenTypes::RightSquareBracket),
				 Pair<const Char, TokenType>(SGE_STR('^'), TokenTypes::Caret),
				 Pair<const Char, TokenType>(SGE_STR('{'), TokenTypes::LeftCurlyBracket),
				 Pair<const Char, TokenType>(SGE_STR('|'), TokenTypes::Vertical),
				 Pair<const Char, TokenType>(SGE_STR('}'), TokenTypes::RightCurlyBracket),
				 Pair<const Char, TokenType>(SGE_STR('~'), TokenTypes::Tilde),
				 Pair<const Char, TokenType>(SGE_STR('`'), TokenTypes::Quote)})
{
}

SpaceGameEngine::CommonParser::Lexer::TokenType SpaceGameEngine::CommonParser::Lexer::SymbolSet::Get(Char c) const
{
	auto iter = m_Content.Find(c);
	if (iter != m_Content.GetConstEnd())
		return iter->m_Second;
	else
		return TokenTypes::Unknown;
}

bool SpaceGameEngine::CommonParser::Lexer::SymbolSet::IsSymbol(Char c) const
{
	if (m_Content.Find(c) != m_Content.GetConstEnd())
		return true;
	else
		return false;
}

SpaceGameEngine::CommonParser::Lexer::EscapeCharacterSet::EscapeCharacterSet()
	: m_Content({Pair<const Char, Char>(SGE_STR('n'), SGE_STR('\n')),
				 Pair<const Char, Char>(SGE_STR('r'), SGE_STR('\r')),
				 Pair<const Char, Char>(SGE_STR('t'), SGE_STR('\t')),
				 Pair<const Char, Char>(SGE_STR('\''), SGE_STR('\'')),
				 Pair<const Char, Char>(SGE_STR('"'), SGE_STR('"')),
				 Pair<const Char, Char>(SGE_STR('0'), SGE_STR('\0'))})
{
}

Char SpaceGameEngine::CommonParser::Lexer::EscapeCharacterSet::Translate(Char c) const
{
	SGE_ASSERT(InvalidEscapeCharacterError, c);
	return m_Content.Find(c)->m_Second;
}

bool SpaceGameEngine::CommonParser::Lexer::EscapeCharacterSet::IsEscapeCharacter(Char c) const
{
	return m_Content.Find(c) != m_Content.GetConstEnd();
}

bool SpaceGameEngine::CommonParser::Lexer::InvalidEscapeCharacterError::Judge(Char c)
{
	return !EscapeCharacterSet::GetSingleton().IsEscapeCharacter(c);
}

namespace SpaceGameEngine
{
	template class COMMON_PARSER_API_TEMPLATE_DEFINE Vector<SpaceGameEngine::CommonParser::Lexer::Token>;
	template class COMMON_PARSER_API_TEMPLATE_DEFINE Vector<SpaceGameEngine::CommonParser::ParserError>;
	template struct COMMON_PARSER_API_TEMPLATE_DEFINE Pair<Vector<SpaceGameEngine::CommonParser::Lexer::Token>, Vector<SpaceGameEngine::CommonParser::ParserError>>;
}

SpaceGameEngine::CommonParser::Lexer::BaseContext::BaseContext(const String& str)
	: m_Iter(str.GetConstBegin()), m_EndIter(str.GetConstEnd())
{
}

Char SpaceGameEngine::CommonParser::Lexer::BaseContext::GetCurrentChar() const
{
	if (!IsEnd())
		return *m_Iter;
	else
		return 0;
}

bool SpaceGameEngine::CommonParser::Lexer::BaseContext::IsEnd() const
{
	return m_Iter == m_EndIter;
}

const Vector<Token>& SpaceGameEngine::CommonParser::Lexer::BaseContext::GetTokens() const
{
	return m_Tokens;
}

const Vector<ParserError>& SpaceGameEngine::CommonParser::Lexer::BaseContext::GetErrors() const
{
	return m_Errors;
}

void BaseContext::Advance()
{
	SGE_CHECK(TouchInputStringEndError, m_Iter, m_EndIter);
	m_Buffer += *m_Iter;
	++m_Iter;
	++m_Column;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::Append(Char c)
{
	m_Buffer += c;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::Skip()
{
	SGE_CHECK(TouchInputStringEndError, m_Iter, m_EndIter);
	++m_Iter;
	++m_Column;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::Clear()
{
	m_Buffer.Clear();
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::NextLine()
{
	++m_Line;
	m_Column = 1;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::SetTokenLineAndColumn()
{
	m_BufferLine = m_Line;
	m_BufferColumn = m_Column;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::AddOffsetToTokenLine(Int64 offset)
{
	m_BufferLine += offset;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::AddOffsetToTokenColumn(Int64 offset)
{
	m_BufferColumn += offset;
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::Submit(TokenType token_type)
{
	m_Tokens.EmplaceBack(token_type, std::move(m_Buffer), m_BufferLine, m_BufferColumn);
	Clear();
}

void SpaceGameEngine::CommonParser::Lexer::BaseContext::Throw(SizeType error_type_id, Vector<String>&& additional_information)
{
	m_Errors.EmplaceBack(ParserError(error_type_id, m_Line, m_Column, std::move(additional_information)));
}

bool SpaceGameEngine::CommonParser::Lexer::DefaultCondition::Get(Char c, const BaseContext&)
{
	return true;
}

void SpaceGameEngine::CommonParser::Lexer::EmptyAction::Run(BaseContext& context)
{
	// do nothing
}

void SpaceGameEngine::CommonParser::Lexer::AdvanceAction::Run(BaseContext& context)
{
	context.Advance();
}

void SpaceGameEngine::CommonParser::Lexer::SkipAction::Run(BaseContext& context)
{
	context.Skip();
}

void SpaceGameEngine::CommonParser::Lexer::ClearAction::Run(BaseContext& context)
{
	context.Clear();
}

void SpaceGameEngine::CommonParser::Lexer::NewLineAction::Run(BaseContext& context)
{
	context.NextLine();
}

void SpaceGameEngine::CommonParser::Lexer::SetTokenLineAndColumnAction::Run(BaseContext& context)
{
	context.SetTokenLineAndColumn();
}

bool SpaceGameEngine::CommonParser::Lexer::TouchInputStringEndError::Judge(const String::ConstIterator iter, const String::ConstIterator end)
{
	return iter == end;
}

SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::CppLikeStyleLexerContext(const String& str)
	: BaseContext(str)
{
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::SubmitRawStringPrefix()
{
	m_RawStringPrefix = std::move(m_Buffer);
	Clear();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::AdvanceRawStringSuffix()
{
	m_RawStringSuffix += GetCurrentChar();
	Skip();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::GiveUpRawStringSuffix()
{
	m_Buffer += SGE_STR(')');
	m_Buffer += m_RawStringSuffix;
	ClearRawStringSuffix();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::ClearRawStringSuffix()
{
	m_RawStringSuffix.Clear();
}

bool SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::CppLikeStyleLexerContext::IsValidRawStringSuffix() const
{
	return m_RawStringPrefix == m_RawStringSuffix;
}

bool SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::IsValidEscapeCharacterCondition::Get(Char c, const CppLikeStyleLexerContext& context)
{
	return EscapeCharacterSet::GetSingleton().IsEscapeCharacter(c);
}

bool SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::IsValidRawStringSuffixCondition::Get(Char c, const CppLikeStyleLexerContext& context)
{
	return context.IsValidRawStringSuffix();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::SubmitRawStringPrefixAction::Run(CppLikeStyleLexerContext& context)
{
	context.SubmitRawStringPrefix();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::AdvanceRawStringSuffixAction::Run(CppLikeStyleLexerContext& context)
{
	context.AdvanceRawStringSuffix();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::GiveUpRawStringSuffixAction::Run(CppLikeStyleLexerContext& context)
{
	context.GiveUpRawStringSuffix();
}

void SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::Detail::ClearRawStringSuffixAction::Run(CppLikeStyleLexerContext& context)
{
	context.ClearRawStringSuffix();
}

COMMON_PARSER_API Pair<Vector<Token>, Vector<ParserError>> SpaceGameEngine::CommonParser::Lexer::CppLikeStyleLexer::GetTokens(const String& source)
{
	using namespace CppLikeStyleLexer::Detail;
	return Lexer::GetTokens<CppLikeStyleLexerContext,
							SGE_STR("IdleState"),
							SGE_STR("IdleState"),
							IdleState,
							IdentifierState,
							LFLineSeparatorState,
							CRLineSeparatorState,
							WordSeparatorState,
							ZeroPrefixState,
							DecimalIntegerState,
							BinaryIntegerState,
							HexIntegerState,
							DoubleDotState,
							DoubleState,
							CharacterBeginState,
							CharacterEndState,
							EscapeCharacterState,
							StringState,
							StringEscapeCharacterState,
							RawPrefixState,
							RawStringBeginState,
							RawStringState,
							RawStringEndState,
							SlashPrefixState,
							CommentBlockState,
							CommentBlockCRLineBreakState,
							CommentBlockLFLineBreakState,
							CommentBlockEndState,
							CommentLineState>(source);
}
