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
#include "AbstractSyntaxTree.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(AbstractSyntaxTreeNode, InstanceTest)
{
	Vector<Lexer::Token> tokens{Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1), Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5)};
	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();
	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
	const Char* node1_name = SGE_STR("node1");
	const Char* node2_name = SGE_STR("node2");
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode(node1_name, begin, begin + 1, {}));
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode(node2_name, begin + 1, end, {}));
	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode node(SGE_STR("test"), begin, end, std::move(children));
	ASSERT_TRUE(IsSameCString(node.GetName(), SGE_STR("test")));
	ASSERT_EQ(node.GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetEndTokenIter(), end);
	ASSERT_EQ(node.GetChildren().GetSize(), 2);
	ASSERT_TRUE(IsSameCString(node.GetChildren()[0].GetName(), node1_name));
	ASSERT_EQ(node.GetChildren()[0].GetBeginTokenIter(), begin);
	ASSERT_EQ(node.GetChildren()[0].GetEndTokenIter(), begin + 1);
	ASSERT_EQ(node.GetChildren()[0].GetChildren().GetSize(), 0);
	ASSERT_TRUE(IsSameCString(node.GetChildren()[1].GetName(), node2_name));
	ASSERT_EQ(node.GetChildren()[1].GetBeginTokenIter(), begin + 1);
	ASSERT_EQ(node.GetChildren()[1].GetEndTokenIter(), end);
	ASSERT_EQ(node.GetChildren()[1].GetChildren().GetSize(), 0);
}

TEST(AbstractSyntaxTreeNode, VisitTest)
{
	// Build a tree:
	//   root("root") [begin, end)
	//     child0("child") [begin, begin+1)
	//     child1("other") [begin+1, end)
	//       grandchild("child") [begin+1, end)
	Vector<Lexer::Token> tokens{Lexer::Token(Lexer::TokenTypes::Identifier, SGE_STR("foo"), 1, 1), Lexer::Token(Lexer::TokenTypes::IntegerLiteral, SGE_STR("42"), 1, 5)};
	auto begin = tokens.GetConstBegin();
	auto end = tokens.GetConstEnd();

	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> grandchildren;
	grandchildren.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode(SGE_STR("child"), begin + 1, end, {}));

	Vector<Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode(SGE_STR("child"), begin, begin + 1, {}));
	children.EmplaceBack(Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode(SGE_STR("other"), begin + 1, end, std::move(grandchildren)));

	Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode root(SGE_STR("root"), begin, end, std::move(children));

	// Visit by rule name "child": should match child0 and grandchild (not root, not child1)
	int count = 0;
	Parser::AbstractSyntaxTree::Visit(SGE_STR("child"), root, [&](const Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode& node) {
		ASSERT_TRUE(IsSameCString(node.GetName(), SGE_STR("child")));
		++count;
	});
	ASSERT_EQ(count, 2);

	// Visit with nullptr rule name: should match all 4 nodes (root + child0 + child1 + grandchild)
	count = 0;
	Parser::AbstractSyntaxTree::Visit(nullptr, root, [&](const Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode&) {
		++count;
	});
	ASSERT_EQ(count, 4);

	// Visit with predicate: match nodes whose token range covers exactly 1 token
	count = 0;
	Parser::AbstractSyntaxTree::Visit(
		[](const Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode& node) {
			return (node.GetEndTokenIter() - node.GetBeginTokenIter()) == 1;
		},
		[&](const Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode&) {
			++count;
		},
		root);
	// child0 [begin, begin+1) and grandchild [begin+1, end) each span 1 token
	ASSERT_EQ(count, 3);
}