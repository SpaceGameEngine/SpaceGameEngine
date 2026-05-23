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

TEST(Grammar, IsExpressionTest)
{
	static_assert(Parser::Grammar::IsExpression<Parser::Grammar::Expression>);
	static_assert(!Parser::Grammar::IsExpression<int>);
}

TEST(Grammar, MatchTokenTypeExpressionTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
}

TEST(Grammar, MatchTokenTypeAndContentExpressionTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::Identifier, SGE_STR("test")>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
	static_assert(IsSameCString(Expression::Content.m_Value, SGE_STR("test")));
}

TEST(Grammar, SequenceExpressionTest)
{
	using Expression = Parser::Grammar::SequenceExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(Grammar, SelectExpressionTest)
{
	using Expression = Parser::Grammar::SelectExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(Grammar, NegateExpressionTest)
{
	using Expression = Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
}

TEST(Grammar, OptionalExpressionTest)
{
	using Expression = Parser::Grammar::OptionalExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
}

TEST(Grammar, RepeatExpressionTest)
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

TEST(Grammar, RuleExpressionTest)
{
	using Expression = Parser::Grammar::RuleExpression<SGE_STR("rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(IsSameCString(Expression::Name.m_Value, SGE_STR("rule")));
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
	static_assert(Expression::IsSynchronousPoint == false);

	using Expression2 = Parser::Grammar::RuleExpression<SGE_STR("sync_rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>, true>;
	static_assert(Parser::Grammar::IsExpression<Expression2>);
	static_assert(IsSameCString(Expression2::Name.m_Value, SGE_STR("sync_rule")));
	static_assert(std::same_as<Expression2::Expression, Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>>);
	static_assert(Expression2::IsSynchronousPoint == true);
}

TEST(Grammar, UnderlyingExpressionTypeTest)
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
	using Expression8 = Parser::Grammar::RuleExpression<SGE_STR("rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression8>, Expression8>);
	using Expression9 = Parser::Grammar::RuleExpression<SGE_STR("sync_rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, true>;
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<Expression9>, Expression9>);
	class CustomExpression : public Expression7
	{
	};
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<CustomExpression>, Expression7>);
	static_assert(!std::same_as<Parser::Grammar::UnderlyingExpressionType<CustomExpression>, CustomExpression>);
}

TEST(Grammar, IsCustomExpressionTest)
{
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::SequenceExpression<
					  Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
					  Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::SelectExpression<
					  Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
					  Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::OptionalExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 1, 10>>);
	static_assert(!Parser::Grammar::IsCustomExpression<Parser::Grammar::RuleExpression<SGE_STR("rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>>);
	static_assert(!Parser::Grammar::IsCustomExpression<int>);
	using BaseExpression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 1, 10>;
	class CustomExpression : public BaseExpression
	{
	};
	static_assert(!Parser::Grammar::IsCustomExpression<BaseExpression>);
	static_assert(Parser::Grammar::IsCustomExpression<CustomExpression>);
}