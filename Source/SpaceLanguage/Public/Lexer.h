/*
Copyright 2023 creatorlxd

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
#include "SpaceLanguageAPI.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::Lexer
{

	enum class TokenType : UInt8
	{
		Unknown = 0,
		Identifier = 1,
		IntegerLiteral = 2,
		FloatLiteral = 3,
		DoubleLiteral = 4,
		CharacterLiteral = 5,
		StringLiteral = 6,
		LineSeparator = 7,
		WordSeparator = 8,
		Exclamation = 9,			//!
		Hash = 10,					// #
		Dollar = 11,				//$
		Mod = 12,					//%
		And = 13,					//&
		LeftBracket = 14,			//(
		RightBracket = 15,			//)
		Multiply = 16,				//*
		Add = 17,					//+
		Comma = 18,					//,
		Subtract = 19,				//-
		Dot = 20,					//.
		Slash = 21,					///
		Colon = 22,					//:
		Semicolon = 23,				//;
		Less = 24,					//<
		Equal = 25,					//=
		Greater = 26,				//>
		Question = 27,				//?
		At = 28,					//@
		LeftSquareBracket = 29,		//[
		Backslash = 30,				/*\*/
		RightSquareBracket = 31,	//]
		Caret = 32,					//^
		LeftCurlyBracket = 33,		//{
		Vertical = 34,				//|
		RightCurlyBracket = 35,		//}
		Tilde = 36,					//~
		Quote = 37,					//`
		CommentLine = 38,
		CommentBlock = 39
	};

	struct InvalidTokenTypeError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The TokenType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(TokenType tt);
	};

	class SPACE_LANGUAGE_API Token
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

	class SPACE_LANGUAGE_API SymbolSet : public UncopyableAndUnmovable, public Singleton<SymbolSet>
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

	class SPACE_LANGUAGE_API EscapeCharacterSet : public UncopyableAndUnmovable, public Singleton<EscapeCharacterSet>
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
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The escape character is invalid.");
		static SPACE_LANGUAGE_API bool Judge(Char c);
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

	using OtherCharacterJudgeFunctionType = bool (*)(String::ConstIterator&, StateType&, const String&, SizeType, SizeType);

	class SPACE_LANGUAGE_API StateMachineForJudge : public UncopyableAndUnmovable, public Singleton<StateMachineForJudge>
	{

	public:
		friend DefaultAllocator;

		/*!
		@param str string which will be judged
		@param error_info_formatter format string which likes "In line:{} column:{}, {}"
		*/
		bool Judge(const String& str, const String& error_info_formatter) const;

	private:
		StateMachineForJudge();

	private:
		HashMap<Char, StateType> m_States[StateSize];
		OtherCharacterJudgeFunctionType m_OtherCharacterJudgeFunctions[StateSize];
	};

	struct InvalidSourceStringError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The Source string is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& str, const String& error_info_formatter);
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
		RawStringEndBack = 9,
		CommentBlockEndBack = 10,
	};

	struct SPACE_LANGUAGE_API StateTransfer
	{
		StateType m_NextState;
		StateMachineControlSignal m_Signal;
		TokenType m_TokenType;

		StateTransfer();
		StateTransfer(StateType next_state, StateMachineControlSignal sign, TokenType token_type);

		bool operator==(const StateTransfer& st) const;
		bool operator!=(const StateTransfer& st) const;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL) && (!defined(SPACE_LANGUAGE_EXPORTS))
	extern template class SGE_DLL_IMPORT Vector<Token>;
#endif

	class SPACE_LANGUAGE_API StateMachine : public UncopyableAndUnmovable, public Singleton<StateMachine>
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
	SPACE_LANGUAGE_API Vector<Token> GetTokens(const String& str, const String& error_info_formatter);

}

/*!
@}
*/