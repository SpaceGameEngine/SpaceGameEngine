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

			/*!
			@brief Advance the iterator, update column number and add the character to the buffer.
			*/
			void Advance();

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

		private:
			SizeType m_Line = 1;
			SizeType m_Column = 1;
			SizeType m_BufferLine = 1;
			SizeType m_BufferColumn = 1;
			String::ConstIterator m_Iter;
			String::ConstIterator m_EndIter;
			String::ConstIterator m_BufferBeginIter;
			String::ConstIterator m_BufferEndIter;
			Vector<Token> m_Tokens;
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
				for (Char ch : Chars.m_Value)
				{
					if (c == ch)
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

		template<IsContext ContextType, IsCondition ConditionType, IsAction<ContextType> ActionType, ArrayLiteral _NextStateName>
		struct Transition
		{
			using Context = ContextType;
			using Condition = ConditionType;
			using Action = ActionType;
			inline static constexpr const auto NextStateName = _NextStateName.m_Value;
		};

		template<typename T, typename ContextType>
		concept IsTransition = IsContext<ContextType> && requires {
			IsContext<typename T::Context>;
			std::is_convertible_v<ContextType, typename T::Context>;
			IsCondition<typename T::Condition>;
			IsAction<typename T::Action, typename T::Context>;
			{
				T::NextStateName
			} -> std::convertible_to<const Char*>;
		};

		template<typename T>
		concept IsStateNameResolver = requires {
			{
				T::template Get<(const Char*)nullptr>()
			} -> std::same_as<SizeType>;
		};

		struct EmptyStateNameResolver
		{
			template<const Char* Name>
			inline static constexpr SizeType Get()
			{
				return static_cast<SizeType>(-1);
			}
		};

		template<IsContext ContextType, ArrayLiteral _Name, IsTransition<ContextType>... Transitions>
		struct State
		{
			inline static constexpr const auto Name = _Name.m_Value;

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
				T::Name
			} -> std::convertible_to<const Char*>;
			{
				T::template Accept<EmptyStateNameResolver>(context)
			} -> std::same_as<SizeType>;
		};

		template<auto... _StateNames>
		struct StateNameResolver
		{
			inline static constexpr const Char* StateNames[sizeof...(_StateNames)] = {_StateNames...};

			template<const Char* Name>
			inline static constexpr SizeType Get()
			{
				constexpr SizeType index = []() constexpr {
					for (SizeType i = 0; i < sizeof...(_StateNames); ++i)
					{
						if (IsSameCString(Name, StateNames[i]))
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
			using StateNameResolver = StateNameResolver<States::Name...>;
			static constexpr const SizeType StartStateIndex = StateNameResolver::template Get<StartStateName.m_Value>();
			static constexpr const SizeType EndStateIndex = StateNameResolver::template Get<EndStateName.m_Value>();
			using StateAcceptorType = SizeType (*)(ContextType&);
			static constexpr const StateAcceptorType StateAcceptors[sizeof...(States)] = {States::template Accept<StateNameResolver>...};

			ContextType context(str);
			SizeType state_id = StartStateIndex;
			while (state_id != EndStateIndex || (!context.IsEnd()))
				state_id = StateAcceptors[state_id](context);
			return context.GetTokens();
		}
	}
}

/*!
@}
*/