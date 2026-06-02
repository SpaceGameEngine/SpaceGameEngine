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

// AbstractSyntaxTreeNode is a non-templated class.
// Constructor: AbstractSyntaxTreeNode(const Char* pname, begin, end, Vector<AbstractSyntaxTreeNode>&&)
// PrintNode always formats: "<name>\t<line:L, column:C>\n" for the node itself,
// then recursively indents each child.
// Node names come from Grammar::XXX::Name.m_Value, e.g. "MatchTokenType<1>",
// "Sequence<MatchTokenType<1>, MatchTokenType<2>>", "rule", etc.

TEST(AbstractSyntaxTreePrinter, MatchTokenTypeExpressionTest)
{
	// MatchTokenType<Identifier(=1)>::Name.m_Value == "MatchTokenType<1>"
	using Expression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	// Leaf node: no children
	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(Expression::Name.m_Value, begin, end, {});

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("MatchTokenType<1>\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, MatchTokenTypeAndContentExpressionTest)
{
	// MatchTokenTypeAndContent<IntegerLiteral(=2), "123">::Name.m_Value == "MatchTokenTypeAndContent<2, 123>"
	using Expression = Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::IntegerLiteral, SGE_STR("123")>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("123"), 2, 5));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	// Leaf node: no children
	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(Expression::Name.m_Value, begin, end, {});

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("MatchTokenTypeAndContent<2, 123>\t<line:2, column:5>\n"));
}

TEST(AbstractSyntaxTreePrinter, SequenceExpressionTest)
{
	// Sequence<MatchTokenType<1>, MatchTokenType<2>>::Name.m_Value
	//     == "Sequence<MatchTokenType<1>, MatchTokenType<2>>"
	using Expression1 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using Expression2 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>;
	using SequenceExpression = Parser::Grammar::Sequence<Expression1, Expression2>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();
	auto mid = begin + 1;

	// Build children as Vector<AbstractSyntaxTreeNode>
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
	children.EmplaceBack(Expression1::Name.m_Value, begin, mid, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});
	children.EmplaceBack(Expression2::Name.m_Value, mid, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(SequenceExpression::Name.m_Value, begin, end, std::move(children));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Sequence<MatchTokenType<1>, MatchTokenType<2>>\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:1>\n\tMatchTokenType<2>\t<line:1, column:5>\n"));
}

TEST(AbstractSyntaxTreePrinter, SelectExpressionTest)
{
	// Select<MatchTokenType<1>, MatchTokenType<2>>::Name.m_Value
	//     == "Select<MatchTokenType<1>, MatchTokenType<2>>"
	// A Select node carries one child: whichever alternative matched.
	using Expression1 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using Expression2 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>;
	using SelectExpression = Parser::Grammar::Select<Expression1, Expression2>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	// Child: the first alternative matched (Expression1)
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
	children.EmplaceBack(Expression1::Name.m_Value, begin, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(SelectExpression::Name.m_Value, begin, end, std::move(children));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Select<MatchTokenType<1>, MatchTokenType<2>>\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, NegateExpressionTest)
{
	// Negate<MatchTokenType<1>>::Name.m_Value == "Negate<MatchTokenType<1>>"
	// A Negate node consumes zero tokens (begin == end of the node's range),
	// but begin must still point to a valid token so PrintNode can read its position.
	using Expression = Parser::Grammar::Negate<Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();

	// Zero-length span: begin == end of the node, but begin is still dereferenceable
	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(Expression::Name.m_Value, begin, begin, {});

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Negate<MatchTokenType<1>>\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, OptionalExpressionTest)
{
	// Optional<MatchTokenType<1>>::Name.m_Value == "Optional<MatchTokenType<1>>"
	// With a matched child: one child node.
	// Without a matched child: no children; the node still has a valid begin for position.
	using InnerExpression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using OptionalExpression = Parser::Grammar::Optional<InnerExpression>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	// Case 1: inner expression matched — one child
	{
		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
		children.EmplaceBack(InnerExpression::Name.m_Value, begin, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(OptionalExpression::Name.m_Value, begin, end, std::move(children));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Optional<MatchTokenType<1>>\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:1>\n"));
	}

	// Case 2: inner expression not present — no children; zero-length span, begin still valid
	{
		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(OptionalExpression::Name.m_Value, begin, begin, {});

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Optional<MatchTokenType<1>>\t<line:1, column:1>\n"));
	}
}

TEST(AbstractSyntaxTreePrinter, RepeatExpressionTest)
{
	// Repeat<MatchTokenType<1>, 0, 10>::Name.m_Value == "Repeat<MatchTokenType<1>, 0, 10>"
	using InnerExpression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using RepeatExpression = Parser::Grammar::Repeat<InnerExpression, 0, 10>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("bar"), 1, 5));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("baz"), 1, 9));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();
	auto mid1 = begin + 1;
	auto mid2 = begin + 2;

	// Case 1: three repetitions
	{
		Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
		children.EmplaceBack(InnerExpression::Name.m_Value, begin, mid1, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});
		children.EmplaceBack(InnerExpression::Name.m_Value, mid1, mid2, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});
		children.EmplaceBack(InnerExpression::Name.m_Value, mid2, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(RepeatExpression::Name.m_Value, begin, end, std::move(children));

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Repeat<MatchTokenType<1>, 0, 10>\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:5>\n\tMatchTokenType<1>\t<line:1, column:9>\n"));
	}

	// Case 2: zero repetitions — no children; zero-length span, begin still valid
	{
		Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(RepeatExpression::Name.m_Value, begin, begin, {});

		CumulateStream<> stream;
		StreamReader<StringSerializer<String>> reader(stream);
		Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

		String result = reader.ReadAll();
		ASSERT_EQ(result, SGE_STR("Repeat<MatchTokenType<1>, 0, 10>\t<line:1, column:1>\n"));
	}
}

TEST(AbstractSyntaxTreePrinter, RuleExpressionTest)
{
	// Rule<"my_rule", InnerExpression>::Name.m_Value == "my_rule"
	// A rule node's name is just the rule name literal; its single child is the matched expression.
	using InnerExpression = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using RuleExpression = Parser::Grammar::Rule<SGE_STR("my_rule"), InnerExpression>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));

	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	// Child: the matched inner expression
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
	children.EmplaceBack(InnerExpression::Name.m_Value, begin, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(RuleExpression::Name.m_Value, begin, end, std::move(children));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("my_rule\t<line:1, column:1>\n\tMatchTokenType<1>\t<line:1, column:1>\n"));
}

TEST(AbstractSyntaxTreePrinter, NestedSequenceExpressionTest)
{
	// A Sequence whose children are themselves compound nodes:
	//   Sequence< Sequence<MTT<Id>, MTTAC<Id,"fn">>, MTT<IntLiteral> >
	// This validates recursive indented printing across three levels.
	using E1 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using E2 = Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>;
	using E3 = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::IntegerLiteral>;
	using InnerSeq = Parser::Grammar::Sequence<E1, E2>;
	using OuterSeq = Parser::Grammar::Sequence<InnerSeq, E3>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 5));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 8));

	auto t0 = tokens.GetConstBegin();
	auto t1 = t0 + 1;
	auto t2 = t0 + 2;
	auto t3 = tokens.GetConstEnd();

	// Build inner Sequence node (E1 + E2)
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> innerChildren;
	innerChildren.EmplaceBack(E1::Name.m_Value, t0, t1, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});
	innerChildren.EmplaceBack(E2::Name.m_Value, t1, t2, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	// Build outer Sequence node (InnerSeq + E3)
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> outerChildren;
	outerChildren.EmplaceBack(InnerSeq::Name.m_Value, t0, t2, std::move(innerChildren));
	outerChildren.EmplaceBack(E3::Name.m_Value, t2, t3, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(OuterSeq::Name.m_Value, t0, t3, std::move(outerChildren));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	// OuterSeq name: "Sequence<Sequence<MatchTokenType<1>, MatchTokenTypeAndContent<1, fn>>, MatchTokenType<2>>"
	// InnerSeq name: "Sequence<MatchTokenType<1>, MatchTokenTypeAndContent<1, fn>>"
	ASSERT_EQ(result,
		SGE_STR("Sequence<Sequence<MatchTokenType<1>, MatchTokenTypeAndContent<1, fn>>, MatchTokenType<2>>\t<line:1, column:1>\n")
		SGE_STR("\tSequence<MatchTokenType<1>, MatchTokenTypeAndContent<1, fn>>\t<line:1, column:1>\n")
		SGE_STR("\t\tMatchTokenType<1>\t<line:1, column:1>\n")
		SGE_STR("\t\tMatchTokenTypeAndContent<1, fn>\t<line:1, column:5>\n")
		SGE_STR("\tMatchTokenType<2>\t<line:1, column:8>\n"));
}

TEST(AbstractSyntaxTreePrinter, RuleWithSequenceBodyTest)
{
	// Rule node whose inner expression is a Sequence, mirroring real grammar usage.
	// Rule<"func_decl", Sequence<MTTAC<Id,"fn">, MTT<Id>>>::Name.m_Value == "func_decl"
	using KwExpr = Parser::Grammar::MatchTokenTypeAndContent<Lexer::TokenTypes::Identifier, SGE_STR("fn")>;
	using NameExpr = Parser::Grammar::MatchTokenType<Lexer::TokenTypes::Identifier>;
	using BodyExpr = Parser::Grammar::Sequence<KwExpr, NameExpr>;
	using RuleExpr = Parser::Grammar::Rule<SGE_STR("func_decl"), BodyExpr>;

	Vector<Lexer::Token> tokens;
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("fn"), 1, 1));
	tokens.EmplaceBack(Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("myFunc"), 1, 4));

	auto begin = tokens.GetConstBegin();
	auto mid = begin + 1;
	auto end = tokens.GetConstEnd();

	// Sequence body has two leaf children
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> bodyChildren;
	bodyChildren.EmplaceBack(KwExpr::Name.m_Value, begin, mid, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});
	bodyChildren.EmplaceBack(NameExpr::Name.m_Value, mid, end, Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>{});

	// Rule node wraps the sequence body as its single child
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> ruleChildren;
	ruleChildren.EmplaceBack(BodyExpr::Name.m_Value, begin, end, std::move(bodyChildren));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(RuleExpr::Name.m_Value, begin, end, std::move(ruleChildren));

	CumulateStream<> stream;
	StreamReader<StringSerializer<String>> reader(stream);
	Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(stream, node);

	String result = reader.ReadAll();
	// BodyExpr name: "Sequence<MatchTokenTypeAndContent<1, fn>, MatchTokenType<1>>"
	ASSERT_EQ(result,
		SGE_STR("func_decl\t<line:1, column:1>\n")
		SGE_STR("\tSequence<MatchTokenTypeAndContent<1, fn>, MatchTokenType<1>>\t<line:1, column:1>\n")
		SGE_STR("\t\tMatchTokenTypeAndContent<1, fn>\t<line:1, column:1>\n")
		SGE_STR("\t\tMatchTokenType<1>\t<line:1, column:4>\n"));
}
