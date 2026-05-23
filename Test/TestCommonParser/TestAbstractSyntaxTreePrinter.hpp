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
#include "AbstractSyntaxTreePrinter.h"
#include "Stream/CumulateStream.hpp"
#include "Stream/StreamReader.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(AbstractSyntaxTreePrinter, MatchTokenTypeExpressionTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> node(begin, end);

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("MatchTokenType 1 \"foo\"\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, MatchTokenTypeAndContentExpressionTest)
{
	using Expression = Parser::Grammar::MatchTokenTypeAndContentExpression<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("123"), 2, 5));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> node(begin, end);

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("MatchTokenTypeAndContent 2 \"123\"\t<line:2, column:5>\n"));
}

TEST(AbstractSyntaxTreePrinter, SequenceExpressionTest)
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

	Tuple<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1>,
		  Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2>>
		children(
			Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1>(begin, mid),
			Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2>(mid, end));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<SequenceExpression> node(begin, end, std::move(children));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Sequence\t<line:1, column:1>\n\tMatchTokenType 1 \"foo\"\t<line:1, column:1>\n\tMatchTokenType 2 \"42\"\t<line:1, column:5>\n"));
}

TEST(AbstractSyntaxTreePrinter, SelectExpressionTest)
{
	using Expression1 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using Expression2 = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::IntegerLiteral>;
	using SelectExpression = Parser::Grammar::SelectExpression<Expression1, Expression2>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1> child(begin, end);
	Variant<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression1>,
			Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression2>>
		varChild(InPlaceIndex<0>, std::move(child));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<SelectExpression> node(begin, end, std::move(varChild));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Select\t<line:1, column:1>\n\tMatchTokenType 1 \"foo\"\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, NegateExpressionTest)
{
	using Expression = Parser::Grammar::NegateExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Expression> node(begin, end);

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Negate\n"));
}

TEST(AbstractSyntaxTreePrinter, OptionalExpressionTest)
{
	using InnerExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using OptionalExpression = Parser::Grammar::OptionalExpression<InnerExpression>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	{
		Optional<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>> optChild(
			Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>(begin, end));

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<OptionalExpression> node(begin, end, std::move(optChild));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Optional\t<line:1, column:1>\n\tMatchTokenType 1 \"foo\"\t<line:1, column:1>\n"));
	}

	{
		Optional<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>> optChild;

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<OptionalExpression> node(begin, end, std::move(optChild));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Optional\n"));
	}
}

TEST(AbstractSyntaxTreePrinter, RepeatExpressionTest)
{
	using InnerExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using RepeatExpression = Parser::Grammar::RepeatExpression<InnerExpression, 0, 10>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 5));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();
	auto mid = begin + 1;

	{
		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>> children;
		children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>(begin, mid));
		children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>(mid, end));

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<RepeatExpression> node(begin, end, std::move(children));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Repeat[2]\t<line:1, column:1>\n\tMatchTokenType 1 \"foo\"\t<line:1, column:1>\n\tMatchTokenType 1 \"bar\"\t<line:1, column:5>\n"));
	}

	{
		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<InnerExpression>> children;

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<RepeatExpression> node(begin, end, std::move(children));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Repeat\n"));
	}
}

TEST(AbstractSyntaxTreePrinter, RuleExpressionTest)
{
	using InnerExpression = Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>;
	using RuleExpression = Parser::Grammar::RuleExpression<SGE_STR("rule"), InnerExpression>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<RuleExpression> node(begin, end);

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Rule rule\t<line:1, column:1>\n\tMatchTokenType 1 \"foo\"\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, CustomDerivedExpressionTest)
{
	using BaseExpression = Parser::Grammar::RepeatExpression<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>, 0, 5>;
	struct CustomRepeatExpression : public BaseExpression
	{
	};

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("x"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>> children;
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<Parser::Grammar::MatchTokenTypeExpression<Lexer::TokenTypes::Identifier>>(begin, end));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode<CustomRepeatExpression> node(begin, end, std::move(children));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Repeat[1]\t<line:1, column:1>\n\tMatchTokenType 1 \"x\"\t<line:1, column:1>\n"));
}
