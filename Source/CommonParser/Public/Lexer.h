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
#pragma once
#include "CommonParserAPI.h"
#include "SGEString.hpp"
#include "Utility/Singleton.hpp"
#include "Container/HashMap.hpp"
#include "Container/Vector.hpp"
#include "ParserError.h"

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

	namespace Experimental
	{
		struct TouchInputStringEndError
		{
			inline static const ErrorMessageChar pContent[] = SGE_ESTR("Already touched the end of the input string.");
			static COMMON_PARSER_API bool Judge(const String::ConstIterator iter, const String::ConstIterator end);
		};

		class COMMON_PARSER_API BaseContext : public UncopyableAndUnmovable
		{
		public:
			BaseContext(const String& str);

			/*!
			@brief Get the current character. If the end of the input string is already touched, it will return 0.
			*/
			Char GetCurrentChar() const;

			bool IsEnd() const;

			const Vector<Token>& GetTokens() const;

			const Vector<ParserError>& GetErrors() const;

			/*!
			@brief Advance the iterator, update column number and add the character to the buffer.
			*/
			void Advance();

			/*!
			@brief add the character to the buffer but do not advance the iterator or update column number.
			*/
			void Append(Char c);

			/*!
			@brief Advance the iterator, update column number but do not add the character to the buffer.
			*/
			void Skip();

			/*!
			@brief Clear the buffer.
			*/
			void Clear();

			/*!
			@brief Update line and column number to next line.
			*/
			void NextLine();

			void Submit(TokenType token_type);

			void Throw(SizeType error_type_id, Vector<String>&& additional_information = Vector<String>());

		private:
			SizeType m_Line = 1;
			SizeType m_Column = 1;
			SizeType m_BufferLine = 1;
			SizeType m_BufferColumn = 1;
			String::ConstIterator m_Iter;
			String::ConstIterator m_EndIter;
			String m_Buffer;
			Vector<Token> m_Tokens;
			Vector<ParserError> m_Errors;
		};

		template<typename T>
		concept IsContext = std::derived_from<T, BaseContext>;

		template<typename T>
		concept IsCondition = requires(T t, Char c) {
			{
				T::Get(c)
			} -> std::same_as<bool>;
		};

		template<ArrayLiteral Chars>
		struct MatchCharsCondition
		{
			inline static bool Get(Char c)
			{
				for (SizeType i = 0; i < decltype(Chars)::Size - 1; ++i)	// avoid the last \0
				{
					if (c == Chars.m_Value[i])
						return true;
				}
				return false;
			}
		};

		template<Char StartChar, Char EndChar>
		struct MatchCharRangeCondition
		{
			inline static bool Get(Char c)
			{
				return c >= StartChar && c <= EndChar;
			}
		};

		template<IsCondition Condition>
		struct NegateCondition
		{
			inline static bool Get(Char c)
			{
				return !Condition::Get(c);
			}
		};

		template<IsCondition... Conditions>
		struct OrCondition
		{
			inline static bool Get(Char c)
			{
				return (... || Conditions::Get(c));
			}
		};

		template<IsCondition... Conditions>
		struct AndCondition
		{
			inline static bool Get(Char c)
			{
				return (... && Conditions::Get(c));
			}
		};

		/*!
		@brief This condition is used to match characters which are not in the other conditions. It should always be placed at last.
		*/
		struct COMMON_PARSER_API DefaultCondition
		{
			static bool Get(Char c);
		};

		template<typename T, typename ContextType>
		concept IsAction = IsContext<ContextType> && requires(ContextType& context) {
			{
				T::Run(context)
			} -> std::same_as<void>;
		};

		struct COMMON_PARSER_API EmptyAction
		{
			static void Run(BaseContext& context);
		};

		template<IsContext ContextType, IsAction<ContextType>... ActionTypes>
		struct ChainAction
		{
			inline static void Run(ContextType& context)
			{
				(..., ActionTypes::Run(context));
			}
		};

		struct AdvanceAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.Advance();
			}
		};

		template<ArrayLiteral Chars>
		struct AppendAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				for (SizeType i = 0; i < decltype(Chars)::Size - 1; ++i)	// avoid the last \0
					context.Append(Chars.m_Value[i]);
			}
		};

		struct SkipAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.Skip();
			}
		};

		struct ClearAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.Clear();
			}
		};

		struct NewLineAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.NextLine();
			}
		};

		template<TokenType TokenTypeValue>
		struct SubmitAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.Submit(TokenTypeValue);
			}
		};

		template<SizeType ErrorTypeId>
		struct ThrowAction
		{
			template<typename ContextType>
			inline static void Run(ContextType& context)
			{
				context.Throw(ErrorTypeId);
			}
		};

		template<IsContext ContextType, IsCondition ConditionType, IsAction<ContextType> ActionType, ArrayLiteral _NextStateName>
		struct Transition
		{
			using Context = ContextType;
			using Condition = ConditionType;
			using Action = ActionType;
			inline static constexpr const auto NextStateName = _NextStateName;
		};

		template<typename T, typename ContextType>
		concept IsTransition = IsContext<ContextType> && requires {
			IsContext<typename T::Context>;
			std::is_convertible_v<ContextType, typename T::Context>;
			IsCondition<typename T::Condition>;
			IsAction<typename T::Action, typename T::Context>;
			{
				T::NextStateName.m_Value
			} -> std::convertible_to<const Char*>;
		};

		template<typename T>
		concept IsStateNameResolver = requires {
			{
				T::template Get<SGE_STR("")>()
			} -> std::same_as<SizeType>;
		};

		struct EmptyStateNameResolver
		{
			template<ArrayLiteral Name>
			inline static constexpr SizeType Get()
			{
				return static_cast<SizeType>(-1);
			}
		};

		template<IsContext ContextType, ArrayLiteral _Name, IsTransition<ContextType>... Transitions>
		struct State
		{
			inline static constexpr const auto Name = _Name;

			static_assert(sizeof...(Transitions) > 0, "State must have at least one Transition.");

			inline static constexpr const SizeType TransitionCount = sizeof...(Transitions);
			using TransitionConditionType = bool (*)(Char);
			inline static constexpr const TransitionConditionType TransitionConditions[TransitionCount] = {Transitions::Condition::Get...};
			using TransitionActionType = void (*)(ContextType&);
			inline static constexpr const TransitionActionType TransitionActions[TransitionCount] = {Transitions::Action::Run...};

			template<IsStateNameResolver StateNameResolver>
			inline static SizeType Accept(ContextType& context)
			{
				static constinit const SizeType NextStateNameIndices[TransitionCount] = {StateNameResolver::template Get<Transitions::NextStateName>()...};
				Char c = context.GetCurrentChar();
				for (SizeType i = 0; i < TransitionCount; ++i)
				{
					if (TransitionConditions[i](c))
					{
						TransitionActions[i](context);
						return NextStateNameIndices[i];
					}
				}
				// todo error handle
			}
		};

		template<typename T, typename ContextType>
		concept IsState = IsContext<ContextType> && requires(ContextType& context) {
			{
				T::Name.m_Value
			} -> std::convertible_to<const Char*>;
			{
				T::template Accept<EmptyStateNameResolver>(context)
			} -> std::same_as<SizeType>;
		};

		template<auto... _StateNames>
		struct StateNameResolver
		{
			template<ArrayLiteral _Name>
			inline static constexpr SizeType Get()
			{
				constexpr SizeType index = []() constexpr -> SizeType {
					const bool matches[] = {IsSameCString(_Name.m_Value, _StateNames.m_Value)...};
					for (SizeType i = 0; i < sizeof...(_StateNames); ++i)
					{
						if (matches[i])
							return i;
					}
					return static_cast<SizeType>(-1);
				}();
				static_assert(index != static_cast<SizeType>(-1), "State name not found.");
				return index;
			}
		};

		template<typename ContextType, ArrayLiteral StartStateName, ArrayLiteral EndStateName, IsState<ContextType>... States>
		inline Vector<Token> GetTokens(const String& str)
		{
			using StateNameResolverType = StateNameResolver<States::Name...>;
			static constexpr const SizeType StartStateIndex = StateNameResolverType::template Get<StartStateName>();
			static constexpr const SizeType EndStateIndex = StateNameResolverType::template Get<EndStateName>();
			using StateAcceptorType = SizeType (*)(ContextType&);
			static constexpr const StateAcceptorType StateAcceptors[sizeof...(States)] = {States::template Accept<StateNameResolverType>...};

			ContextType context(str);
			SizeType state_id = StartStateIndex;
			while (state_id != EndStateIndex || (!context.IsEnd()))
				state_id = StateAcceptors[state_id](context);
			return context.GetTokens();
		}

		namespace CppLikeStyleLexer
		{
			namespace Detail
			{
				class COMMON_PARSER_API CppLikeStyleLexerContext : public BaseContext
				{
				};

				struct COMMON_PARSER_API IsValidEscapeCharacterCondition
				{
					static bool Get(Char c);
				};

				struct AdvanceEscapeCharacterAction
				{
					template<typename ContextType>
					inline static void Run(ContextType& context)
					{
						context.Append(EscapeCharacterSet::GetInstance().Translate(context.GetCurrentChar()));
						context.Skip();
					}
				};

				namespace ErrorTypeId
				{
					inline constexpr const SizeType InvalidCharacter = 1;	 // todo make this divided into much more detailed errors
				}

				using IdleToIdentifierTransition = Transition<CppLikeStyleLexerContext,
															  OrCondition<
																  MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>,
																  AndCondition<
																	  MatchCharRangeCondition<SGE_STR('A'), SGE_STR('Z')>,
																	  NegateCondition<MatchCharsCondition<SGE_STR("R")>>>,
																  MatchCharsCondition<SGE_STR("_")>>,
															  AdvanceAction,
															  SGE_STR("IdentifierState")>;
				using IdleToDecimalIntegerTransition = Transition<CppLikeStyleLexerContext,
																  MatchCharRangeCondition<SGE_STR('1'), SGE_STR('9')>,
																  AdvanceAction,
																  SGE_STR("DecimalIntegerState")>;
				using IdleToZeroPrefixTransition = Transition<CppLikeStyleLexerContext,
															  MatchCharsCondition<SGE_STR("0")>,
															  AdvanceAction,
															  SGE_STR("ZeroPrefixState")>;
				using IdleToCharacterBeginTransition = Transition<CppLikeStyleLexerContext,
																  MatchCharsCondition<SGE_STR("'")>,
																  SkipAction,
																  SGE_STR("CharacterBeginState")>;
				using IdleToStringTransition = Transition<CppLikeStyleLexerContext,
														  MatchCharsCondition<SGE_STR("\"")>,
														  SkipAction,
														  SGE_STR("StringState")>;
				using IdleToRawPrefixTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR("R")>,
															 AdvanceAction,
															 SGE_STR("RawPrefixState")>;
				using IdleToLFLineSeparatorTransition = Transition<CppLikeStyleLexerContext,
																   MatchCharsCondition<SGE_STR("\n")>,
																   AdvanceAction,
																   SGE_STR("LFLineSeparatorState")>;
				using IdleToCRLineSeparatorTransition = Transition<CppLikeStyleLexerContext,
																   MatchCharsCondition<SGE_STR("\r")>,
																   AdvanceAction,
																   SGE_STR("CRLineSeparatorState")>;
				using IdleToWordSeparatorTransition = Transition<CppLikeStyleLexerContext,
																 MatchCharsCondition<SGE_STR(" \t")>,
																 AdvanceAction,
																 SGE_STR("WordSeparatorState")>;
				using IdleToSlashPrefixTransition = Transition<CppLikeStyleLexerContext,
															   MatchCharsCondition<SGE_STR("/")>,
															   AdvanceAction,
															   SGE_STR("SlashPrefixState")>;
				using IdleSubmitExclamationTransition = Transition<CppLikeStyleLexerContext,
																   MatchCharsCondition<SGE_STR("!")>,
																   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Exclamation>>,
																   SGE_STR("IdleState")>;
				using IdleSubmitHashTransition = Transition<CppLikeStyleLexerContext,
															MatchCharsCondition<SGE_STR("#")>,
															ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Hash>>,
															SGE_STR("IdleState")>;
				using IdleSubmitDollarTransition = Transition<CppLikeStyleLexerContext,
															  MatchCharsCondition<SGE_STR("$")>,
															  ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Dollar>>,
															  SGE_STR("IdleState")>;
				using IdleSubmitModTransition = Transition<CppLikeStyleLexerContext,
														   MatchCharsCondition<SGE_STR("%")>,
														   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Mod>>,
														   SGE_STR("IdleState")>;
				using IdleSubmitAndTransition = Transition<CppLikeStyleLexerContext,
														   MatchCharsCondition<SGE_STR("&")>,
														   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::And>>,
														   SGE_STR("IdleState")>;
				using IdleSubmitLeftBracketTransition = Transition<CppLikeStyleLexerContext,
																   MatchCharsCondition<SGE_STR("(")>,
																   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::LeftBracket>>,
																   SGE_STR("IdleState")>;
				using IdleSubmitRightBracketTransition = Transition<CppLikeStyleLexerContext,
																	MatchCharsCondition<SGE_STR(")")>,
																	ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::RightBracket>>,
																	SGE_STR("IdleState")>;
				using IdleSubmitMultiplyTransition = Transition<CppLikeStyleLexerContext,
																MatchCharsCondition<SGE_STR("*")>,
																ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Multiply>>,
																SGE_STR("IdleState")>;
				using IdleSubmitAddTransition = Transition<CppLikeStyleLexerContext,
														   MatchCharsCondition<SGE_STR("+")>,
														   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Add>>,
														   SGE_STR("IdleState")>;
				using IdleSubmitCommaTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR(",")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Comma>>,
															 SGE_STR("IdleState")>;
				using IdleSubmitSubtractTransition = Transition<CppLikeStyleLexerContext,
																MatchCharsCondition<SGE_STR("-")>,
																ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Subtract>>,
																SGE_STR("IdleState")>;
				using IdleSubmitDotTransition = Transition<CppLikeStyleLexerContext,
														   MatchCharsCondition<SGE_STR(".")>,
														   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Dot>>,
														   SGE_STR("IdleState")>;
				using IdleSubmitColonTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR(":")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Colon>>,
															 SGE_STR("IdleState")>;
				using IdleSubmitSemicolonTransition = Transition<CppLikeStyleLexerContext,
																 MatchCharsCondition<SGE_STR(";")>,
																 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Semicolon>>,
																 SGE_STR("IdleState")>;
				using IdleSubmitLessTransition = Transition<CppLikeStyleLexerContext,
															MatchCharsCondition<SGE_STR("<")>,
															ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Less>>,
															SGE_STR("IdleState")>;
				using IdleSubmitEqualTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR("=")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Equal>>,
															 SGE_STR("IdleState")>;
				using IdleSubmitGreaterTransition = Transition<CppLikeStyleLexerContext,
															   MatchCharsCondition<SGE_STR(">")>,
															   ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Greater>>,
															   SGE_STR("IdleState")>;
				using IdleSubmitQuestionTransition = Transition<CppLikeStyleLexerContext,
																MatchCharsCondition<SGE_STR("?")>,
																ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Question>>,
																SGE_STR("IdleState")>;
				using IdleSubmitAtTransition = Transition<CppLikeStyleLexerContext,
														  MatchCharsCondition<SGE_STR("@")>,
														  ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::At>>,
														  SGE_STR("IdleState")>;
				using IdleSubmitLeftSquareBracketTransition = Transition<CppLikeStyleLexerContext,
																		 MatchCharsCondition<SGE_STR("[")>,
																		 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::LeftSquareBracket>>,
																		 SGE_STR("IdleState")>;
				using IdleSubmitBackslashTransition = Transition<CppLikeStyleLexerContext,
																 MatchCharsCondition<SGE_STR("\\")>,
																 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Backslash>>,
																 SGE_STR("IdleState")>;
				using IdleSubmitRightSquareBracketTransition = Transition<CppLikeStyleLexerContext,
																		  MatchCharsCondition<SGE_STR("]")>,
																		  ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::RightSquareBracket>>,
																		  SGE_STR("IdleState")>;
				using IdleSubmitCaretTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR("^")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Caret>>,
															 SGE_STR("IdleState")>;
				using IdleSubmitLeftCurlyBracketTransition = Transition<CppLikeStyleLexerContext,
																		MatchCharsCondition<SGE_STR("{")>,
																		ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::LeftCurlyBracket>>,
																		SGE_STR("IdleState")>;
				using IdleSubmitVerticalTransition = Transition<CppLikeStyleLexerContext,
																MatchCharsCondition<SGE_STR("|")>,
																ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Vertical>>,
																SGE_STR("IdleState")>;
				using IdleSubmitRightCurlyBracketTransition = Transition<CppLikeStyleLexerContext,
																		 MatchCharsCondition<SGE_STR("}")>,
																		 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::RightCurlyBracket>>,
																		 SGE_STR("IdleState")>;
				using IdleSubmitTildeTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR("~")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Tilde>>,
															 SGE_STR("IdleState")>;
				using IdleSubmitQuoteTransition = Transition<CppLikeStyleLexerContext,
															 MatchCharsCondition<SGE_STR("`")>,
															 ChainAction<CppLikeStyleLexerContext, AdvanceAction, SubmitAction<TokenTypes::Quote>>,
															 SGE_STR("IdleState")>;
				using IdleInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																  DefaultCondition,
																  ChainAction<CppLikeStyleLexerContext, SkipAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																  SGE_STR("IdleState")>;
				using IdentifierToIdentifierTransition = Transition<CppLikeStyleLexerContext,
																	OrCondition<
																		MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>,
																		MatchCharRangeCondition<SGE_STR('A'), SGE_STR('Z')>,
																		MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
																		MatchCharsCondition<SGE_STR("_")>>,
																	AdvanceAction,
																	SGE_STR("IdentifierState")>;
				using IdentifierSubmitTransition = Transition<CppLikeStyleLexerContext,
															  DefaultCondition,
															  SubmitAction<TokenTypes::Identifier>,
															  SGE_STR("IdleState")>;
				using LFLineSeparatorSubmitTransition = Transition<CppLikeStyleLexerContext,
																   DefaultCondition,
																   ChainAction<CppLikeStyleLexerContext, SubmitAction<TokenTypes::LineSeparator>, NewLineAction>,
																   SGE_STR("IdleState")>;
				using CRLineSeparatorToLFLineSeparatorTransition = Transition<CppLikeStyleLexerContext,
																			  MatchCharsCondition<SGE_STR("\n")>,
																			  AdvanceAction,
																			  SGE_STR("LFLineSeparatorState")>;
				using CRLineSeparatorSubmitTransition = Transition<CppLikeStyleLexerContext,
																   DefaultCondition,
																   ChainAction<CppLikeStyleLexerContext, SubmitAction<TokenTypes::LineSeparator>, NewLineAction>,
																   SGE_STR("IdleState")>;
				using WordSeparatorToWordSeparatorTransition = Transition<CppLikeStyleLexerContext,
																		  MatchCharsCondition<SGE_STR(" \t")>,
																		  AdvanceAction,
																		  SGE_STR("WordSeparatorState")>;
				using WordSeparatorSubmitTransition = Transition<CppLikeStyleLexerContext,
																 DefaultCondition,
																 SubmitAction<TokenTypes::WordSeparator>,
																 SGE_STR("IdleState")>;
				using ZeroPrefixToDecimalIntegerTransition = Transition<CppLikeStyleLexerContext,
																		MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
																		AdvanceAction,
																		SGE_STR("DecimalIntegerState")>;
				using ZeroPrefixToBinaryIntegerTransition = Transition<CppLikeStyleLexerContext,
																	   MatchCharsCondition<SGE_STR("b")>,
																	   AdvanceAction,
																	   SGE_STR("BinaryIntegerState")>;
				using ZeroPrefixToHexIntegerTransition = Transition<CppLikeStyleLexerContext,
																	MatchCharsCondition<SGE_STR("x")>,
																	AdvanceAction,
																	SGE_STR("HexIntegerState")>;
				using ZeroPrefixSubmitTransition = Transition<CppLikeStyleLexerContext,
															  DefaultCondition,
															  SubmitAction<TokenTypes::IntegerLiteral>,
															  SGE_STR("IdleState")>;
				using DecimalIntegerToDecimalIntegerTransition = Transition<CppLikeStyleLexerContext,
																			MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
																			AdvanceAction,
																			SGE_STR("DecimalIntegerState")>;
				using DecimalIntegerToDoubleDotTransition = Transition<CppLikeStyleLexerContext,
																	   MatchCharsCondition<SGE_STR(".")>,
																	   AdvanceAction,
																	   SGE_STR("DoubleDotState")>;
				using DecimalIntegerSubmitTransition = Transition<CppLikeStyleLexerContext,
																  DefaultCondition,
																  SubmitAction<TokenTypes::IntegerLiteral>,
																  SGE_STR("IdleState")>;
				using BinaryIntegerToBinaryIntegerTransition = Transition<CppLikeStyleLexerContext,
																		  MatchCharsCondition<SGE_STR("01")>,
																		  AdvanceAction,
																		  SGE_STR("BinaryIntegerState")>;
				using BinaryIntegerInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																		   MatchCharRangeCondition<SGE_STR('2'), SGE_STR('9')>,
																		   ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																		   SGE_STR("IdleState")>;
				using BinaryIntegerSubmitTransition = Transition<CppLikeStyleLexerContext,
																 DefaultCondition,
																 SubmitAction<TokenTypes::IntegerLiteral>,
																 SGE_STR("IdleState")>;
				using HexIntegerToHexIntegerTransition = Transition<CppLikeStyleLexerContext,
																	OrCondition<
																		MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
																		MatchCharRangeCondition<SGE_STR('a'), SGE_STR('f')>,
																		MatchCharRangeCondition<SGE_STR('A'), SGE_STR('F')>>,
																	AdvanceAction,
																	SGE_STR("HexIntegerState")>;
				using HexIntegerSubmitTransition = Transition<CppLikeStyleLexerContext,
															  DefaultCondition,
															  SubmitAction<TokenTypes::IntegerLiteral>,
															  SGE_STR("IdleState")>;
				using DoubleDotToDoubleTransition = Transition<CppLikeStyleLexerContext,
															   MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
															   AdvanceAction,
															   SGE_STR("DoubleState")>;
				using DoubleDotInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																	   DefaultCondition,
																	   ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																	   SGE_STR("IdleState")>;
				using DoubleToDoubleTransition = Transition<CppLikeStyleLexerContext,
															MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>,
															AdvanceAction,
															SGE_STR("DoubleState")>;
				using DoubleSubmitFloatTransition = Transition<CppLikeStyleLexerContext,
															   MatchCharsCondition<SGE_STR("f")>,
															   ChainAction<CppLikeStyleLexerContext, SubmitAction<TokenTypes::FloatLiteral>, SkipAction>,
															   SGE_STR("IdleState")>;
				using DoubleSubmitDoubleTransition = Transition<CppLikeStyleLexerContext,
																DefaultCondition,
																SubmitAction<TokenTypes::DoubleLiteral>,
																SGE_STR("IdleState")>;
				using CharacterBeginToEscapeCharacterTransition = Transition<CppLikeStyleLexerContext,
																			 MatchCharsCondition<SGE_STR("\\")>,
																			 SkipAction,
																			 SGE_STR("EscapeCharacterState")>;
				using CharacterBeginInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																			MatchCharsCondition<SGE_STR("'")>,
																			ChainAction<CppLikeStyleLexerContext, SkipAction, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																			SGE_STR("IdleState")>;
				using CharacterBeginToCharacterEndTransition = Transition<CppLikeStyleLexerContext,
																		  DefaultCondition,
																		  AdvanceAction,
																		  SGE_STR("CharacterEndState")>;
				using CharacterEndSubmitTransition = Transition<CppLikeStyleLexerContext,
																MatchCharsCondition<SGE_STR("'")>,
																ChainAction<CppLikeStyleLexerContext, SubmitAction<TokenTypes::CharacterLiteral>, SkipAction>,
																SGE_STR("IdleState")>;
				using CharacterEndInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																		  DefaultCondition,
																		  ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																		  SGE_STR("IdleState")>;
				using EscapeCharacterToCharacterEndTransition = Transition<CppLikeStyleLexerContext,
																		   IsValidEscapeCharacterCondition,
																		   AdvanceEscapeCharacterAction,
																		   SGE_STR("CharacterEndState")>;
				using EscapeCharacterInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																			 DefaultCondition,
																			 ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																			 SGE_STR("IdleState")>;
				using StringToStringEscapeCharacter = Transition<CppLikeStyleLexerContext,
																 MatchCharsCondition<SGE_STR("\\")>,
																 SkipAction,
																 SGE_STR("StringEscapeCharacterState")>;
				using StringSubmitTransition = Transition<CppLikeStyleLexerContext,
														  MatchCharsCondition<SGE_STR("\"")>,
														  ChainAction<CppLikeStyleLexerContext, SubmitAction<TokenTypes::StringLiteral>, SkipAction>,
														  SGE_STR("IdleState")>;
				using StringInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																	MatchCharsCondition<SGE_STR("\r\n")>,
																	ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																	SGE_STR("IdleState")>;
				using StringToStringTransition = Transition<CppLikeStyleLexerContext,
															DefaultCondition,
															AdvanceAction,
															SGE_STR("StringState")>;
				using StringEscapeCharacterToStringTransition = Transition<CppLikeStyleLexerContext,
																		   IsValidEscapeCharacterCondition,
																		   AdvanceEscapeCharacterAction,
																		   SGE_STR("StringState")>;
				using StringEscapeCharacterInvalidCharacterTransition = Transition<CppLikeStyleLexerContext,
																				   DefaultCondition,
																				   ChainAction<CppLikeStyleLexerContext, ClearAction, ThrowAction<ErrorTypeId::InvalidCharacter>>,
																				   SGE_STR("IdleState")>;

				using IdleState = State<CppLikeStyleLexerContext, SGE_STR("IdleState"),
										IdleToIdentifierTransition,
										IdleToDecimalIntegerTransition,
										IdleToZeroPrefixTransition,
										IdleToCharacterBeginTransition,
										IdleToStringTransition,
										IdleToRawPrefixTransition,
										IdleToLFLineSeparatorTransition,
										IdleToCRLineSeparatorTransition,
										IdleToWordSeparatorTransition,
										IdleToSlashPrefixTransition,
										IdleSubmitExclamationTransition,
										IdleSubmitHashTransition,
										IdleSubmitDollarTransition,
										IdleSubmitModTransition,
										IdleSubmitAndTransition,
										IdleSubmitLeftBracketTransition,
										IdleSubmitRightBracketTransition,
										IdleSubmitMultiplyTransition,
										IdleSubmitAddTransition,
										IdleSubmitCommaTransition,
										IdleSubmitSubtractTransition,
										IdleSubmitDotTransition,
										IdleSubmitColonTransition,
										IdleSubmitSemicolonTransition,
										IdleSubmitLessTransition,
										IdleSubmitEqualTransition,
										IdleSubmitGreaterTransition,
										IdleSubmitQuestionTransition,
										IdleSubmitAtTransition,
										IdleSubmitLeftSquareBracketTransition,
										IdleSubmitBackslashTransition,
										IdleSubmitRightSquareBracketTransition,
										IdleSubmitCaretTransition,
										IdleSubmitLeftCurlyBracketTransition,
										IdleSubmitVerticalTransition,
										IdleSubmitRightCurlyBracketTransition,
										IdleSubmitTildeTransition,
										IdleSubmitQuoteTransition,
										IdleInvalidCharacterTransition>;
				using IdentifierState = State<CppLikeStyleLexerContext, SGE_STR("IdentifierState"),
											  IdentifierToIdentifierTransition,
											  IdentifierSubmitTransition>;
				using LFLineSeparatorState = State<CppLikeStyleLexerContext, SGE_STR("LFLineSeparatorState"),
												   LFLineSeparatorSubmitTransition>;
				using CRLineSeparatorState = State<CppLikeStyleLexerContext, SGE_STR("CRLineSeparatorState"),
												   CRLineSeparatorToLFLineSeparatorTransition,
												   CRLineSeparatorSubmitTransition>;
				using WordSeparatorState = State<CppLikeStyleLexerContext, SGE_STR("WordSeparatorState"),
												 WordSeparatorToWordSeparatorTransition,
												 WordSeparatorSubmitTransition>;
				using ZeroPrefixState = State<CppLikeStyleLexerContext, SGE_STR("ZeroPrefixState"),
											  ZeroPrefixToDecimalIntegerTransition,
											  ZeroPrefixToBinaryIntegerTransition,
											  ZeroPrefixToHexIntegerTransition,
											  ZeroPrefixSubmitTransition>;
				using DecimalIntegerState = State<CppLikeStyleLexerContext, SGE_STR("DecimalIntegerState"),
												  DecimalIntegerToDecimalIntegerTransition,
												  DecimalIntegerToDoubleDotTransition,
												  DecimalIntegerSubmitTransition>;
				using BinaryIntegerState = State<CppLikeStyleLexerContext, SGE_STR("BinaryIntegerState"),
												 BinaryIntegerToBinaryIntegerTransition,
												 BinaryIntegerInvalidCharacterTransition,
												 BinaryIntegerSubmitTransition>;
				using HexIntegerState = State<CppLikeStyleLexerContext, SGE_STR("HexIntegerState"),
											  HexIntegerToHexIntegerTransition,
											  HexIntegerSubmitTransition>;
				using DoubleDotState = State<CppLikeStyleLexerContext, SGE_STR("DoubleDotState"),
											 DoubleDotToDoubleTransition,
											 DoubleDotInvalidCharacterTransition>;
				using DoubleState = State<CppLikeStyleLexerContext, SGE_STR("DoubleState"),
										  DoubleToDoubleTransition,
										  DoubleSubmitFloatTransition,
										  DoubleSubmitDoubleTransition>;
				using CharacterBeginState = State<CppLikeStyleLexerContext, SGE_STR("CharacterBeginState"),
												  CharacterBeginToEscapeCharacterTransition,
												  CharacterBeginInvalidCharacterTransition,
												  CharacterBeginToCharacterEndTransition>;
				using CharacterEndState = State<CppLikeStyleLexerContext, SGE_STR("CharacterEndState"),
												CharacterEndSubmitTransition,
												CharacterEndInvalidCharacterTransition>;
				using EscapeCharacterState = State<CppLikeStyleLexerContext, SGE_STR("EscapeCharacterState"),
												   EscapeCharacterToCharacterEndTransition,
												   EscapeCharacterInvalidCharacterTransition>;
				using StringState = State<CppLikeStyleLexerContext, SGE_STR("StringState"),
										  StringToStringEscapeCharacter,
										  StringSubmitTransition,
										  StringInvalidCharacterTransition,
										  StringToStringTransition>;
				using StringEscapeCharacterState = State<CppLikeStyleLexerContext, SGE_STR("StringEscapeCharacterState"),
														 StringEscapeCharacterToStringTransition,
														 StringEscapeCharacterInvalidCharacterTransition>;
			}
		}
	}
}

/*!
@}
*/