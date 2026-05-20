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
	};

	template<typename T>
	concept IsExpression = std::derived_from<T, Expression>;

	template<Lexer::TokenType _Type>
	struct MatchTokenTypeExpression : public Expression
	{
		inline static constexpr const Lexer::TokenType Type = _Type;
	};

	template<Lexer::TokenType _Type, ArrayLiteral _Content>
	struct MatchTokenTypeAndContentExpression : public Expression
	{
		inline static constexpr const Lexer::TokenType Type = _Type;
		inline static constexpr const auto Content = _Content;
	};

	template<IsExpression... _Expressions>
	struct SequenceExpression : public Expression
	{
		using Expressions = TypeList<_Expressions...>;
	};

	template<IsExpression... _Expressions>
	struct SelectExpression : public Expression
	{
		using Expressions = TypeList<_Expressions...>;
	};

	template<IsExpression _Expression>
	struct NegateExpression : public Expression
	{
		using Expression = _Expression;
	};

	template<IsExpression _Expression>
	struct OptionalExpression : public Expression
	{
		using Expression = _Expression;
	};

	template<IsExpression _Expression, SizeType _MinCount = 0, SizeType _MaxCount = UINT64_MAX>
	struct RepeatExpression : public Expression
	{
		using Expression = _Expression;
		inline static constexpr const SizeType MinCount = _MinCount;
		inline static constexpr const SizeType MaxCount = _MaxCount;
	};

	namespace Detail
	{
		template<Lexer::TokenType _Type>
		inline constexpr auto GetUnderlyingExpressionType(MatchTokenTypeExpression<_Type>&&)
		{
			return TypeWrapper<MatchTokenTypeExpression<_Type>>{};
		}

		template<Lexer::TokenType _Type, ArrayLiteral _Content>
		inline constexpr auto GetUnderlyingExpressionType(MatchTokenTypeAndContentExpression<_Type, _Content>&&)
		{
			return TypeWrapper<MatchTokenTypeAndContentExpression<_Type, _Content>>{};
		}

		template<IsExpression... _Expressions>
		inline constexpr auto GetUnderlyingExpressionType(SequenceExpression<_Expressions...>&&)
		{
			return TypeWrapper<SequenceExpression<_Expressions...>>{};
		}

		template<IsExpression... _Expressions>
		inline constexpr auto GetUnderlyingExpressionType(SelectExpression<_Expressions...>&&)
		{
			return TypeWrapper<SelectExpression<_Expressions...>>{};
		}

		template<IsExpression _Expression>
		inline constexpr auto GetUnderlyingExpressionType(NegateExpression<_Expression>&&)
		{
			return TypeWrapper<NegateExpression<_Expression>>{};
		}

		template<IsExpression _Expression>
		inline constexpr auto GetUnderlyingExpressionType(OptionalExpression<_Expression>&&)
		{
			return TypeWrapper<OptionalExpression<_Expression>>{};
		}

		template<IsExpression _Expression, SizeType _MinCount, SizeType _MaxCount>
		inline constexpr auto GetUnderlyingExpressionType(RepeatExpression<_Expression, _MinCount, _MaxCount>&&)
		{
			return TypeWrapper<RepeatExpression<_Expression, _MinCount, _MaxCount>>{};
		}
	}

	/*!
	@brief get underlying expression type of any expression type. So the user can define custom rule expressions by deriving from the composited expression type.
	*/
	template<IsExpression _Expression>
	using UnderlyingExpressionType = typename decltype(Detail::GetUnderlyingExpressionType(std::declval<_Expression>()))::Type;
}

/*!
@}
*/