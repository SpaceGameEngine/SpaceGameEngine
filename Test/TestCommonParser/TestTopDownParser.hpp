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

TEST(TopDownParser, MatchTokenTypeExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailUnexpectedTokenType
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailUnexpectedEnd
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, MatchTokenTypeAndContentExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::Identifier, SGE_STR("foo")>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailUnexpectedToken
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
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

	// ParseFailUnexpectedEnd
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, SequenceExpression)
{
	using Expression = Parser::Grammar::SequenceExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_EQ(result.m_First.Get().GetChildren().template Get<0>().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetChildren().template Get<0>().GetEndTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren().template Get<1>().GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetChildren().template Get<1>().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailOnFirstExpression: first token is IntegerLiteral, expected Identifier
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("99"), 1, 4));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailOnSecondExpression: second token is Identifier, expected IntegerLiteral
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Identifier));
	}
}

TEST(TopDownParser, SelectExpression)
{
	using Expression = Parser::Grammar::SelectExpression<
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>,
		Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>>;

	// ParseSuccessFirstAlternative
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
		ASSERT_EQ(result.m_First.Get().GetChild().GetTypeIndex(), 0);
	}

	// ParseSuccessSecondAlternative
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChild().GetTypeIndex(), 1);
	}

	// ParseFailNoAlternativeMatches: each branch produces one error, merged in order
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Semicolon, SGE_STR(";"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		// First branch: expected Identifier, got Semicolon
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Semicolon));
		// Second branch: expected IntegerLiteral, got Semicolon
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::Semicolon));
	}
}

TEST(TopDownParser, NegateExpression)
{
	using Expression = Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

	// ParseSuccessWhenInnerFails: inner error is forwarded in result.m_Second
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
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

	// ParseFailWhenInnerSucceeds
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::InvalidExpression);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, OptionalExpression)
{
	using Expression = Parser::Grammar::OptionalExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

	// ParseSuccessWhenInnerSucceeds
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_TRUE(result.m_First.Get().GetChild().HasValue());
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseSuccessWhenInnerFails: inner error is forwarded in result.m_Second
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_FALSE(result.m_First.Get().GetChild().HasValue());
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

TEST(TopDownParser, RepeatExpression)
{
	// ParseSuccessZeroOrMore: matches 2 identifiers, stops on IntegerLiteral; stop error is forwarded
	{
		using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
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
		using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 0);
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
		using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 2, 5>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstBegin() + 2);
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailBelowMinCount: matches 1, fails on 2nd (UnexpectedTokenType at col=3), then appends RequireMoreRepetition at col=1
	{
		using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 2, 5>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 3));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
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
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 0);
	}

	// ParseSuccessUpToMaxCount: stops exactly at MaxCount=2, no failed parse attempt, no errors
	{
		using Expression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 1, 2>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("a"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("b"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("c"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetChildren().GetSize(), 2);
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}
}

TEST(TopDownParser, RuleExpression)
{
	using Expression = Parser::Grammar::RuleExpression<SGE_STR("identifier_rule"), Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

	// ParseSuccess
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFail: inner error (UnexpectedTokenType) is transparently forwarded
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
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

TEST(TopDownParser, RuleExpressionSynchronousPoint)
{
	using InnerExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using Expression = Parser::Grammar::RuleExpression<SGE_STR("sync_rule"), InnerExpression, true>;

	// ParseSuccess: first token matches, no recovery needed
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseRecovery: first token does not match, skips it, recovers on second token
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 5));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 1);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseRecoveryMultipleSkipped: skips several non-matching tokens before recovering
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("1"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("2"), 1, 3));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("3"), 1, 5));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 7));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 3);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin() + 3);
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailNoRecovery: all tokens are non-matching, cannot recover, returns failure with errors
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("1"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("2"), 1, 3));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 3);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
		// After exhausting all tokens, a final UnexpectedEnd is recorded at the last token's position
		ASSERT_EQ(result.m_Second[2].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[2].GetLine(), 1);
		ASSERT_EQ(result.m_Second[2].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 0);
	}

	// ParseFailEmptyInput: empty token stream, cannot recover, returns failure
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<Expression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}

TEST(TopDownParser, UnderlyingExpression)
{
	using BaseExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;

	// Custom expression derived from a built-in expression; parser dispatches via UnderlyingExpressionType
	class CustomIdentifierExpression : public BaseExpression
	{
	};
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<CustomIdentifierExpression>, BaseExpression>);

	// ParseSuccess: custom expression resolves to BaseExpression and matches an identifier token
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<CustomIdentifierExpression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_TRUE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 0);
		ASSERT_EQ(result.m_First.Get().GetBeginTokenIter(), tokens.GetConstBegin());
		ASSERT_EQ(result.m_First.Get().GetEndTokenIter(), tokens.GetConstEnd());
	}

	// ParseFailUnexpectedTokenType: custom expression rejects a non-identifier token with correct error
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 1));

		auto result = Parser::TopDownParser::Parse<CustomIdentifierExpression>(tokens.GetConstBegin(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedTokenType);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], ToString<String>(Lexer::TokenTypes::Identifier));
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[1], ToString<String>(Lexer::TokenTypes::IntegerLiteral));
	}

	// ParseFailUnexpectedEnd: custom expression reports UnexpectedEnd on empty input
	{
		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto result = Parser::TopDownParser::Parse<CustomIdentifierExpression>(tokens.GetConstEnd(), tokens.GetConstEnd());
		ASSERT_FALSE(result.m_First.HasValue());
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Parser::TopDownParser::ErrorTypeId::UnexpectedEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
}