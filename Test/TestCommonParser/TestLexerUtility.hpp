/*
Copyright 2025 creatorlxd

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

TEST(CombineToken, Test)
{
	String formatter(SGE_STR("line:{} column:{}, {}"));
	auto tokens = Lexer::GetTokens(SGE_STR("test.combine_token.test"), formatter);
	ASSERT_EQ(tokens.GetSize(), 5);
	ASSERT_EQ(tokens[0].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[0].GetLine(), 1);
	ASSERT_EQ(tokens[0].GetColumn(), 1);
	ASSERT_EQ(tokens[1].GetType(), Lexer::TokenType::Dot);
	ASSERT_EQ(tokens[1].GetContent(), SGE_STR("."));
	ASSERT_EQ(tokens[1].GetLine(), 1);
	ASSERT_EQ(tokens[1].GetColumn(), 5);
	ASSERT_EQ(tokens[2].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(tokens[2].GetContent(), SGE_STR("combine_token"));
	ASSERT_EQ(tokens[2].GetLine(), 1);
	ASSERT_EQ(tokens[2].GetColumn(), 6);
	ASSERT_EQ(tokens[3].GetType(), Lexer::TokenType::Dot);
	ASSERT_EQ(tokens[3].GetContent(), SGE_STR("."));
	ASSERT_EQ(tokens[3].GetLine(), 1);
	ASSERT_EQ(tokens[3].GetColumn(), 19);
	ASSERT_EQ(tokens[4].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(tokens[4].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[4].GetLine(), 1);
	ASSERT_EQ(tokens[4].GetColumn(), 20);

	auto result_no_repeat = Lexer::CombineToken(tokens, {Lexer::TokenType::Identifier, Lexer::TokenType::Dot, Lexer::TokenType::Identifier}, Lexer::TokenType::Identifier);
	ASSERT_EQ(result_no_repeat.GetSize(), 3);
	ASSERT_EQ(result_no_repeat[0].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(result_no_repeat[0].GetContent(), SGE_STR("test.combine_token"));
	ASSERT_EQ(result_no_repeat[0].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[0].GetColumn(), 1);
	ASSERT_EQ(result_no_repeat[1].GetType(), Lexer::TokenType::Dot);
	ASSERT_EQ(result_no_repeat[1].GetContent(), SGE_STR("."));
	ASSERT_EQ(result_no_repeat[1].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[1].GetColumn(), 19);
	ASSERT_EQ(result_no_repeat[2].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(result_no_repeat[2].GetContent(), SGE_STR("test"));
	ASSERT_EQ(result_no_repeat[2].GetLine(), 1);
	ASSERT_EQ(result_no_repeat[2].GetColumn(), 20);

	auto result_with_repeat = Lexer::CombineToken(tokens, {Lexer::TokenType::Identifier, Lexer::TokenType::Dot, Lexer::TokenType::Identifier}, Lexer::TokenType::Identifier, true);
	ASSERT_EQ(result_with_repeat.GetSize(), 1);
	ASSERT_EQ(result_with_repeat[0].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(result_with_repeat[0].GetContent(), SGE_STR("test.combine_token.test"));
	ASSERT_EQ(result_with_repeat[0].GetLine(), 1);
	ASSERT_EQ(result_with_repeat[0].GetColumn(), 1);
}

TEST(RemoveToken, Test)
{
	String formatter(SGE_STR("line:{} column:{}, {}"));
	auto tokens = Lexer::GetTokens(SGE_STR("test \n123"), formatter);
	ASSERT_EQ(tokens.GetSize(), 4);
	ASSERT_EQ(tokens[0].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(tokens[0].GetLine(), 1);
	ASSERT_EQ(tokens[0].GetColumn(), 1);
	ASSERT_EQ(tokens[1].GetType(), Lexer::TokenType::WordSeparator);
	ASSERT_EQ(tokens[1].GetContent(), SGE_STR(" "));
	ASSERT_EQ(tokens[1].GetLine(), 1);
	ASSERT_EQ(tokens[1].GetColumn(), 5);
	ASSERT_EQ(tokens[2].GetType(), Lexer::TokenType::LineSeparator);
	ASSERT_EQ(tokens[2].GetContent(), SGE_STR("\n"));
	ASSERT_EQ(tokens[2].GetLine(), 1);
	ASSERT_EQ(tokens[2].GetColumn(), 6);
	ASSERT_EQ(tokens[3].GetType(), Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(tokens[3].GetContent(), SGE_STR("123"));
	ASSERT_EQ(tokens[3].GetLine(), 2);
	ASSERT_EQ(tokens[3].GetColumn(), 1);

	auto result = Lexer::RemoveToken(tokens, {Lexer::TokenType::WordSeparator, Lexer::TokenType::LineSeparator});
	ASSERT_EQ(result.GetSize(), 2);
	ASSERT_EQ(result[0].GetType(), Lexer::TokenType::Identifier);
	ASSERT_EQ(result[0].GetContent(), SGE_STR("test"));
	ASSERT_EQ(result[0].GetLine(), 1);
	ASSERT_EQ(result[0].GetColumn(), 1);
	ASSERT_EQ(result[1].GetType(), Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(result[1].GetContent(), SGE_STR("123"));
	ASSERT_EQ(result[1].GetLine(), 2);
	ASSERT_EQ(result[1].GetColumn(), 1);
}