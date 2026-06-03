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
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));	 // rule_b matches cleanly; rule_a skips this
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 4));		 // rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 7));		 // rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 10));		 // unconsumed

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
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));		  // bad for both
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 3));	  // rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 6));	  // rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 9));		  // unconsumed; provides DisablePanicMode position

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// rule_a panic result is returned: errors = RequireExpression + UnexpectedToken + EnablePanicMode + DisablePanicMode
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// rule_a Sequence: MatchTokenTypeAndContent<Identifier,"fn"> fails on Semicolon at (1,1)
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
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::Semicolon));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR(";"));
		// rule_a enters panic mode at Semicolon (1,1)
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rule_a"));
		// rule_a recovers on "fn"+"bar"; iter now points to trailing Semicolon (1,9)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 9);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rule_a"));
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
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));			  // bad for both
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 3));		  // rule_a recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("baz"), 1, 6));		  // rule_a recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 10));	  // rule_b could recover here
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 12));		  // unconsumed

		auto result = Parser::TopDownParser::Parse<SelectLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// rule_a panic result is returned: errors = RequireExpression + UnexpectedToken + EnablePanicMode + DisablePanicMode
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// rule_a Sequence: MatchTokenTypeAndContent<Identifier,"fn"> fails on Semicolon at (1,1)
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
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::Semicolon));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR(";"));
		// rule_a enters panic mode at Semicolon (1,1)
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rule_a"));
		// rule_a recovers on "fn"+"baz"; iter now points to IntegerLiteral "99" (1,10)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 10);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rule_a"));
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

	// ParseSuccessWhenInnerFails: inner error is swallowed, empty node returned with no errors
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin());
		// inner expression failed but Optional swallows the error and returns empty node with no errors
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseSuccessWhenInnerPanics: inner synchronous-point rule succeeds via panic recovery (has errors),
	// Optional treats the panic result as a failure, restores iter and returns an empty node with no errors.
	// A root rule Sequence<Optional<opt_inner>, MatchTokenType<IntegerLiteral>> is used so that the
	// token after Optional is consumed correctly, verifying that iter was properly restored.
	{
		using OptInnerRule = Parser::Grammar::Rule<
			SGE_STR("opt_inner"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using RootRule = Parser::Grammar::Rule<
			SGE_STR("root"),
			Parser::Grammar::Sequence<
				Parser::Grammar::Optional<Parser::Grammar::RuleReference<SGE_STR("opt_inner")>>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>,
			false>;
		using OptLang = Parser::Grammar::Language<OptInnerRule, RootRule>;

		// tokens: IntegerLiteral "42" (bad for opt_inner, good for the trailing MatchTokenType)
		//       + "fn" + "bar" (opt_inner would recover here, but Optional discards panic results)
		//       + Semicolon (unconsumed)
		// Expected flow:
		//   1. Optional<opt_inner> tries opt_inner on "42": panic, skips "42", recovers on "fn"+"bar"
		//      -> panic result has errors -> Optional restores iter to "42", returns empty node, no errors
		//   2. MatchTokenType<IntegerLiteral> consumes "42" cleanly
		//   3. Sequence succeeds; iter now points to "fn"
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 4));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 7));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 10));

		auto result = Parser::TopDownParser::Parse<OptLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// no errors: Optional discarded the panic result silently, IntegerLiteral matched cleanly
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		// root wraps the Sequence node
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		auto& seq = result.m_First.Get().GetChildren()[0];
		ASSERT_EQ(seq.GetChildren().GetSize(), 2);
		// child 0: empty Optional node (iter was restored, "42" not consumed by Optional)
		auto& opt_node = seq.GetChildren()[0];
		ASSERT_EQ(opt_node.GetChildren().GetSize(), 0);
		ASSERT_EQ(opt_node.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(opt_node.GetEndTokenIter(), tokens.GetConstBegin());
		// child 1: MatchTokenType<IntegerLiteral> consumed "42"
		auto& int_node = seq.GetChildren()[1];
		ASSERT_TRUE(IsSameCString(int_node.GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(int_node.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(int_node.GetEndTokenIter(), tokens.GetConstBegin() + 1);
		// Sequence covers "42" only; "fn", "bar", ";" are unconsumed
		ASSERT_EQ(seq.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(seq.GetEndTokenIter(), tokens.GetConstBegin() + 1);
	}
}

TEST(TopDownParser, RepeatTest)
{
	// ============================================================
	// Conservative (IsAggressive = false, default)
	// ============================================================

	// ParseSuccessZeroOrMore: matches 2 identifiers, stops on IntegerLiteral;
	// count(2) >= minCount(0) so iter is restored and no error is forwarded.
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
		// stop error is no longer forwarded; iter restored before IntegerLiteral
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseSuccessZeroMatches: immediately stops on IntegerLiteral;
	// count(0) >= minCount(0) so iter is restored and no error is forwarded.
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin());
		// stop error is no longer forwarded; iter restored before IntegerLiteral
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseSuccessExactlyMinCount: matches 2, stops on IntegerLiteral;
	// count(2) >= minCount(2) so iter is restored and no error is forwarded.
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
		// stop error is no longer forwarded; iter restored before IntegerLiteral
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseFailBelowMinCount: matches 1, fails on 2nd (count(1) < minCount(2));
	// RequireMoreRepetition is prepended before the inner error.
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 2, 5>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 3));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		// RequireMoreRepetition is prepended first
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireMoreRepetition);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 3);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], String(SGE_STR("2")));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[2], String(SGE_STR("1")));
		// inner error follows
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseSuccessStreamEnds: matches all tokens then stream ends;
	// count(2) >= minCount(0) so iter is restored (to end_iter) and no error is forwarded.
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
		// stream-end stop error is no longer forwarded
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// ParseSuccessUpToMaxCount: stops exactly at MaxCount=2, no failed parse attempt, no errors.
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

	// ParseSuccessIterRestoredOnStop: verifies that iter is properly restored after the conservative stop,
	// so the token that caused the stop can be consumed by a subsequent expression.
	// Sequence<Repeat<Identifier, 0, 10>, MatchTokenType<IntegerLiteral>>:
	//   Repeat consumes "a"+"b", stops on "42" (restores iter), then IntegerLiteral consumes "42".
	{
		using RepeatExpr = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 0, 10>;
		using SeqExpression = Parser::Grammar::Sequence<RepeatExpr, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<SeqExpression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		// child[0]: Repeat node, covers "a"+"b"
		auto& repeat_node = result.m_First.Get().GetChildren()[0];
		ASSERT_EQ(repeat_node.GetChildren().GetSize(), 2);
		ASSERT_EQ(repeat_node.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(repeat_node.GetEndTokenIter(), tokens.GetConstBegin() + 2);
		// child[1]: IntegerLiteral node, covers "42"; confirms iter was restored after Repeat
		auto& int_node = result.m_First.Get().GetChildren()[1];
		ASSERT_TRUE(IsSameCString(int_node.GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(int_node.GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(int_node.GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseSuccessIterRestoredOnPanicStop: verifies conservative stop when the inner synchronous-point
	// rule succeeds via panic (has errors). count(1) >= minCount(1) so iter is restored before the
	// panicked token, and the subsequent MatchTokenType<IntegerLiteral> can consume it cleanly.
	// Language: "rep_elem" (sync point) = Sequence<"fn" Identifier>
	//           "root" = Sequence<Repeat<RuleReference<"rep_elem">, 1, 5>, MatchTokenType<IntegerLiteral>>
	// tokens: [fn][foo][42][fn][bar][;]
	//   Repeat iter 1: "fn"+"foo" -> clean success, child pushed
	//   Repeat iter 2: "42" triggers panic in rep_elem (skips "42", recovers "fn"+"bar")
	//                  -> panic result has errors; count(1) >= minCount(1)
	//                  -> iter restored to "42", break, no errors accumulated
	//   MatchTokenType<IntegerLiteral> then consumes "42"
	{
		using RepElemRule = Parser::Grammar::Rule<
			SGE_STR("rep_elem"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using RootRule = Parser::Grammar::Rule<
			SGE_STR("root"),
			Parser::Grammar::Sequence<
				Parser::Grammar::Repeat<Parser::Grammar::RuleReference<SGE_STR("rep_elem")>, 1, 5>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>,
			false>;
		using RepeatLang = Parser::Grammar::Language<RepElemRule, RootRule>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));		 // rep_elem[0] start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 4));		 // rep_elem[0] name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 8));	 // triggers panic; restored by Repeat
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 11));		 // panic recovery start (not consumed)
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 14));		 // panic recovery name (not consumed)
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 17));		 // unconsumed

		auto result = Parser::TopDownParser::Parse<RepeatLang, SGE_STR("root")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		// no errors: panic stop is silent, IntegerLiteral matched cleanly after iter restore
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root")));
		// root wraps Sequence
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		auto& seq = result.m_First.Get().GetChildren()[0];
		ASSERT_EQ(seq.GetChildren().GetSize(), 2);
		// child[0]: Repeat node, contains 1 rep_elem child covering "fn"+"foo"
		auto& rep_node = seq.GetChildren()[0];
		ASSERT_EQ(rep_node.GetChildren().GetSize(), 1);
		ASSERT_EQ(rep_node.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(rep_node.GetEndTokenIter(), tokens.GetConstBegin() + 2);
		auto& elem_node = rep_node.GetChildren()[0];
		ASSERT_TRUE(IsSameCString(elem_node.GetName(), SGE_STR("rep_elem")));
		ASSERT_EQ(elem_node.GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(elem_node.GetEndTokenIter(), tokens.GetConstBegin() + 2);
		// child[1]: MatchTokenType<IntegerLiteral> consumed "42"; confirms iter was restored to "42"
		auto& int_node = seq.GetChildren()[1];
		ASSERT_TRUE(IsSameCString(int_node.GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(int_node.GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(int_node.GetEndTokenIter(), tokens.GetConstBegin() + 3);
	}

	// ParseConservativePanicAcceptedBelowMinCount: when count < minCount, conservative ALSO accepts panic
	// results (HasValue + errors) — it accumulates the errors and pushes the child.
	// Once the pushed child brings count to minCount, the next error (any kind) triggers the conservative
	// stop: iter is restored and the loop exits cleanly with no additional errors.
	// Tokens: [42][fn][foo][;]
	//   iter 1: "42" -> panic in rep_elem: skips "42", recovers "fn"+"foo" -> HasValue + errors
	//           conservative: count(0) < minCount(1) AND HasValue => accepted; panic errors accumulated; iter at ";"
	//   iter 2: ";" -> count(1) >= minCount(1) -> errors present => restore iter to ";", break
	//   Result: HasValue=true, 1 panic child, panic errors forwarded
	{
		using RepElemRule = Parser::Grammar::Rule<
			SGE_STR("rep_elem_cv"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using ConservativeLang = Parser::Grammar::Language<RepElemRule,
														   Parser::Grammar::Rule<
															   SGE_STR("root_cv"),
															   Parser::Grammar::Repeat<Parser::Grammar::RuleReference<SGE_STR("rep_elem_cv")>, 1, 5>,
															   false>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));	 // triggers panic in rep_elem
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 4));		 // panic recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 7));		 // panic recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 10));		 // stops iter 2; iter restored here

		auto result = Parser::TopDownParser::Parse<ConservativeLang, SGE_STR("root_cv")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		// count(1) reached minCount(1), so Repeat succeeds with the panic child
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root_cv")));
		auto& rep_cv = result.m_First.Get().GetChildren()[0];
		ASSERT_EQ(rep_cv.GetChildren().GetSize(), 1);
		// panic-recovered child covers "fn"+"foo"
		ASSERT_EQ(rep_cv.GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(rep_cv.GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// Repeat end iter is restored to before ";" because iter 2 triggered the conservative stop
		ASSERT_EQ(rep_cv.GetEndTokenIter(), tokens.GetConstBegin() + 3);
		// panic errors from iter 1: RequireExpression + UnexpectedToken + EnablePanicMode + DisablePanicMode
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// [0] Sequence inside rep_elem_cv: MatchTokenTypeAndContent<fn> failed on "42"(1,1)
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		// [1] MatchTokenTypeAndContent<fn> on IntegerLiteral "42"
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		// [2] RuleReference rep_elem_cv enters panic at "42"(1,1)
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rep_elem_cv"));
		// [3] panic recovery consumed "fn"(1,4)+"foo"(1,7); iter now at ";"(1,10)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 10);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rep_elem_cv"));
	}

	// ============================================================
	// Aggressive (IsAggressive = true)
	// ============================================================

	// AggressiveAcceptsPanicResult: panic result (HasValue + errors) is accepted and pushed;
	// iter advances past the panic-consumed tokens.
	// Tokens: [fn][foo][42][fn][bar][;]
	//   iter 1: "fn"+"foo" -> clean success, pushed
	//   iter 2: "42" -> panic: skips "42", recovers "fn"+"bar" -> HasValue + errors
	//           aggressive: HasValue => accepted, panic errors accumulated, iter now at ";"
	//   iter 3: ";" -> no value (clean fail), count(2) >= minCount(0) -> stop without restoring iter
	{
		using RepElemRuleAg = Parser::Grammar::Rule<
			SGE_STR("rep_elem_ag"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using AggressiveLang = Parser::Grammar::Language<RepElemRuleAg,
														 Parser::Grammar::Rule<
															 SGE_STR("root_ag"),
															 Parser::Grammar::Repeat<Parser::Grammar::RuleReference<SGE_STR("rep_elem_ag")>, 0, 5, true>,
															 false>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 4));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 8));	 // triggers panic
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 12));		 // panic recovery start
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 15));		 // panic recovery name
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 18));		 // stops Repeat (no value)

		auto result = Parser::TopDownParser::Parse<AggressiveLang, SGE_STR("root_ag")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root_ag")));
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 1);
		auto& rep_node = result.m_First.Get().GetChildren()[0];
		// 2 children: clean rep_elem[0] + panic rep_elem[1]
		ASSERT_EQ(rep_node.GetChildren().GetSize(), 2);
		ASSERT_EQ(rep_node.GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(rep_node.GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(rep_node.GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 3);	 // after skipped "42"
		ASSERT_EQ(rep_node.GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 5);
		// panic errors forwarded: RequireExpression + UnexpectedToken + EnablePanicMode + DisablePanicMode
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// [0] Sequence inside rep_elem_ag: MatchTokenTypeAndContent<fn> failed on "42"(1,8)
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		// [1] MatchTokenTypeAndContent<fn> on IntegerLiteral "42"(1,8)
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		// [2] RuleReference rep_elem_ag enters panic at "42"(1,8)
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag"));
		// [3] panic recovery consumed "fn"(1,12)+"bar"(1,15); iter now at ";"(1,18)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 18);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag"));
	}

	// AggressiveSuccessCleanOnly: clean-only tokens, no panic; same outcome as conservative.
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 0, 10, true>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}

	// AggressiveFailBelowMinCount: matches 1 cleanly, next fails with no value;
	// count(1) < minCount(2) -> RequireMoreRepetition prepended before inner error.
	{
		using Expression = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 2, 5, true>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 3));

		auto result = TestParseExpression<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireMoreRepetition);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>::Name.m_Value));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], String(SGE_STR("2")));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[2], String(SGE_STR("1")));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// AggressiveIterNotRestoredOnCleanStop: after aggressive Repeat stops on a clean failure,
	// iter stays where it is (not restored), so the following element can consume the stopping token.
	// Sequence<Repeat<Identifier, 0, 10, true>, MatchTokenType<IntegerLiteral>>:
	//   Repeat consumes "a"+"b", then "42" fails (no value), iter stays at "42";
	//   MatchTokenType<IntegerLiteral> consumes "42" normally.
	{
		using RepeatExpr = Parser::Grammar::Repeat<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>, 0, 10, true>;
		using SeqExpression = Parser::Grammar::Sequence<RepeatExpr, Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = TestParseExpression<SeqExpression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		auto& int_node = result.m_First.Get().GetChildren()[1];
		ASSERT_EQ(int_node.GetBeginTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(int_node.GetEndTokenIter(), tokens.GetConstEnd());
	}

	// AggressivePanicIterNotRestoredOnStop: contrast with conservative ParseSuccessIterRestoredOnPanicStop.
	// After aggressive Repeat accepts 2 elements (1 clean + 1 panic), the next parse has no value and
	// iter is left at ";" (NOT restored to before the panic). The following MatchTokenType<IntegerLiteral>
	// sees ";" and fails, making the whole Sequence fail.
	// Tokens: [fn][foo][42][fn][bar][;][IntegerLiteral "99"]
	//   Repeat iter 1: "fn"+"foo" -> clean
	//   Repeat iter 2: "42" -> panic: skips "42", recovers "fn"+"bar"; iter now at ";"
	//   Repeat iter 3: ";" -> no value, count(2)>=minCount(1) -> stop; iter stays at ";" (aggressive)
	//   MatchTokenType<IntegerLiteral> sees ";" -> fails -> Sequence fails
	{
		using RepElemRuleAg2 = Parser::Grammar::Rule<
			SGE_STR("rep_elem_ag2"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using AggressiveLang2 = Parser::Grammar::Language<RepElemRuleAg2,
														  Parser::Grammar::Rule<
															  SGE_STR("root_ag2"),
															  Parser::Grammar::Sequence<
																  Parser::Grammar::Repeat<Parser::Grammar::RuleReference<SGE_STR("rep_elem_ag2")>, 1, 5, true>,
																  Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>,
															  false>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 4));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 8));	 // triggers panic
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 12));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 15));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 18));		  // stops Repeat; iter stays here
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 20));	  // unreachable

		auto result = Parser::TopDownParser::Parse<AggressiveLang2, SGE_STR("root_ag2")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		// Sequence fails because aggressive Repeat left iter at ";" and IntegerLiteral cannot match it
		ASSERT_FALSE(result.m_First.HasValue());
		// 4 panic errors forwarded by Repeat + RequireExpression + UnexpectedTokenType from the failing IntegerLiteral child
		ASSERT_EQ(result.m_Second.GetSize(), 6);
		// [0..3]: panic errors accumulated by Repeat during iter 2 (same as AggressiveAcceptsPanicResult)
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag2"));
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 18);	  // iter at ";" after consuming "fn"(1,12)+"bar"(1,15)
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag2"));
		// [4] Sequence.ParseChildren: MatchTokenType<IntegerLiteral> expected but iter stayed at ";"(1,18)
		ASSERT_EQ(result.m_Second[4].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[4].GetLine(), 1);
		ASSERT_EQ(result.m_Second[4].GetColumn(), 18);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		// [5] MatchTokenType<IntegerLiteral> failed on ";"(1,18)
		ASSERT_EQ(result.m_Second[5].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[5].GetLine(), 1);
		ASSERT_EQ(result.m_Second[5].GetColumn(), 18);
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[5].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Semicolon));
	}

	// AggressivePanicFollowedByCleanSuccess: after aggressive Repeat with panic children,
	// the following element still parses correctly when the token stream is set up so that
	// the clean-fail stop token is the right type for the next expression.
	// Tokens: [fn][foo][42][fn][bar][99]
	//   Repeat<rep_elem_ag3, 1, 5, true>:
	//     iter 1: "fn"+"foo" -> clean success
	//     iter 2: "42" -> panic: skips "42", recovers "fn"+"bar"; iter now at "99"
	//     iter 3: "99" (IntegerLiteral) -> no value (MatchTokenTypeAndContent<fn> fails cleanly),
	//             count(2)>=minCount(1) -> stop; iter stays at "99"
	//   MatchTokenType<IntegerLiteral> sees "99" -> success
	//   Sequence succeeds with panic errors carried through
	{
		using RepElemRuleAg3 = Parser::Grammar::Rule<
			SGE_STR("rep_elem_ag3"),
			Parser::Grammar::Sequence<
				Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>,
				Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>,
			true>;
		using AggressiveLang3 = Parser::Grammar::Language<RepElemRuleAg3,
														  Parser::Grammar::Rule<
															  SGE_STR("root_ag3"),
															  Parser::Grammar::Sequence<
																  Parser::Grammar::Repeat<Parser::Grammar::RuleReference<SGE_STR("rep_elem_ag3")>, 1, 5, true>,
																  Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>>,
															  false>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 4));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 8));	 // triggers panic in iter 2
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 12));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 15));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 18));	  // clean-fail stop for Repeat; consumed by IntegerLiteral

		auto result = Parser::TopDownParser::Parse<AggressiveLang3, SGE_STR("root_ag3")>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("root_ag3")));
		auto& seq3 = result.m_First.Get().GetChildren()[0];
		ASSERT_EQ(seq3.GetChildren().GetSize(), 2);
		// child[0]: Repeat node with 2 rep_elem children (1 clean + 1 panic)
		auto& rep3 = seq3.GetChildren()[0];
		ASSERT_EQ(rep3.GetChildren().GetSize(), 2);
		ASSERT_EQ(rep3.GetChildren()[0].GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(rep3.GetChildren()[0].GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(rep3.GetChildren()[1].GetBeginTokenIter(), tokens.GetConstBegin() + 3);
		ASSERT_EQ(rep3.GetChildren()[1].GetEndTokenIter(), tokens.GetConstBegin() + 5);
		// child[1]: IntegerLiteral consumed "99"; proves iter was at "99" after aggressive stop
		auto& int3 = seq3.GetChildren()[1];
		ASSERT_TRUE(IsSameCString(int3.GetName(), Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>::Name.m_Value));
		ASSERT_EQ(int3.GetBeginTokenIter(), tokens.GetConstBegin() + 5);
		ASSERT_EQ(int3.GetEndTokenIter(), tokens.GetConstEnd());
		// panic errors from iter 2 are carried through the sequence result
		ASSERT_EQ(result.m_Second.GetSize(), 4);
		// [0] Sequence inside rep_elem_ag3: MatchTokenTypeAndContent<fn> failed on "42"(1,8)
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::RequireExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], String(Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>::Name.m_Value));
		// [1] MatchTokenTypeAndContent<fn> on IntegerLiteral "42"(1,8)
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedToken);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 4);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], SGE_STR("fn"));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[2], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[3], SGE_STR("42"));
		// [2] RuleReference rep_elem_ag3 enters panic at "42"(1,8)
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::EnablePanicMode);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 8);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag3"));
		// [3] panic recovery consumed "fn"(1,12)+"bar"(1,15); iter now at "99"(1,18)
		ASSERT_EQ(result.m_Second[3].GetTypeId(), Parser::TopDownParser::ErrorTypeId::DisablePanicMode);
		ASSERT_EQ(result.m_Second[3].GetLine(), 1);
		ASSERT_EQ(result.m_Second[3].GetColumn(), 18);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation().GetSize(), 1);
		ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("rep_elem_ag3"));
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

	// SynchronousPoint: Rule body is a Sequence: keyword "fn" (Identifier) followed by a function name (Identifier)
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