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
*/)"));
	ASSERT_EQ(result1.GetSize(), 3);
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
}