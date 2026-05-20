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
#include "Grammar.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(IsExpression, Test)
{
	static_assert(Parser::Grammar::IsExpression<Parser::Grammar::Expression>);
	static_assert(!Parser::Grammar::IsExpression<int>);
}

TEST(MatchTokenTypeExpression, Test)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
}

TEST(MatchTokenTypeAndContentExpression, Test)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::Identifier, SGE_STR("test")>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
	static_assert(IsSameCString(Expression::Content.m_Value, SGE_STR("test")));
}

TEST(SequenceExpression, Test)
{
	using Expression = Parser::Grammar::SequenceExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(SelectExpression, Test)
{
	using Expression = Parser::Grammar::SelectExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(NegateExpression, Test)
{
	using Expression = Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
}

TEST(OptionalExpression, Test)
{
	using Expression = Parser::Grammar::OptionalExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
}

TEST(RepeatExpression, Test)
{
	using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
	static_assert(Expression::MinCount == 0);
	static_assert(Expression::MaxCount == UINT64_MAX);

	using Expression2 = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>, 1, 10>;
	static_assert(Parser::Grammar::IsExpression<Expression2>);
	static_assert(std::same_as<Expression2::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>>);
	static_assert(Expression2::MinCount == 1);
	static_assert(Expression2::MaxCount == 10);
}

TEST(UnderlyingExpressionType, Test)
{
	using Expression1 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression1>, Expression1>);
	using Expression2 = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression2>, Expression2>);
	using Expression3 = Parser::Grammar::SequenceExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression3>, Expression3>);
	using Expression4 = Parser::Grammar::SelectExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression4>, Expression4>);
	using Expression5 = Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression5>, Expression5>);
	using Expression6 = Parser::Grammar::OptionalExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression6>, Expression6>);
	using Expression7 = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 1, 10>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression7>, Expression7>);
	class CustomExpression : public Expression7
	{
	};
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<CustomExpression>, Expression7>);
	static_assert(!std::same_as<Parser::Grammar::UnderlyingExpressionType<CustomExpression>, CustomExpression>);
}