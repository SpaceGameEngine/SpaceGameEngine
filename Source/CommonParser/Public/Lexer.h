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
#include "SGEString.hpp"
#include "Utility/Singleton.hpp"
#include "Container/HashMap.hpp"
#include "Container/Vector.hpp"
#include "CommonParserAPI.h"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Lexer
{
	using TokenType = UInt8;

	namespace TokenTypes
	{
		inline constexpr const TokenType Unknown = 0;
		inline constexpr const TokenType Identifier = 1;
		inline constexpr const TokenType IntegerLiteral = 2;
		inline constexpr const TokenType FloatLiteral = 3;
		inline constexpr const TokenType DoubleLiteral = 4;
		inline constexpr const TokenType CharacterLiteral = 5;
		inline constexpr const TokenType StringLiteral = 6;
		inline constexpr const TokenType LineSeparator = 7;
		inline constexpr const TokenType WordSeparator = 8;
		inline constexpr const TokenType Exclamation = 9;			 //!
		inline constexpr const TokenType Hash = 10;					 // #
		inline constexpr const TokenType Dollar = 11;				 //$
		inline constexpr const TokenType Mod = 12;					 //%
		inline constexpr const TokenType And = 13;					 //&
		inline constexpr const TokenType LeftBracket = 14;			 //(
		inline constexpr const TokenType RightBracket = 15;			 //)
		inline constexpr const TokenType Multiply = 16;				 //*
		inline constexpr const TokenType Add = 17;					 //+
		inline constexpr const TokenType Comma = 18;				 //,
		inline constexpr const TokenType Subtract = 19;				 //-
		inline constexpr const TokenType Dot = 20;					 //.
		inline constexpr const TokenType Slash = 21;				 ///
		inline constexpr const TokenType Colon = 22;				 //:
		inline constexpr const TokenType Semicolon = 23;			 //;
		inline constexpr const TokenType Less = 24;					 //<
		inline constexpr const TokenType Equal = 25;				 //=
		inline constexpr const TokenType Greater = 26;				 //>
		inline constexpr const TokenType Question = 27;				 //?
		inline constexpr const TokenType At = 28;					 //@
		inline constexpr const TokenType LeftSquareBracket = 29;	 //[
		inline constexpr const TokenType Backslash = 30;			 /*\*/
		inline constexpr const TokenType RightSquareBracket = 31;	 //]
		inline constexpr const TokenType Caret = 32;				 //^
		inline constexpr const TokenType LeftCurlyBracket = 33;		 //{
		inline constexpr const TokenType Vertical = 34;				 //|
		inline constexpr const TokenType RightCurlyBracket = 35;	 //}
		inline constexpr const TokenType Tilde = 36;				 //~
		inline constexpr const TokenType Quote = 37;				 //`
		inline constexpr const TokenType CommentLine = 38;
		inline constexpr const TokenType CommentBlock = 39;
		inline constexpr const TokenType DefaultTokenTypesCount = 40;
	};

	struct InvalidTokenTypeError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The TokenType is invalid.");
		static COMMON_PARSER_API bool Judge(TokenType tt);
	};

	class COMMON_PARSER_API Token
	{
	public:
		Token();
		Token(TokenType token_type, const String& str, SizeType line, SizeType column);

		TokenType GetType() const;
		const String& GetContent() const;
		SizeType GetLine() const;
		SizeType GetColumn() const;

		bool operator==(const Token& token) const;
		bool operator!=(const Token& token) const;

	private:
		TokenType m_Type;
		String m_Content;
		SizeType m_Line;
		SizeType m_Column;
	};

	class COMMON_PARSER_API SymbolSet : public UncopyableAndUnmovable, public Singleton<SymbolSet>
	{
	private:
		SymbolSet();

	public:
		friend DefaultAllocator;

		TokenType Get(Char c) const;
		bool IsSymbol(Char c) const;

	private:
		HashMap<Char, TokenType> m_Content;
	};

	class COMMON_PARSER_API EscapeCharacterSet : public UncopyableAndUnmovable, public Singleton<EscapeCharacterSet>
	{
	private:
		EscapeCharacterSet();

	public:
		friend DefaultAllocator;

		Char Translate(Char c) const;
		bool IsEscapeCharacter(Char c) const;

	private:
		HashMap<Char, Char> m_Content;
	};

	struct InvalidEscapeCharacterError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The escape character is invalid.");
		static COMMON_PARSER_API bool Judge(Char c);
	};

	using StateType = UInt8;

	namespace State
	{
		inline constexpr const StateType Start = 0;
		inline constexpr const StateType Identifier = 1;
		inline constexpr const StateType LineSeparator = 2;
		inline constexpr const StateType WordSeparator = 3;
		inline constexpr const StateType ZeroPrefix = 4;
		inline constexpr const StateType DecimalInteger = 5;
		inline constexpr const StateType BinaryInteger = 6;
		inline constexpr const StateType HexInteger = 7;
		inline constexpr const StateType DoubleDot = 8;
		inline constexpr const StateType Double = 9;
		inline constexpr const StateType CharacterBegin = 10;
		inline constexpr const StateType CharacterEnd = 11;
		inline constexpr const StateType EscapeCharacter = 12;
		inline constexpr const StateType String = 13;
		inline constexpr const StateType StringEscapeCharacter = 14;
		inline constexpr const StateType RawPrefix = 15;
		inline constexpr const StateType RawStringBegin = 16;
		inline constexpr const StateType RawString = 17;
		inline constexpr const StateType RawStringEnd = 18;
		inline constexpr const StateType SlashPrefix = 19;
		inline constexpr const StateType CommentBlock = 20;
		inline constexpr const StateType CommentBlockEnd = 21;
		inline constexpr const StateType CommentLine = 22;
	}
	inline constexpr const SizeType StateSize = 23;

	class COMMON_PARSER_API StateMachineForJudge : public UncopyableAndUnmovable, public Singleton<StateMachineForJudge>
	{

	public:
		friend DefaultAllocator;

		/*!
		@param str string which will be judged
		@param error_info_formatter format string which likes "In line:{} column:{}, {}"
		*/
		bool Judge(const String& str, const String& error_info_formatter) const;

	private:
		struct AdditionalContextForJudge
		{
			String m_RawStringPrefix;
			String m_RawStringSuffix;
		};

		using OtherCharacterJudgeFunctionType = bool (*)(String::ConstIterator&, StateType&, const String&, SizeType, SizeType, AdditionalContextForJudge&);

	private:
		StateMachineForJudge();

	private:
		HashMap<Char, StateType> m_States[StateSize];
		OtherCharacterJudgeFunctionType m_OtherCharacterJudgeFunctions[StateSize];
	};

	struct InvalidSourceStringError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The Source string is invalid.");
		static COMMON_PARSER_API bool Judge(const String& str, const String& error_info_formatter);
	};

	enum class StateMachineControlSignal : UInt8
	{
		Forward = 0,
		Stay = 1,
		Skip = 2,
		Submit = 3,
		SubmitSymbol = 4,
		PartialSubmitLineSeparator = 5,
		SubmitSkip = 6,
		EscapeCharacter = 7,
		Clear = 8,
		RawStringPrefixSubmit = 9,
		RawStringSuffixForward = 10,
		RawStringSuffixClear = 11,
		RawStringSuffixSubmit = 12,
		CommentBlockEndBack = 13,
	};

	struct COMMON_PARSER_API StateTransfer
	{
		StateType m_NextState;
		StateMachineControlSignal m_Signal;
		TokenType m_TokenType;

		StateTransfer();
		StateTransfer(StateType next_state, StateMachineControlSignal sign, TokenType token_type);

		bool operator==(const StateTransfer& st) const;
		bool operator!=(const StateTransfer& st) const;
	};
}

namespace SpaceGameEngine
{
	extern template class COMMON_PARSER_API_TEMPLATE_DECLARE Vector<SpaceGameEngine::CommonParser::Lexer::Token>;
}

namespace SpaceGameEngine::CommonParser::Lexer
{
	class COMMON_PARSER_API StateMachine : public UncopyableAndUnmovable, public Singleton<StateMachine>
	{
	public:
		friend DefaultAllocator;

		/*!
		@brief Get tokens by giving string.
		@warning The giving string need to be checked before invoking this function.
		*/
		Vector<Token> Run(const String& str) const;

	private:
		StateMachine();

	private:
		HashMap<Char, StateTransfer> m_States[StateSize];
		StateTransfer m_OtherCharacterStates[StateSize];
	};

	/*!
	@brief Get tokens by giving string.
	@param error_info_formatter format string which likes "In line:{} column:{}, {}"
	*/
	COMMON_PARSER_API Vector<Token> GetTokens(const String& str, const String& error_info_formatter);

}

/*!
@}
*/