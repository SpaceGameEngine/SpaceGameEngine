﻿/*
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

TEST(StateMachineForJudge, Test)
{
	Lexer::StateMachineForJudge& sm = Lexer::StateMachineForJudge::GetSingleton();
	String formatter(SGE_STR("line:{} column:{}, {}"));

	ASSERT_FALSE(sm.Judge(SGE_STR(""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("identifier_ID12"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123\n234\n"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123\r234\r"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123\r\n234\r\n"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123\r\n234"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123\r234\r\n"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123\n234\r\n"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123\r\n234\r"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123\r\n234\n"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123\r\n234\n\r"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("this is test for 1\r\n 3m m4"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("1 - 0\n1-0\n-100"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("01234"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("0b1001"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("0b1234"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("0x1234"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("0x1A2B3c4f"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123.345"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("123.345f"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("-123.345"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("-123.345f"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123."), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123.f"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("123..345"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"('a')"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"('\r')"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"('\a')"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"(')"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"('s)"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"('\)"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"("test string 123.321")"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"("test string\n")"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"("test string\n)"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"("test string\)"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"(test\\a)\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"("), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"()"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"()\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("/**/"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("/*test*/"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("/*test\ntest2*/"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("//"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("//\n"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("//test"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("//test\n//test\n"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("//test\n//test"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("/*"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("/*\n"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("/**"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("/**\n"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("?\n/\\"), formatter));
}