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
#include "Assembler/Parser.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser::Parser;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

TEST(Parse, EmptyProgramTest)
{
	Vector<CommonParser::Lexer::Token> tokens;
	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(tokens);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);
	ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("Program")));
	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 0);
}

TEST(Parse, StatementWithNoArgumentsTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.op;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);
	ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("Program")));

	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 1);

	// VariableIdentifier 内容为 "result"
	SizeType varIdCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), result.m_First.Get(), [&](const auto& node) {
		++varIdCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("result"));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::Identifier);
	});
	ASSERT_EQ(varIdCount, 1);

	// Symbol 内容为 "test.op"
	SizeType symbolCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		++symbolCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("test.op"));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier);
	});
	ASSERT_EQ(symbolCount, 1);

	// 无 Argument
	SizeType argumentCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto&) { ++argumentCount; });
	ASSERT_EQ(argumentCount, 0);
}

TEST(Parse, StatementWithNoLHSTest)
{
	// Statement 可省略 LHS（无赋值），仅有 Symbol 和可选参数
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("test.call 1 2;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 1 条 Statement
	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 1);

	// 无 VariableIdentifier（没有 LHS）
	SizeType varIdCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), result.m_First.Get(), [&](const auto&) { ++varIdCount; });
	ASSERT_EQ(varIdCount, 0);

	// Symbol 为 "test.call"
	SizeType symbolCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		++symbolCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("test.call"));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier);
	});
	ASSERT_EQ(symbolCount, 1);

	// 2 个整数 Argument：依次为 "1"、"2"
	const Char* expectedArgs[] = {SGE_STR("1"), SGE_STR("2")};
	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedArgs[argIndex]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::IntegerLiteral);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 2u);
}

TEST(Parse, StatementWithMultipleLHSTest)
{
	// VariableIdentifierList: 逗号分隔的多个变量作为赋值 LHS
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("a, b, c = test.op;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 1 条 Statement
	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 1);

	// VariableIdentifierList 出现 1 次
	SizeType varIdListCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifierList"), result.m_First.Get(), [&](const auto& node) {
		++varIdListCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("a"));
	});
	ASSERT_EQ(varIdListCount, 1);

	// VariableIdentifier 依次为 "a"、"b"、"c"
	const Char* expectedVarIds[] = {SGE_STR("a"), SGE_STR("b"), SGE_STR("c")};
	SizeType varIdIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(varIdIndex, 3u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedVarIds[varIdIndex]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::Identifier);
		++varIdIndex;
	});
	ASSERT_EQ(varIdIndex, 3u);

	// Symbol 为 "test.op"
	SizeType symbolCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		++symbolCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("test.op"));
	});
	ASSERT_EQ(symbolCount, 1);
}

TEST(Parse, StatementWithIntegerArgumentsTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.add 1 -2;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 指令符号为 "test.add"
	SizeType symbolCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		++symbolCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("test.add"));
	});
	ASSERT_EQ(symbolCount, 1);

	// 两个整数参数：内容依次为 "1"、"-2"，类型均为 IntegerLiteral
	const Char* expectedContents[] = {SGE_STR("1"), SGE_STR("-2")};
	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedContents[argIndex]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::IntegerLiteral);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 2u);
}

TEST(Parse, StatementWithSymbolArgumentsTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.op test.a test.b;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 两个 Symbol 参数：内容依次为 "test.a"、"test.b"，类型均为 SymbolIdentifier
	const Char* expectedSymbolArgs[] = {SGE_STR("test.a"), SGE_STR("test.b")};
	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedSymbolArgs[argIndex]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 2u);

	// 共三个 Symbol 节点：指令 "test.op" + 两个参数
	const Char* expectedAllSymbols[] = {SGE_STR("test.op"), SGE_STR("test.a"), SGE_STR("test.b")};
	SizeType symIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(symIndex, 3u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedAllSymbols[symIndex]);
		++symIndex;
	});
	ASSERT_EQ(symIndex, 3u);
}

TEST(Parse, StatementWithVariableIdentifierArgumentsTest)
{
	// VariableIdentifier 作为 Argument：普通标识符（非 symbol.dot 形式）
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.call foo bar;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 1 条 Statement
	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 1);

	// VariableIdentifier 依次为：LHS "result"、Argument "foo"、Argument "bar"
	const Char* expectedVarIds[] = {SGE_STR("result"), SGE_STR("foo"), SGE_STR("bar")};
	SizeType varIdCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(varIdCount, 3u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedVarIds[varIdCount]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::Identifier);
		++varIdCount;
	});
	ASSERT_EQ(varIdCount, 3u);

	// 2 个 Argument，首 token 分别为 "foo"、"bar"
	const Char* expectedArgs[] = {SGE_STR("foo"), SGE_STR("bar")};
	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedArgs[argIndex]);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::Identifier);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 2u);
}

TEST(Parse, StatementWithMixedLiteralArgumentsTest)
{
	// FloatLiteral 内容不含 'f' 后缀；StringLiteral 内容不含引号
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR(R"(result = test.op -1.5f -2.0 "hello" true;)"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	struct ExpectedArg
	{
		const Char* content;
		CommonParser::Lexer::TokenType type;
	};
	ExpectedArg expected[] = {
		{SGE_STR("-1.5"), CommonParser::Lexer::TokenTypes::FloatLiteral},
		{SGE_STR("-2.0"), CommonParser::Lexer::TokenTypes::DoubleLiteral},
		{SGE_STR("hello"), CommonParser::Lexer::TokenTypes::StringLiteral},
		{SGE_STR("true"), CommonIntermediateRepresentation::Assembler::TokenTypes::BooleanLiteral},
	};

	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 4u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expected[argIndex].content);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), expected[argIndex].type);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 4u);
}

TEST(Parse, StatementWithEmptyArgumentListTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.call ();"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 仅有 1 个 Argument（它本身是一个 ArgumentList）
	SizeType argumentCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto&) { ++argumentCount; });
	ASSERT_EQ(argumentCount, 1);

	// ArgumentList 首 token 为 '('，末 token 前一个为 ')'，内部无 Argument
	SizeType argumentListCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("ArgumentList"), result.m_First.Get(), [&](const auto& node) {
		++argumentListCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("("));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::LeftBracket);
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR(")"));
		ASSERT_EQ(endToken->GetType(), CommonParser::Lexer::TokenTypes::RightBracket);

		SizeType innerArgCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("Argument"), node, [&](const auto&) { ++innerArgCount; });
		ASSERT_EQ(innerArgCount, 0);
	});
	ASSERT_EQ(argumentListCount, 1);
}

TEST(Parse, StatementWithArgumentListTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.call (1, 2);"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	SizeType argumentListCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("ArgumentList"), result.m_First.Get(), [&](const auto& node) {
		++argumentListCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("("));
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR(")"));

		// 两个整数参数：依次为 "1"、"2"
		const Char* expectedContents[] = {SGE_STR("1"), SGE_STR("2")};
		SizeType innerArgIndex = 0;
		AbstractSyntaxTree::Visit(SGE_STR("Argument"), node, [&](const auto& argNode) {
			ASSERT_LT(innerArgIndex, 2u);
			ASSERT_EQ(argNode.GetBeginTokenIter()->GetContent(), expectedContents[innerArgIndex]);
			ASSERT_EQ(argNode.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::IntegerLiteral);
			++innerArgIndex;
		});
		ASSERT_EQ(innerArgIndex, 2u);
	});
	ASSERT_EQ(argumentListCount, 1);
}

TEST(Parse, StatementWithEmptyBlockTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.func { };"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	SizeType blockCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Block"), result.m_First.Get(), [&](const auto& node) {
		++blockCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("{"));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::LeftCurlyBracket);
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR("}"));
		ASSERT_EQ(endToken->GetType(), CommonParser::Lexer::TokenTypes::RightCurlyBracket);

		SizeType innerStatementCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("Statement"), node, [&](const auto&) { ++innerStatementCount; });
		ASSERT_EQ(innerStatementCount, 0);
	});
	ASSERT_EQ(blockCount, 1);
}

TEST(Parse, StatementWithBlockTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.func { inner = test.inner; };"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	SizeType blockCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Block"), result.m_First.Get(), [&](const auto& node) {
		++blockCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("{"));
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR("}"));

		// Block 内有 1 条 Statement
		SizeType innerStatementCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("Statement"), node, [&](const auto&) { ++innerStatementCount; });
		ASSERT_EQ(innerStatementCount, 1);

		// 内层 Statement 的 VariableIdentifier 为 "inner"，Symbol 为 "test.inner"
		SizeType innerVarIdCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), node, [&](const auto& varNode) {
			++innerVarIdCount;
			ASSERT_EQ(varNode.GetBeginTokenIter()->GetContent(), SGE_STR("inner"));
		});
		ASSERT_EQ(innerVarIdCount, 1);

		SizeType innerSymbolCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("Symbol"), node, [&](const auto& symNode) {
			++innerSymbolCount;
			ASSERT_EQ(symNode.GetBeginTokenIter()->GetContent(), SGE_STR("test.inner"));
		});
		ASSERT_EQ(innerSymbolCount, 1);
	});
	ASSERT_EQ(blockCount, 1);
}

TEST(Parse, StatementWithEmptyAttributeDictionaryTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = [] test.op;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	SizeType attrDictCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("AttributeDictionary"), result.m_First.Get(), [&](const auto& node) {
		++attrDictCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("["));
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::LeftSquareBracket);
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR("]"));
		ASSERT_EQ(endToken->GetType(), CommonParser::Lexer::TokenTypes::RightSquareBracket);

		SizeType attrDefCount = 0;
		AbstractSyntaxTree::Visit(SGE_STR("AttributeDefinition"), node, [&](const auto&) { ++attrDefCount; });
		ASSERT_EQ(attrDefCount, 0);
	});
	ASSERT_EQ(attrDictCount, 1);
}

TEST(Parse, StatementWithAttributeDictionaryTest)
{
	// StringLiteral 的 token 内容不含引号，BooleanLiteral 为 "true"/"false"
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR(R"(result = [attr=42, flag=true, name="hello"] test.op;)"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	struct ExpectedAttr
	{
		const Char* key;
		const Char* value;
		CommonParser::Lexer::TokenType valueType;
	};
	ExpectedAttr expectedAttrs[] = {
		{SGE_STR("attr"), SGE_STR("42"), CommonParser::Lexer::TokenTypes::IntegerLiteral},
		{SGE_STR("flag"), SGE_STR("true"), CommonIntermediateRepresentation::Assembler::TokenTypes::BooleanLiteral},
		{SGE_STR("name"), SGE_STR("hello"), CommonParser::Lexer::TokenTypes::StringLiteral},
	};

	SizeType attrDictCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("AttributeDictionary"), result.m_First.Get(), [&](const auto& node) {
		++attrDictCount;
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("["));
		auto endToken = node.GetEndTokenIter() - 1;
		ASSERT_EQ(endToken->GetContent(), SGE_STR("]"));

		// 三个 AttributeDefinition，每个首 token 为属性键，第三个 token 为属性值
		SizeType attrDefIndex = 0;
		AbstractSyntaxTree::Visit(SGE_STR("AttributeDefinition"), node, [&](const auto& defNode) {
			ASSERT_LT(attrDefIndex, 3u);
			// 首 token：属性键（Identifier）
			ASSERT_EQ(defNode.GetBeginTokenIter()->GetContent(), expectedAttrs[attrDefIndex].key);
			ASSERT_EQ(defNode.GetBeginTokenIter()->GetType(), CommonParser::Lexer::TokenTypes::Identifier);
			// 第三个 token（跳过键和 '='）：属性值
			auto valueToken = defNode.GetBeginTokenIter() + 2;
			ASSERT_EQ(valueToken->GetContent(), expectedAttrs[attrDefIndex].value);
			ASSERT_EQ(valueToken->GetType(), expectedAttrs[attrDefIndex].valueType);
			++attrDefIndex;
		});
		ASSERT_EQ(attrDefIndex, 3u);
	});
	ASSERT_EQ(attrDictCount, 1);
}

TEST(Parse, StatementWithNestedAttributeDictionaryTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = [meta=[x=1]] test.op;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);

	// 共两个 AttributeDictionary：外层与内层
	SizeType attrDictCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("AttributeDictionary"), result.m_First.Get(), [&](const auto&) { ++attrDictCount; });
	ASSERT_EQ(attrDictCount, 2);

	// 外层 AttributeDefinition 键为 "meta"，其 AttributeValue 包含内层 AttributeDictionary
	SizeType outerAttrDefCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("AttributeDefinition"), result.m_First.Get(), [&](const auto& defNode) {
		if (outerAttrDefCount == 0)
		{
			// 外层定义键为 "meta"
			ASSERT_EQ(defNode.GetBeginTokenIter()->GetContent(), SGE_STR("meta"));
		}
		else
		{
			// 内层定义键为 "x"，值为整数 "1"
			ASSERT_EQ(defNode.GetBeginTokenIter()->GetContent(), SGE_STR("x"));
			auto valueToken = defNode.GetBeginTokenIter() + 2;
			ASSERT_EQ(valueToken->GetContent(), SGE_STR("1"));
			ASSERT_EQ(valueToken->GetType(), CommonParser::Lexer::TokenTypes::IntegerLiteral);
		}
		++outerAttrDefCount;
	});
	ASSERT_EQ(outerAttrDefCount, 2u);
}

TEST(Parse, MultipleStatementsTest)
{
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR(R"(
a = test.first 1;
b = test.second a;
)"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 0);
	ASSERT_TRUE(IsSameCString(result.m_First.Get().GetName(), SGE_STR("Program")));

	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 2);

	// VariableIdentifier 依次为 "a"（第一条 LHS）、"b"（第二条 LHS）、"a"（第二条 Argument）
	const Char* expectedVarIds[] = {SGE_STR("a"), SGE_STR("b"), SGE_STR("a")};
	SizeType varIdIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("VariableIdentifier"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(varIdIndex, 3u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedVarIds[varIdIndex]);
		++varIdIndex;
	});
	ASSERT_EQ(varIdIndex, 3u);

	// Symbol 依次为 "test.first"、"test.second"
	const Char* expectedSymbols[] = {SGE_STR("test.first"), SGE_STR("test.second")};
	SizeType symIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(symIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedSymbols[symIndex]);
		++symIndex;
	});
	ASSERT_EQ(symIndex, 2u);

	// Argument 依次为整数 "1" 和标识符 "a"
	struct ExpectedArg
	{
		const Char* content;
		CommonParser::Lexer::TokenType type;
	};
	ExpectedArg expectedArgs[] = {
		{SGE_STR("1"), CommonParser::Lexer::TokenTypes::IntegerLiteral},
		{SGE_STR("a"), CommonParser::Lexer::TokenTypes::Identifier},
	};
	SizeType argIndex = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Argument"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_LT(argIndex, 2u);
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), expectedArgs[argIndex].content);
		ASSERT_EQ(node.GetBeginTokenIter()->GetType(), expectedArgs[argIndex].type);
		++argIndex;
	});
	ASSERT_EQ(argIndex, 2u);
}

TEST(Parse, MissingSemicolonErrorTest)
{
	// 缺少分号时，Statement panic mode 静默恢复，Repeat 匹配 0 条 Statement，Program 仍有效
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR("result = test.op"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	// iter 到达末尾时 panic mode 不会触发，Repeat 匹配 0 次，错误被丢弃；
	// 但 token 流中的 token 仍未被消耗，报告 RemainingTokensAfterParsing
	ASSERT_EQ(result.m_Second.GetSize(), 1);
	ASSERT_EQ(result.m_Second[0].GetTypeId(), TopDownParser::ErrorTypeId::RemainingTokensAfterParsing);
	ASSERT_EQ(result.m_Second[0].GetLine(), 1);
	ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
	ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);

	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 0);
}

TEST(Parse, PanicModeRecoveryTest)
{
	// "bad = not_a_symbol" 中 Symbol 位置出现 Identifier，Statement 触发 panic mode
	// panic mode 跳过错误 token 后从 "valid = test.op;" 成功恢复
	auto lexResult = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(
		SGE_STR("bad = not_a_symbol valid = test.op;"));
	ASSERT_EQ(lexResult.m_Second.GetSize(), 0);

	auto result = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::Parse(lexResult.m_First);
	ASSERT_TRUE(result.m_First.HasValue());
	ASSERT_EQ(result.m_Second.GetSize(), 8);

	// panic mode 成功恢复后 Program 包含 1 条有效 Statement（"valid = test.op;"）
	SizeType statementCount = 0;
	AbstractSyntaxTree::Visit(SGE_STR("Statement"), result.m_First.Get(), [&](const auto&) { ++statementCount; });
	ASSERT_EQ(statementCount, 1);
	AbstractSyntaxTree::Visit(SGE_STR("Symbol"), result.m_First.Get(), [&](const auto& node) {
		ASSERT_EQ(node.GetBeginTokenIter()->GetContent(), SGE_STR("test.op"));
	});

	// 错误 0: RequireExpression("Symbol") — Statement Sequence 在 Symbol 位置匹配失败
	ASSERT_EQ(result.m_Second[0].GetTypeId(), TopDownParser::ErrorTypeId::RequireExpression);
	ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 1);
	ASSERT_EQ(result.m_Second[0].GetAdditionalInformation()[0], SGE_STR("Symbol"));
	ASSERT_EQ(result.m_Second[0].GetLine(), 1);
	ASSERT_EQ(result.m_Second[0].GetColumn(), 7);	 // "not_a_symbol" 的起始列

	// 错误 1: UnexpectedTokenType — 期望 SymbolIdentifier，实际遇到 Identifier
	ASSERT_EQ(result.m_Second[1].GetTypeId(), TopDownParser::ErrorTypeId::UnexpectedTokenType);
	ASSERT_EQ(result.m_Second[1].GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[0], ToString<String>(CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier));
	ASSERT_EQ(result.m_Second[1].GetAdditionalInformation()[1], ToString<String>(CommonParser::Lexer::TokenTypes::Identifier));
	ASSERT_EQ(result.m_Second[1].GetLine(), 1);
	ASSERT_EQ(result.m_Second[1].GetColumn(), 7);

	// 错误 2: EnablePanicMode("Statement") — iter 未到末尾，触发 panic mode，从第一个 token 处开始
	ASSERT_EQ(result.m_Second[2].GetTypeId(), TopDownParser::ErrorTypeId::EnablePanicMode);
	ASSERT_EQ(result.m_Second[2].GetAdditionalInformation().GetSize(), 1);
	ASSERT_EQ(result.m_Second[2].GetAdditionalInformation()[0], SGE_STR("Statement"));
	ASSERT_EQ(result.m_Second[2].GetLine(), 1);
	ASSERT_EQ(result.m_Second[2].GetColumn(), 1);	 // Statement 起始位置 "bad"

	// 错误 3-4: 跳过 "bad"，在 "=" 处尝试 Statement；
	// Optional<Sequence<VariableIdentifierList, Equal>> 因 VariableIdentifierList 需要 Identifier 而 "=" 不是，
	// Optional 静默吞掉内部失败，随后 Symbol 在 "=" 处失败。
	ASSERT_EQ(result.m_Second[3].GetTypeId(), TopDownParser::ErrorTypeId::RequireExpression);
	ASSERT_EQ(result.m_Second[3].GetAdditionalInformation()[0], SGE_STR("Symbol"));
	ASSERT_EQ(result.m_Second[3].GetLine(), 1);
	ASSERT_EQ(result.m_Second[3].GetColumn(), 5);	 // "=" 的列
	ASSERT_EQ(result.m_Second[4].GetTypeId(), TopDownParser::ErrorTypeId::UnexpectedTokenType);
	ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[0], ToString<String>(CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier));
	ASSERT_EQ(result.m_Second[4].GetAdditionalInformation()[1], ToString<String>(CommonParser::Lexer::TokenTypes::Equal));
	ASSERT_EQ(result.m_Second[4].GetLine(), 1);
	ASSERT_EQ(result.m_Second[4].GetColumn(), 5);

	// 错误 5-6: 跳过 "="，在 "not_a_symbol" 处尝试 Statement；
	// Optional<Sequence<VariableIdentifierList, Equal>> 因 Equal 在 "valid" 处失败而静默吞掉，
	// 随后 Symbol 在 "not_a_symbol"（Identifier）处失败。
	ASSERT_EQ(result.m_Second[5].GetTypeId(), TopDownParser::ErrorTypeId::RequireExpression);
	ASSERT_EQ(result.m_Second[5].GetAdditionalInformation()[0], SGE_STR("Symbol"));
	ASSERT_EQ(result.m_Second[5].GetLine(), 1);
	ASSERT_EQ(result.m_Second[5].GetColumn(), 7);	 // "not_a_symbol" 的列
	ASSERT_EQ(result.m_Second[6].GetTypeId(), TopDownParser::ErrorTypeId::UnexpectedTokenType);
	ASSERT_EQ(result.m_Second[6].GetAdditionalInformation()[0], ToString<String>(CommonIntermediateRepresentation::Assembler::TokenTypes::SymbolIdentifier));
	ASSERT_EQ(result.m_Second[6].GetAdditionalInformation()[1], ToString<String>(CommonParser::Lexer::TokenTypes::Identifier));
	ASSERT_EQ(result.m_Second[6].GetLine(), 1);
	ASSERT_EQ(result.m_Second[6].GetColumn(), 7);

	// 错误 7: DisablePanicMode("Statement") — 在 "valid" 处成功恢复
	ASSERT_EQ(result.m_Second[7].GetTypeId(), TopDownParser::ErrorTypeId::DisablePanicMode);
	ASSERT_EQ(result.m_Second[7].GetAdditionalInformation().GetSize(), 1);
	ASSERT_EQ(result.m_Second[7].GetAdditionalInformation()[0], SGE_STR("Statement"));
}
