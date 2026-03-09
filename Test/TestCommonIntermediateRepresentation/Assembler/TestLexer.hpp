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
#include "Assembler/Lexer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser::Lexer;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

TEST(GetTokens, Test)
{
	auto result1 = SpaceGameEngine::CommonIntermediateRepresentation::Assembler::GetTokens(SGE_STR(R"(test.flag = true
//test comment line
/*test comment block
*/
test.negative_integer = -123
test.negative_float = -123.456f
test.negative_double = -123.456789
)"));
	ASSERT_EQ(result1.GetSize(), 12);
	ASSERT_EQ(result1[0].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result1[0].GetContent(), SGE_STR("test.flag"));
	ASSERT_EQ(result1[0].GetLine(), 1);
	ASSERT_EQ(result1[0].GetColumn(), 1);
	ASSERT_EQ(result1[1].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Equal);
	ASSERT_EQ(result1[1].GetContent(), SGE_STR("="));
	ASSERT_EQ(result1[1].GetLine(), 1);
	ASSERT_EQ(result1[1].GetColumn(), 11);
	ASSERT_EQ(result1[2].GetType(), SpaceGameEngine::CommonIntermediateRepresentation::Assembler::TokenTypes::BooleanLiteral);
	ASSERT_EQ(result1[2].GetContent(), SGE_STR("true"));
	ASSERT_EQ(result1[2].GetLine(), 1);
	ASSERT_EQ(result1[2].GetColumn(), 13);
	ASSERT_EQ(result1[3].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result1[3].GetContent(), SGE_STR("test.negative_integer"));
	ASSERT_EQ(result1[3].GetLine(), 5);
	ASSERT_EQ(result1[3].GetColumn(), 1);
	ASSERT_EQ(result1[4].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Equal);
	ASSERT_EQ(result1[4].GetContent(), SGE_STR("="));
	ASSERT_EQ(result1[4].GetLine(), 5);
	ASSERT_EQ(result1[4].GetColumn(), 23);
	ASSERT_EQ(result1[5].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(result1[5].GetContent(), SGE_STR("-123"));
	ASSERT_EQ(result1[5].GetLine(), 5);
	ASSERT_EQ(result1[5].GetColumn(), 25);
	ASSERT_EQ(result1[6].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result1[6].GetContent(), SGE_STR("test.negative_float"));
	ASSERT_EQ(result1[6].GetLine(), 6);
	ASSERT_EQ(result1[6].GetColumn(), 1);
	ASSERT_EQ(result1[7].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Equal);
	ASSERT_EQ(result1[7].GetContent(), SGE_STR("="));
	ASSERT_EQ(result1[7].GetLine(), 6);
	ASSERT_EQ(result1[7].GetColumn(), 21);
	ASSERT_EQ(result1[8].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(result1[8].GetContent(), SGE_STR("-123.456"));
	ASSERT_EQ(result1[8].GetLine(), 6);
	ASSERT_EQ(result1[8].GetColumn(), 23);
	ASSERT_EQ(result1[9].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Identifier);
	ASSERT_EQ(result1[9].GetContent(), SGE_STR("test.negative_double"));
	ASSERT_EQ(result1[9].GetLine(), 7);
	ASSERT_EQ(result1[9].GetColumn(), 1);
	ASSERT_EQ(result1[10].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::Equal);
	ASSERT_EQ(result1[10].GetContent(), SGE_STR("="));
	ASSERT_EQ(result1[10].GetLine(), 7);
	ASSERT_EQ(result1[10].GetColumn(), 22);
	ASSERT_EQ(result1[11].GetType(), SpaceGameEngine::CommonParser::Lexer::TokenTypes::DoubleLiteral);
	ASSERT_EQ(result1[11].GetContent(), SGE_STR("-123.456789"));
	ASSERT_EQ(result1[11].GetLine(), 7);
	ASSERT_EQ(result1[11].GetColumn(), 24);
}