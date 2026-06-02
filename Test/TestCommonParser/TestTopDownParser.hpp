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
#include "TopDownParser.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

template<typename _Expression>
inline auto TestParseExpression(const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
{
	auto iter = begin_iter;
	return Parser::TopDownParser::Detail::ParseCore<Parser::Grammar::Language<>, _Expression>::Parse(iter, begin_iter, end_iter);
}

TEST(TopDownParser, MatchTokenTypeTest)
{
	using Expression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), Expression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailUnexpectedTokenType
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailUnexpectedEnd: empty stream (begin == end)
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, MatchTokenTypeAndContentTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("foo")>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), Expression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailUnexpectedToken
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], SGE_STR("foo"));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[3], SGE_STR("bar"));
	}

	// ParseFailUnexpectedEnd: empty stream (begin == end)
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, SequenceTest)
{
	using Expression = Parser::Grammar::Sequence<
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), Expression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[1].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailOnFirstExpression: first token is IntegerLiteral, expected Identifier
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 4));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// Sequence: first child fails -> RequireExpression then inner UnexpectedTokenType
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailOnSecondExpression: second token is Identifier, expected IntegerLiteral
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// Sequence: second child fails -> RequireExpression then inner UnexpectedTokenType
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Identifier));
	}

	// ParseFailUnexpectedEnd: first token matches Identifier, second is missing; error position follows last token
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// Sequence: second child hits UnexpectedEnd -> RequireExpression then UnexpectedEnd
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		// UnexpectedEnd: prev_iter is "foo" at (1,1) with size 3, so column = 1 + 3 = 4
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, SelectTest)
{
	using Expression = Parser::Grammar::Select<
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;

	// ParseSuccessFirstAlternative
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseSuccessSecondAlternative
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailNoAlternativeMatches: each branch produces one error, merged in order
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// Each failed branch appends UnsatisfiedExpression then its inner error, so 4 errors total
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// First branch: UnsatisfiedExpression for Identifier alternative
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnsatisfiedExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value);
		// First branch inner error: expected Identifier, got Semicolon
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Semicolon));
		// Second branch: UnsatisfiedExpression for IntegerLiteral alternative
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnsatisfiedExpression);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value);
		// Second branch inner error: expected IntegerLiteral, got Semicolon
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Semicolon));
	}

	// Priority tests: language with two synchronous-point rules as Select alternatives.
	// "rule_a" matches Sequence<"fn" Identifier>, "rule_b" matches IntegerLiteral.
	// The root rule "root" = Select<RuleReference<"rule_a">, RuleReference<"rule_b">>.
	using RuleA = Parser::Grammar::Rule<
		SGE_STR("rule_a"),
		Parser::Grammar::Sequence<
			Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
			Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
		true>;
	using RuleB = Parser::Grammar::Rule<
		SGE_STR("rule_b"),
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>,
		true>;
	using RootRule = Parser::Grammar::Rule<
		SGE_STR("root"),
		Parser::Grammar::Select<
			Parser::Grammar::RuleReference<SGE_STR("rule_a")>,
			Parser::Grammar::RuleReference<SGE_STR("rule_b")>>,
		false>;
	using SelectLang = Parser::Grammar::Language<RuleA, RuleB, RootRule>;

	// ParseSuccessCleanOverFailedPanic: "rule_a" enters panic mode but fails to recover
	// (only one IntegerLiteral token; after skipping it the stream ends).
	// "rule_b" then succeeds cleanly on the same IntegerLiteral.
	// The clean result is preferred; no errors are reported.
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		// children[0] is the Select node; children[0].GetChildren()[0] is the matched rule
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 1);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetName(), SGE_STR("rule_b")));
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseSuccessCleanOverSuccessfulPanic: "rule_a" enters panic mode and successfully recovers
	// on "fn bar" after skipping the leading IntegerLiteral. Its panic result is stored.
	// "rule_b" then succeeds cleanly on the same leading IntegerLiteral.
	// The clean result wins over the earlier panic result.
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));	// rule_b matches cleanly; rule_a skips this
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 4));		// rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 7));		// rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 10));		// unconsumed

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// clean success: no errors
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		// children[0] is the Select node; children[0].GetChildren()[0] is the matched rule
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 1);
		// rule_b clean result wins over rule_a panic result
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetName(), SGE_STR("rule_b")));
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 1);
	}

	// ParseSuccessPanicWinsOverFailure: both alternatives start from the same bad token (Semicolon).
	// "rule_a" enters panic mode, skips Semicolon, and recovers on "fn bar".
	// "rule_b" fails cleanly (Semicolon is not IntegerLiteral).
	// The first panic result ("rule_a") is returned.
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));	   // bad for both
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 3));	   // rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 6));	   // rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 9));	   // unconsumed; provides DisablePanicMode position

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_GT(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		// children[0] is the Select node; children[0].GetChildren()[0] is the matched rule
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 1);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetName(), SGE_STR("rule_a")));
		// Select node begin/end come from the recovered panic child's begin/end
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// rule_a recovered node covers only "fn" + "bar"
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 3);
	}

	// ParseSuccessFirstPanicWinsOverSecondPanic: both alternatives enter panic mode from the same bad token.
	// "rule_a" recovers on "fn baz", "rule_b" could also recover on IntegerLiteral further ahead,
	// but "rule_a" is listed first so its panic result is chosen.
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));		// bad for both
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 3));		// rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("baz"), 1, 6));		// rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 10));	// rule_b could recover here
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 12));		// unconsumed

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_GT(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		// children[0] is the Select node; children[0].GetChildren()[0] is the matched rule
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 1);
		// rule_a panic wins because it was encountered first
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetName(), SGE_STR("rule_a")));
		// Select node begin/end come from the recovered panic child's begin/end
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// rule_a recovered node covers only "fn" + "baz"
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 3);
	}
}

TEST(TopDownParser, NegateTest)
{
	using Expression = Parser::Grammar::Negate<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

	// ParseSuccessWhenInnerFails: negate succeeds, inner errors are discarded
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), Expression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseFailWhenInnerSucceeds
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::InvalidExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
	}
}

TEST(TopDownParser, OptionalTest)
{
	using Expression = Parser::Grammar::Optional<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

	// ParseSuccessWhenInnerSucceeds
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseSuccessWhenInnerFails: inner error is forwarded in result.m_Second
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}
}

TEST(TopDownParser, RepeatTest)
{
	// ParseSuccessZeroOrMore: matches 2 identifiers, stops on IntegerLiteral; stop error is forwarded
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseSuccessZeroMatches: immediately stops on IntegerLiteral; stop error is forwarded
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseSuccessExactlyMinCount: matches 2, stops on IntegerLiteral; stop error is forwarded
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 2, 5>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailBelowMinCount: matches 1, fails on 2nd (UnexpectedTokenType at col=3), then appends RequireMoreRepetition at col=3
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 2, 5>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 3));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireMoreRepetition);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], String(SGE_STR("2")));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], String(SGE_STR("1")));
	}

	// ParseSuccessStreamEnds: matches all tokens then stream ends; UnexpectedEnd error position is derived from the last token
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		// last token "b" is at col 3 with content size 1, so error column = 3 + 1 = 4
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 4);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}

	// ParseSuccessUpToMaxCount: stops exactly at MaxCount=2, no failed parse attempt, no errors
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 1, 2>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("c"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}
}

TEST(TopDownParser, RuleTest)
{
	using RuleLanguage = Parser::Grammar::Language<
		Parser::Grammar::Rule<SGE_STR("identifier_rule"), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<RuleLanguage, SGE_STR("identifier_rule")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("identifier_rule")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFail: inner error (UnexpectedTokenType) is transparently forwarded
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<RuleLanguage, SGE_STR("identifier_rule")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}
}

TEST(TopDownParser, RuleSynchronousPointTest)
{
	// Rule body is a Sequence: keyword "fn" (Identifier) followed by a function name (Identifier)
	using InnerExpression = Parser::Grammar::Sequence<
		Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
		Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;
	using SyncLanguage = Parser::Grammar::Language<
		Parser::Grammar::Rule<
			SGE_STR("func_decl"),
			InnerExpression,
			true>>;

	// ParseSuccess: tokens match the sequence exactly, no recovery needed
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("myFunc"), 1, 4));

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("func_decl")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		// Child 0: the Sequence node
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), InnerExpression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstEnd());
		// Child 0.0: the MatchTokenTypeAndContent node for "fn"
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetName(), Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 1);
		// Child 0.1: the MatchTokenType node for the function name
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseRecovery: one bad token before a valid "fn <name>", followed by an unconsumed token
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));	 // bad: IntegerLiteral
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 4));		 // recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("myFunc"), 1, 7));	 // function name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 13));		 // unconsumed; provides the DisablePanicMode position

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// errors: RequireExpression(1,1) + UnexpectedToken(1,1) + EnablePanicMode(1,1) + DisablePanicMode(1,13)
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// initial Sequence parse: first child MatchTokenTypeAndContent fails on IntegerLiteral -> RequireExpression then UnexpectedToken
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		// entering panic mode at position of the IntegerLiteral
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		// recovery succeeded; after consuming "fn" and "myFunc", iter points to Semicolon (1,13)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 13);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		// recovered node covers only the consumed "fn" + "myFunc" tokens
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("func_decl")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// Child 0: Sequence node
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), InnerExpression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// Child 0.0: MatchTokenTypeAndContent node for "fn"
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		// Child 0.1: MatchTokenType node for the function name
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 3);
	}

	// ParseRecoveryMultipleSkipped: two bad tokens before a valid "fn <name>", followed by an unconsumed token
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("1"), 1, 1));	// bad
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 3));			// bad
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 5));		// recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 8));		// function name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 11));		// unconsumed; provides the DisablePanicMode position

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// errors: RequireExpression(1,1) + UnexpectedToken(1,1) + EnablePanicMode(1,1)
		//       + RequireExpression(1,3) + UnexpectedToken(1,3)
		//       + DisablePanicMode(1,11)
		ASSERT_EQ(result.m_Second.GetSize(), 6);
		// initial Sequence parse fails on "1"
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("1"));
		// entering panic mode at position of "1"
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		// recovery attempt at ";" (1,3) fails: MatchTokenTypeAndContent<Identifier,"fn"> on Semicolon -> UnexpectedToken
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[4].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[4].GetLine(), 1);
		ASSERT_EQ(result.m_Second[4].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::Semicolon));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[3], SGE_STR(";"));
		// recovery succeeded at "fn" (1,5); after consuming "fn" and "bar", iter points to Semicolon (1,11)
		ASSERT_EQ(result.m_Second[5].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[5].GetLine(), 1);
		ASSERT_EQ(result.m_Second[5].GetColumn(), 11);
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		// recovered node covers "fn" + "bar"
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("func_decl")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 4);
		// Child 0: Sequence node
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetChildren()[0].GetName(), InnerExpression::Name.m_Value));
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 4);
		// Child 0.0: MatchTokenTypeAndContent node for "fn"
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// Child 0.1: MatchTokenType node for the function name
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 3);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 4);
	}

	// ParseFailNoRecovery: all tokens fail to start a valid "fn <name>" sequence
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("1"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 3));

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// errors: RequireExpression(1,1) + UnexpectedToken(1,1) + EnablePanicMode(1,1)
		//       + RequireExpression(1,3) + UnexpectedToken(1,3) + UnexceptedEnd
		ASSERT_EQ(result.m_Second.GetSize(), 6);
		// initial Sequence parse fails on "1"
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("1"));
		// entering panic mode at position of "1"
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		// recovery attempt at ";" (1,3) fails
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[4].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[4].GetLine(), 1);
		ASSERT_EQ(result.m_Second[4].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::Semicolon));
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[3], SGE_STR(";"));
		// recovery attempt at end of token stream fails with UnexpectedEnd
		ASSERT_EQ(result.m_Second[5].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[5].GetLine(), 1);
		ASSERT_EQ(result.m_Second[5].GetColumn(), 4);
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation().GetSize(), 0);
	}

	// ParseFailEmptyInput: empty token stream; Sequence immediately hits UnexpectedEnd, panic mode cannot recover
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// Sequence: first child MatchTokenTypeAndContent hits UnexpectedEnd -> RequireExpression then UnexpectedEnd;
		// iter == end_iter so panic mode is skipped entirely.
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 0);
	}

	// ParseFailSingleBadToken: one invalid token; panic mode skips it and hits stream end,
	// UnexpectedEnd position is derived from the last token via prev_iter
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<SyncLanguage, SGE_STR("func_decl")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		// errors: RequireExpression(1,1) + UnexpectedToken(1,1) + EnablePanicMode(1,1) + UnexpectedEnd(1,3)
		// UnexpectedEnd: prev_iter is "42"(1,1,size=2), so column = 1 + 2 = 3
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("func_decl"));
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 0);
	}
}