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

	template<IsExpression _Expression>
	struct Rule : public Expression
	{
		using Expression = _Expression;
	};

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
}

/*!
@}
*/