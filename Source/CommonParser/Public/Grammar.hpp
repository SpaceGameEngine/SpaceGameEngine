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
#include "Lexer.h"
#include "Meta/TypeList.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::Grammar
{
	struct Expression
	{
		inline static constexpr const auto Name = ArrayLiteral(SGE_STR("Expression"));
	};

	template<typename T>
	concept IsExpression = std::derived_from<T, Expression> &&
							   requires()
	{
		{
			T::Name.m_Value
		} -> std::convertible_to<const Char*>;
	};

	template<Lexer::TokenType _Type>
	struct MatchTokenType : public Expression
	{
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("MatchTokenType<")), IntegerToCStringLiteral<_Type, Char>(), ArrayLiteral(SGE_STR(">")));
		inline static constexpr const Lexer::TokenType Type = _Type;
	};

	template<Lexer::TokenType _Type, ArrayLiteral _Content>
	struct MatchTokenTypeAndContent : public Expression
	{
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("MatchTokenTypeAndContent<")), IntegerToCStringLiteral<_Type, Char>(), ArrayLiteral(SGE_STR(", ")), _Content, ArrayLiteral(SGE_STR(">")));
		inline static constexpr const Lexer::TokenType Type = _Type;
		inline static constexpr const auto Content = _Content;
	};

	template<IsExpression... _Expressions>
	struct Sequence : public Expression
	{
		static_assert(sizeof...(_Expressions) > 0, "SequenceExpression must contain at least one expression.");
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("Sequence<")), JoinCStringLiterals(ArrayLiteral(SGE_STR(", ")), _Expressions::Name...), ArrayLiteral(SGE_STR(">")));
		using Expressions = TypeList<_Expressions...>;
	};

	template<IsExpression... _Expressions>
	struct Select : public Expression
	{
		static_assert(sizeof...(_Expressions) > 0, "SelectExpression must contain at least one expression.");
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("Select<")), JoinCStringLiterals(ArrayLiteral(SGE_STR(", ")), _Expressions::Name...), ArrayLiteral(SGE_STR(">")));
		using Expressions = TypeList<_Expressions...>;
	};

	template<IsExpression _Expression>
	struct Negate : public Expression
	{
		using Expression = _Expression;
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("Negate<")), _Expression::Name, ArrayLiteral(SGE_STR(">")));
	};

	template<IsExpression _Expression>
	struct Optional : public Expression
	{
		using Expression = _Expression;
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("Optional<")), _Expression::Name, ArrayLiteral(SGE_STR(">")));
	};

	template<IsExpression _Expression, SizeType _MinCount = 0, SizeType _MaxCount = UINT64_MAX, bool _IsAggressive = false>
	struct Repeat : public Expression
	{
		static_assert(_MinCount <= _MaxCount, "MinCount must be less than or equal to MaxCount.");
		using Expression = _Expression;
		inline static constexpr const SizeType MinCount = _MinCount;
		inline static constexpr const SizeType MaxCount = _MaxCount;
		inline static constexpr const bool IsAggressive = _IsAggressive;
		inline static constexpr const auto Name = ConcatCStringLiteral(ArrayLiteral(SGE_STR("Repeat<")), _Expression::Name, ArrayLiteral(SGE_STR(", ")), IntegerToCStringLiteral<_MinCount, Char>(), ArrayLiteral(SGE_STR(", ")), IntegerToCStringLiteral<_MaxCount, Char>(), ArrayLiteral(SGE_STR(", ")), BoolToCStringLiteral<_IsAggressive, Char>(), ArrayLiteral(SGE_STR(">")));
	};

	template<ArrayLiteral _Name, IsExpression _Expression, bool _IsSynchronousPoint = false>
	struct Rule
	{
		inline static constexpr const auto Name = _Name;
		using Expression = _Expression;
		inline static constexpr const bool IsSynchronousPoint = _IsSynchronousPoint;
	};

	template<typename T>
	concept IsRule = requires() {
		{
			T::Name.m_Value
		} -> std::convertible_to<const Char*>;
		typename T::Expression;
		{
			T::IsSynchronousPoint
		} -> std::convertible_to<bool>;
	};

	template<ArrayLiteral _Name>
	struct RuleReference : public Expression
	{
		inline static constexpr const auto Name = _Name;
	};

	namespace Detail
	{
		template<ArrayLiteral _ExpectedRuleName, typename _Rule>
		struct IsRuleWithName
		{
			inline static constexpr const bool Value = []() constexpr -> bool {
				if constexpr (IsRule<_Rule>)
					return IsSameCString(_ExpectedRuleName.m_Value, _Rule::Name.m_Value);
				else
					return false;
			}();
		};

		template<ArrayLiteral _ExpectedRuleName>
		struct IsRuleWithNameWrapper
		{
			template<typename _Rule>
			using Type = IsRuleWithName<_ExpectedRuleName, _Rule>;
		};
	}

	template<IsRule... _Rules>
	struct Language
	{
		using Rules = TypeList<_Rules...>;

		template<ArrayLiteral _Name>
		using GetRule = typename Rules::template Get<Rules::template FirstIndex<Detail::IsRuleWithNameWrapper<_Name>::template Type>>;
	};

	template<typename T>
	concept IsLanguage = requires() {
		typename T::Rules;
	};
}

/*!
@}
*/