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
#include "LexerUtility.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(TransformToken, Test)
{
	auto tokens = Lexer::CppLikeStyleLexer::GetTokens(SGE_STR("a=true")).m_First;
	ASSERT_EQ(tokens.GetSize(), 3);
	ASSERT_EQ(tokens[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("a"));
	ASSERT_EQ(tokens[0].GetLine(), 1);
	ASSERT_EQ(tokens[0].GetColumn(), 1);
	ASSERT_EQ(tokens[1].GetType(), Lexer::TokenTypes::Equal);
	ASSERT_EQ(tokens[1].GetContent(), SGE_STR("="));
	ASSERT_EQ(tokens[1].GetLine(), 1);
	ASSERT_EQ(tokens[1].GetColumn(), 2);
	ASSERT_EQ(tokens[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[2].GetContent(), SGE_STR("true"));
	ASSERT_EQ(tokens[2].GetLine(), 1);
	ASSERT_EQ(tokens[2].GetColumn(), 3);

	auto BooleanLiteral = CommonParser::Lexer::TokenTypes::DefaultTokenTypesCount;

	auto result = Lexer::TransformToken(tokens, [BooleanLiteral](const Lexer::Token& token) {
		if (token.GetType() == Lexer::TokenTypes::Identifier)
		{
			if (token.GetContent() == SGE_STR("true") || token.GetContent() == SGE_STR("false"))
			{
				return Lexer::Token(BooleanLiteral, token.GetContent(), token.GetLine(), token.GetColumn());
			}
		}
		return token;
	});

	ASSERT_EQ(result.GetSize(), 3);
	ASSERT_EQ(result[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result[0].GetContent(), SGE_STR("a"));
	ASSERT_EQ(result[0].GetLine(), 1);
	ASSERT_EQ(result[0].GetColumn(), 1);
	ASSERT_EQ(result[1].GetType(), Lexer::TokenTypes::Equal);
	ASSERT_EQ(result[1].GetContent(), SGE_STR("="));
	ASSERT_EQ(result[1].GetLine(), 1);
	ASSERT_EQ(result[1].GetColumn(), 2);
	ASSERT_EQ(result[2].GetType(), BooleanLiteral);
	ASSERT_EQ(result[2].GetContent(), SGE_STR("true"));
	ASSERT_EQ(result[2].GetLine(), 1);
	ASSERT_EQ(result[2].GetColumn(), 3);
}

TEST(CombineToken, Test)
{
	auto tokens = Lexer::CppLikeStyleLexer::GetTokens(SGE_STR("test.combine_token.test")).m_First;
	ASSERT_EQ(tokens.GetSize(), 5);
	ASSERT_EQ(tokens[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[0].GetLine(), 1);
	ASSERT_EQ(tokens[0].GetColumn(), 1);
	ASSERT_EQ(tokens[1].GetType(), Lexer::TokenTypes::Dot);
	ASSERT_EQ(tokens[1].GetContent(), SGE_STR("."));
	ASSERT_EQ(tokens[1].GetLine(), 1);
	ASSERT_EQ(tokens[1].GetColumn(), 5);
	ASSERT_EQ(tokens[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[2].GetContent(), SGE_STR("combine_token"));
	ASSERT_EQ(tokens[2].GetLine(), 1);
	ASSERT_EQ(tokens[2].GetColumn(), 6);
	ASSERT_EQ(tokens[3].GetType(), Lexer::TokenTypes::Dot);
	ASSERT_EQ(tokens[3].GetContent(), SGE_STR("."));
	ASSERT_EQ(tokens[3].GetLine(), 1);
	ASSERT_EQ(tokens[3].GetColumn(), 19);
	ASSERT_EQ(tokens[4].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[4].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[4].GetLine(), 1);
	ASSERT_EQ(tokens[4].GetColumn(), 20);

	auto result_no_repeat = Lexer::CombineToken(tokens, {Lexer::TokenTypes::Identifier, Lexer::TokenTypes::Dot, Lexer::TokenTypes::Identifier}, Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result_no_repeat.GetSize(), 3);
	ASSERT_EQ(result_no_repeat[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result_no_repeat[0].GetContent(), SGE_STR("test.combine_token"));
	ASSERT_EQ(result_no_repeat[0].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[0].GetColumn(), 1);
	ASSERT_EQ(result_no_repeat[1].GetType(), Lexer::TokenTypes::Dot);
	ASSERT_EQ(result_no_repeat[1].GetContent(), SGE_STR("."));
	ASSERT_EQ(result_no_repeat[1].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[1].GetColumn(), 19);
	ASSERT_EQ(result_no_repeat[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result_no_repeat[2].GetContent(), SGE_STR("test"));
	ASSERT_EQ(result_no_repeat[2].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[2].GetColumn(), 20);

	auto result_with_repeat = Lexer::CombineToken(tokens, {Lexer::TokenTypes::Identifier, Lexer::TokenTypes::Dot, Lexer::TokenTypes::Identifier}, Lexer::TokenTypes::Identifier, true);
	ASSERT_EQ(result_with_repeat.GetSize(), 1);
	ASSERT_EQ(result_with_repeat[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result_with_repeat[0].GetContent(), SGE_STR("test.combine_token.test"));
	ASSERT_EQ(result_with_repeat[0].GetLine(), 1);
	ASSERT_EQ(result_with_repeat[0].GetColumn(), 1);
}

TEST(RemoveToken, Test)
{
	auto tokens = Lexer::CppLikeStyleLexer::GetTokens(SGE_STR("test \n123")).m_First;
	ASSERT_EQ(tokens.GetSize(), 4);
	ASSERT_EQ(tokens[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[0].GetLine(), 1);
	ASSERT_EQ(tokens[0].GetColumn(), 1);
	ASSERT_EQ(tokens[1].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(tokens[1].GetContent(), SGE_STR(" "));
	ASSERT_EQ(tokens[1].GetLine(), 1);
	ASSERT_EQ(tokens[1].GetColumn(), 5);
	ASSERT_EQ(tokens[2].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(tokens[2].GetContent(), SGE_STR("\n"));
	ASSERT_EQ(tokens[2].GetLine(), 1);
	ASSERT_EQ(tokens[2].GetColumn(), 6);
	ASSERT_EQ(tokens[3].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(tokens[3].GetContent(), SGE_STR("123"));
	ASSERT_EQ(tokens[3].GetLine(), 2);
	ASSERT_EQ(tokens[3].GetColumn(), 1);

	auto result = Lexer::RemoveToken(tokens, {Lexer::TokenTypes::WordSeparator, Lexer::TokenTypes::LineSeparator});
	ASSERT_EQ(result.GetSize(), 2);
	ASSERT_EQ(result[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(result[0].GetLine(), 1);
	ASSERT_EQ(result[0].GetColumn(), 1);
	ASSERT_EQ(result[1].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(result[1].GetContent(), SGE_STR("123"));
	ASSERT_EQ(result[1].GetLine(), 2);
	ASSERT_EQ(result[1].GetColumn(), 1);
}