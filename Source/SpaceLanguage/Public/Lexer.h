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
#include "Utility/Singleton.hpp"
#include "Container/HashMap.hpp"
#include "SpaceLanguageAPI.h"

namespace SpaceGameEngine::SpaceLanguage::Lexer
{
	/*!
	@ingroup SpaceLanguage
	@{
	*/

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
		Hash = 10,					//#
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
	};

	struct SPACE_LANGUAGE_API Token
	{
		Token();
		Token(TokenType token_type, const String& str);
		TokenType m_Type;
		String m_Content;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<Char, TokenType>;
#endif

	class SPACE_LANGUAGE_API SymbolSet : public Singleton<SymbolSet>
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

	struct InvalidSourceStringError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The Source string is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const String& src_str);
	};

	/*!
	@}
	*/
}