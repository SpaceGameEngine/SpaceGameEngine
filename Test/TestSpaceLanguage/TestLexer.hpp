/*
Copyright 2022 creatorlxd

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
#include "Lexer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(Token, Test)
{
	Lexer::Token token1;
	ASSERT_EQ(token1.m_Type, Lexer::TokenType::Unknown);
	ASSERT_EQ(token1.m_Content.GetSize(), 0);

	Lexer::Token token2(Lexer::TokenType::Identifier, SGE_STR("test"));
	ASSERT_EQ(token2.m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(token2.m_Content, SGE_STR("test"));
}

TEST(SymbolSet, Test)
{
	auto& ss = Lexer::SymbolSet::GetSingleton();
	ASSERT_EQ(ss.Get(SGE_STR(';')), Lexer::TokenType::Semicolon);
	ASSERT_EQ(ss.Get(SGE_STR('A')), Lexer::TokenType::Unknown);
	ASSERT_TRUE(ss.IsSymbol(SGE_STR(';')));
	ASSERT_FALSE(ss.IsSymbol(SGE_STR('a')));
}