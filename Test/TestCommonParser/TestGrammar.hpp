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

TEST(Grammar, MatchTokenTypeTest)
{
	using Expression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
}

TEST(Grammar, MatchTokenTypeAndContentTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("test")>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(Expression::Type == Lexer::TokenTypes::Identifier);
	static_assert(IsSameCString(Expression::Content.m_Value, SGE_STR("test")));
}

TEST(Grammar, SequenceTest)
{
	using Expression = Parser::Grammar::Sequence<
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(Grammar, SelectTest)
{
	using Expression = Parser::Grammar::Select<
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expressions, TypeList<
															Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
															Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>>>);
}

TEST(Grammar, NegateTest)
{
	using Expression = Parser::Grammar::Negate<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>);
}

TEST(Grammar, OptionalTest)
{
	using Expression = Parser::Grammar::Optional<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>);
}

TEST(Grammar, RepeatTest)
{
	using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;
	static_assert(Parser::Grammar::IsExpression<Expression>);
	static_assert(std::same_as<Expression::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>);
	static_assert(Expression::MinCount == 0);
	static_assert(Expression::MaxCount == UINT64_MAX);

	using Expression2 = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>, 1, 10>;
	static_assert(Parser::Grammar::IsExpression<Expression2>);
	static_assert(std::same_as<Expression2::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>);
	static_assert(Expression2::MinCount == 1);
	static_assert(Expression2::MaxCount == 10);
}

TEST(Grammar, RuleTest)
{
	using Rule = Parser::Grammar::Rule<SGE_STR("rule"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;
	static_assert(IsSameCString(Rule::Name.m_Value, SGE_STR("rule")));
	static_assert(std::same_as<Rule::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>);
	static_assert(Rule::IsSynchronousPoint == false);

	using Rule2 = Parser::Grammar::Rule<SGE_STR("sync_rule"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>, true>;
	static_assert(IsSameCString(Rule2::Name.m_Value, SGE_STR("sync_rule")));
	static_assert(std::same_as<Rule2::Expression, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>);
	static_assert(Rule2::IsSynchronousPoint == true);
}

TEST(Grammar, IsRuleTest)
{
	static_assert(Parser::Grammar::IsRule<Parser::Grammar::Rule<SGE_STR("rule"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>>);
	static_assert(!Parser::Grammar::IsRule<int>);
}

TEST(Grammar, RuleReferenceTest)
{
	using Expression = Parser::Grammar::RuleReference<SGE_STR("rule")>;
	static_assert(IsSameCString(Expression::Name.m_Value, SGE_STR("rule")));

	using Expression2 = Parser::Grammar::RuleReference<SGE_STR("sync_rule")>;
	static_assert(IsSameCString(Expression2::Name.m_Value, SGE_STR("sync_rule")));
}

TEST(Grammar, LanguageTest)
{
	using NumberRule = Parser::Grammar::Rule<SGE_STR("number"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;
	using OperatorRule = Parser::Grammar::Rule<SGE_STR("operator"), Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("+")>>;
	using ExpressionRule = Parser::Grammar::Rule<SGE_STR("expression"), Parser::Grammar::Sequence<Parser::Grammar::RuleReference<SGE_STR("number")>, Parser::Grammar::RuleReference<SGE_STR("operator")>, Parser::Grammar::RuleReference<SGE_STR("number")>>>;
	using Language = Parser::Grammar::Language<NumberRule, OperatorRule, ExpressionRule>;

	static_assert(Parser::Grammar::IsLanguage<Language>);
	static_assert(std::same_as<Language::Rules, TypeList<NumberRule, OperatorRule, ExpressionRule>>);

	static_assert(std::same_as<Language::GetRule<SGE_STR("number")>, NumberRule>);
	static_assert(std::same_as<Language::GetRule<SGE_STR("operator")>, OperatorRule>);
	static_assert(std::same_as<Language::GetRule<SGE_STR("expression")>, ExpressionRule>);
}

TEST(Grammar, IsLanguageTest)
{
	using NumberRule = Parser::Grammar::Rule<SGE_STR("number"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;
	using OperatorRule = Parser::Grammar::Rule<SGE_STR("operator"), Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("+")>>;
	using ExpressionRule = Parser::Grammar::Rule<SGE_STR("expression"), Parser::Grammar::Sequence<Parser::Grammar::RuleReference<SGE_STR("number")>, Parser::Grammar::RuleReference<SGE_STR("operator")>, Parser::Grammar::RuleReference<SGE_STR("number")>>>;
	using Language = Parser::Grammar::Language<NumberRule, OperatorRule, ExpressionRule>;

	static_assert(Parser::Grammar::IsLanguage<Language>);
	static_assert(!Parser::Grammar::IsLanguage<int>);
}