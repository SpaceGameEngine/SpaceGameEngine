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
#include "AbstractSyntaxTree.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(AbstractSyntaxTreeNode, MatchTokenTypeExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	static_assert(Parser::Grammar::IsExpression<Expression>);

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> node(begin, end);
	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
}

TEST(AbstractSyntaxTreeNode, MatchTokenTypeAndContentExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::Identifier, SGE_STR("foo")>;
	static_assert(Parser::Grammar::IsExpression<Expression>);

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> node(begin, end);
	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
}

TEST(AbstractSyntaxTreeNode, SequenceExpression)
{
	using Expression1 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using Expression2 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>;
	using SequenceExpression = Parser::Grammar::SequenceExpression<Expression1, Expression2>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();
	auto mid = begin + 1;

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1> child1(begin, mid);
	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2> child2(mid, end);

	Tuple<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1>, Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2>> children(std::move(child1), std::move(child2));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<SequenceExpression> node(begin, end, std::move(children));

	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
	ASSERT_EQ(node.GetChildren().template Get<0>().GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetChildren().template Get<0>().GetEndTokenIter(), mid);
	ASSERT_EQ(node.GetChildren().template Get<1>().GetBeginTokenIter(), mid);
	ASSERT_EQ(node.GetChildren().template Get<1>().GetEndTokenIter(), end);
}

TEST(AbstractSyntaxTreeNode, SelectExpression)
{
	using Expression1 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using Expression2 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>;
	using SelectExpression = Parser::Grammar::SelectExpression<Expression1, Expression2>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1> child(begin, end);
	Variant<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1>, Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2>> varChild(InPlaceIndex<0>, std::move(child));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<SelectExpression> node(begin, end, std::move(varChild));

	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
	ASSERT_EQ(node.GetChild().GetTypeIndex(), 0u);
	ASSERT_EQ(node.GetChild().template Get<0>().GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetChild().template Get<0>().GetEndTokenIter(), end);
}

TEST(AbstractSyntaxTreeNode, OptionalExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using OptionalExpression = Parser::Grammar::OptionalExpression<Expression>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	{
		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> child(begin, end);
		Optional<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>> optChild(std::move(child));

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<OptionalExpression> node(begin, end, std::move(optChild));

		ASSERT_EQ(node.GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetEndTokenIter(), end);
		ASSERT_TRUE(node.GetChild().HasValue());
		ASSERT_EQ(node.GetChild().Get().GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetChild().Get().GetEndTokenIter(), end);
	}

	{
		Optional<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>> optChild;

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<OptionalExpression> node(begin, end, std::move(optChild));

		ASSERT_EQ(node.GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetEndTokenIter(), end);
		ASSERT_FALSE(node.GetChild().HasValue());
	}
}

TEST(AbstractSyntaxTreeNode, RepeatExpression)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;

	{
		using RepeatExpression = Parser::Grammar::RepeatExpression<Expression, 0, 10>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 5));

		auto begin = tokens.GetConstBegin();
		auto end = tokens.GetConstEnd();
		auto mid = begin + 1;

		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>> children;
		children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>(begin, mid));
		children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>(mid, end));

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<RepeatExpression> node(begin, end, std::move(children));

		ASSERT_EQ(node.GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetEndTokenIter(), end);
		ASSERT_EQ(node.GetChildren().GetSize(), 2u);
		ASSERT_EQ(node.GetChildren()[0].GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetChildren()[0].GetEndTokenIter(), mid);
		ASSERT_EQ(node.GetChildren()[1].GetBeginTokenIter(), mid);
		ASSERT_EQ(node.GetChildren()[1].GetEndTokenIter(), end);
	}

	{
		using RepeatExpression = Parser::Grammar::RepeatExpression<Expression>;

		Vector<Lexer::Token> tokens;
		tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

		auto begin = tokens.GetConstBegin();
		auto end = tokens.GetConstEnd();

		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression>> children;

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<RepeatExpression> node(begin, end, std::move(children));

		ASSERT_EQ(node.GetBeginTokenIter(), begin);
		ASSERT_EQ(node.GetEndTokenIter(), end);
		ASSERT_EQ(node.GetChildren().GetSize(), 0u);
	}
}

// Custom expression for testing derived expression support (must be at namespace scope to be used as template argument)
using TestCustomBaseExpression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 1, 5>;
struct TestCustomDerivedExpression : public TestCustomBaseExpression
{
};

TEST(AbstractSyntaxTreeNode, CustomDerivedExpression)
{
	static_assert(Parser::Grammar::IsExpression<TestCustomDerivedExpression>);
	static_assert(std::same_as<Parser::Grammar::UnderlyingExpressionType<TestCustomDerivedExpression>, TestCustomBaseExpression>);

	using InnerExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("x"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>> children;
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>(begin, end));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<TestCustomDerivedExpression> node(begin, end, std::move(children));

	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
	ASSERT_EQ(node.GetChildren().GetSize(), 1u);
}
