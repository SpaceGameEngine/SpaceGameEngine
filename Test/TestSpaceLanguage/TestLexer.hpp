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

TEST(EscapeCharacterSet, Test)
{
	auto& ecs = Lexer::EscapeCharacterSet::GetSingleton();
	ASSERT_EQ(ecs.Translate(SGE_STR('n')), SGE_STR('\n'));
	ASSERT_EQ(ecs.Translate(SGE_STR('r')), SGE_STR('\r'));
	ASSERT_EQ(ecs.Translate(SGE_STR('t')), SGE_STR('\t'));
	ASSERT_EQ(ecs.Translate(SGE_STR('\'')), SGE_STR('\''));
	ASSERT_EQ(ecs.Translate(SGE_STR('"')), SGE_STR('"'));
	ASSERT_EQ(ecs.Translate(SGE_STR('0')), SGE_STR('\0'));

	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('n')));
	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('r')));
	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('t')));
	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('\'')));
	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('"')));
	ASSERT_TRUE(ecs.IsEscapeCharacter(SGE_STR('0')));
	ASSERT_FALSE(ecs.IsEscapeCharacter(SGE_STR('a')));
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
	ASSERT_TRUE(sm.Judge(SGE_STR(R"('')"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"("test string 123.321")"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR(R"("test string\n")"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR(R"("test string\n)"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("\"test string\\"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"(test\\a)\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"("), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"()"), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"()\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("Rest"), formatter));
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

TEST(StateMachine, Test)
{
	Lexer::StateMachine& sm = Lexer::StateMachine::GetSingleton();

	auto res1 = sm.Run(SGE_STR("this is\ta_test_for R\r\n\r\n123"));
	ASSERT_EQ(res1.GetSize(), 10);
	ASSERT_EQ(res1[0].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[0].m_Content, SGE_STR("this"));
	ASSERT_EQ(res1[1].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[1].m_Content, SGE_STR(" "));
	ASSERT_EQ(res1[2].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[2].m_Content, SGE_STR("is"));
	ASSERT_EQ(res1[3].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[3].m_Content, SGE_STR("\t"));
	ASSERT_EQ(res1[4].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[4].m_Content, SGE_STR("a_test_for"));
	ASSERT_EQ(res1[5].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[5].m_Content, SGE_STR(" "));
	ASSERT_EQ(res1[6].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[6].m_Content, SGE_STR("R"));
	ASSERT_EQ(res1[7].m_Type, Lexer::TokenType::LineSeparator);
	ASSERT_EQ(res1[7].m_Content, SGE_STR("\r\n"));
	ASSERT_EQ(res1[8].m_Type, Lexer::TokenType::LineSeparator);
	ASSERT_EQ(res1[8].m_Content, SGE_STR("\r\n"));
	ASSERT_EQ(res1[9].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res1[9].m_Content, SGE_STR("123"));

	auto res2 = sm.Run(SGE_STR("\r\n\r\n\r\n"));
	ASSERT_EQ(res2.GetSize(), 3);
	for (auto i = 0; i < 3; ++i)
	{
		ASSERT_EQ(res2[i].m_Type, Lexer::TokenType::LineSeparator);
		ASSERT_EQ(res2[i].m_Content, SGE_STR("\r\n"));
	}

	auto res3 = sm.Run(SGE_STR("123.12"));
	ASSERT_EQ(res3.GetSize(), 1);
	ASSERT_EQ(res3[0].m_Type, Lexer::TokenType::DoubleLiteral);
	ASSERT_EQ(res3[0].m_Content, SGE_STR("123.12"));

	auto res4 = sm.Run(SGE_STR("123.45f"));
	ASSERT_EQ(res4.GetSize(), 1);
	ASSERT_EQ(res4[0].m_Type, Lexer::TokenType::FloatLiteral);
	ASSERT_EQ(res4[0].m_Content, SGE_STR("123.45"));

	auto res5 = sm.Run(SGE_STR("0xA02f+0b1011"));
	ASSERT_EQ(res5.GetSize(), 3);
	ASSERT_EQ(res5[0].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res5[0].m_Content, SGE_STR("0xA02f"));
	ASSERT_EQ(res5[1].m_Type, Lexer::TokenType::Add);
	ASSERT_EQ(res5[1].m_Content, SGE_STR("+"));
	ASSERT_EQ(res5[2].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res5[2].m_Content, SGE_STR("0b1011"));

	auto res6 = sm.Run(SGE_STR("//test comment line"));
	ASSERT_EQ(res6.GetSize(), 1);
	ASSERT_EQ(res6[0].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res6[0].m_Content, SGE_STR("test comment line"));

	auto res7 = sm.Run(SGE_STR("/*test comment block*/a"));
	ASSERT_EQ(res7.GetSize(), 2);
	ASSERT_EQ(res7[0].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res7[0].m_Content, SGE_STR("test comment block"));
	ASSERT_EQ(res7[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res7[1].m_Content, SGE_STR("a"));

	auto res8 = sm.Run(SGE_STR("1/b//"));
	ASSERT_EQ(res8.GetSize(), 4);
	ASSERT_EQ(res8[0].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res8[0].m_Content, SGE_STR("1"));
	ASSERT_EQ(res8[1].m_Type, Lexer::TokenType::Slash);
	ASSERT_EQ(res8[1].m_Content, SGE_STR("/"));
	ASSERT_EQ(res8[2].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res8[2].m_Content, SGE_STR("b"));
	ASSERT_EQ(res8[3].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res8[3].m_Content, SGE_STR(""));

	auto res9 = sm.Run(SGE_STR("'a''\n'"));
	ASSERT_EQ(res9.GetSize(), 2);
	ASSERT_EQ(res9[0].m_Type, Lexer::TokenType::CharacterLiteral);
	ASSERT_EQ(res9[0].m_Content, SGE_STR("a"));
	ASSERT_EQ(res9[1].m_Type, Lexer::TokenType::CharacterLiteral);
	ASSERT_EQ(res9[1].m_Content, SGE_STR("\n"));

	auto res10 = sm.Run(SGE_STR("\"test string\ttest\"b"));
	ASSERT_EQ(res10.GetSize(), 2);
	ASSERT_EQ(res10[0].m_Type, Lexer::TokenType::StringLiteral);
	ASSERT_EQ(res10[0].m_Content, SGE_STR("test string\ttest"));
	ASSERT_EQ(res10[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res10[1].m_Content, SGE_STR("b"));

	auto res11 = sm.Run(SGE_STR("R\"(test string\\ttest)\"R"));
	ASSERT_EQ(res11.GetSize(), 2);
	ASSERT_EQ(res11[0].m_Type, Lexer::TokenType::StringLiteral);
	ASSERT_EQ(res11[0].m_Content, SGE_STR("test string\\ttest"));
	ASSERT_EQ(res11[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res11[1].m_Content, SGE_STR("R"));
}

TEST(GetTokens, Test)
{
	String formatter(SGE_STR("line:{} column:{}, {}"));

	auto res1 = Lexer::GetTokens(SGE_STR("this is\ta_test_for R\r\n\r\n123"), formatter);
	ASSERT_EQ(res1.GetSize(), 10);
	ASSERT_EQ(res1[0].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[0].m_Content, SGE_STR("this"));
	ASSERT_EQ(res1[1].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[1].m_Content, SGE_STR(" "));
	ASSERT_EQ(res1[2].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[2].m_Content, SGE_STR("is"));
	ASSERT_EQ(res1[3].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[3].m_Content, SGE_STR("\t"));
	ASSERT_EQ(res1[4].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[4].m_Content, SGE_STR("a_test_for"));
	ASSERT_EQ(res1[5].m_Type, Lexer::TokenType::WordSeparator);
	ASSERT_EQ(res1[5].m_Content, SGE_STR(" "));
	ASSERT_EQ(res1[6].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res1[6].m_Content, SGE_STR("R"));
	ASSERT_EQ(res1[7].m_Type, Lexer::TokenType::LineSeparator);
	ASSERT_EQ(res1[7].m_Content, SGE_STR("\r\n"));
	ASSERT_EQ(res1[8].m_Type, Lexer::TokenType::LineSeparator);
	ASSERT_EQ(res1[8].m_Content, SGE_STR("\r\n"));
	ASSERT_EQ(res1[9].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res1[9].m_Content, SGE_STR("123"));

	auto res2 = Lexer::GetTokens(SGE_STR("\r\n\r\n\r\n"), formatter);
	ASSERT_EQ(res2.GetSize(), 3);
	for (auto i = 0; i < 3; ++i)
	{
		ASSERT_EQ(res2[i].m_Type, Lexer::TokenType::LineSeparator);
		ASSERT_EQ(res2[i].m_Content, SGE_STR("\r\n"));
	}

	auto res3 = Lexer::GetTokens(SGE_STR("123.12"), formatter);
	ASSERT_EQ(res3.GetSize(), 1);
	ASSERT_EQ(res3[0].m_Type, Lexer::TokenType::DoubleLiteral);
	ASSERT_EQ(res3[0].m_Content, SGE_STR("123.12"));

	auto res4 = Lexer::GetTokens(SGE_STR("123.45f"), formatter);
	ASSERT_EQ(res4.GetSize(), 1);
	ASSERT_EQ(res4[0].m_Type, Lexer::TokenType::FloatLiteral);
	ASSERT_EQ(res4[0].m_Content, SGE_STR("123.45"));

	auto res5 = Lexer::GetTokens(SGE_STR("0xA02f+0b1011"), formatter);
	ASSERT_EQ(res5.GetSize(), 3);
	ASSERT_EQ(res5[0].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res5[0].m_Content, SGE_STR("0xA02f"));
	ASSERT_EQ(res5[1].m_Type, Lexer::TokenType::Add);
	ASSERT_EQ(res5[1].m_Content, SGE_STR("+"));
	ASSERT_EQ(res5[2].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res5[2].m_Content, SGE_STR("0b1011"));

	auto res6 = Lexer::GetTokens(SGE_STR("//test comment line"), formatter);
	ASSERT_EQ(res6.GetSize(), 1);
	ASSERT_EQ(res6[0].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res6[0].m_Content, SGE_STR("test comment line"));

	auto res7 = Lexer::GetTokens(SGE_STR("/*test comment block*/a"), formatter);
	ASSERT_EQ(res7.GetSize(), 2);
	ASSERT_EQ(res7[0].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res7[0].m_Content, SGE_STR("test comment block"));
	ASSERT_EQ(res7[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res7[1].m_Content, SGE_STR("a"));

	auto res8 = Lexer::GetTokens(SGE_STR("1/b//"), formatter);
	ASSERT_EQ(res8.GetSize(), 4);
	ASSERT_EQ(res8[0].m_Type, Lexer::TokenType::IntegerLiteral);
	ASSERT_EQ(res8[0].m_Content, SGE_STR("1"));
	ASSERT_EQ(res8[1].m_Type, Lexer::TokenType::Slash);
	ASSERT_EQ(res8[1].m_Content, SGE_STR("/"));
	ASSERT_EQ(res8[2].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res8[2].m_Content, SGE_STR("b"));
	ASSERT_EQ(res8[3].m_Type, Lexer::TokenType::Comment);
	ASSERT_EQ(res8[3].m_Content, SGE_STR(""));

	auto res9 = Lexer::GetTokens(SGE_STR("'a''\n'"), formatter);
	ASSERT_EQ(res9.GetSize(), 2);
	ASSERT_EQ(res9[0].m_Type, Lexer::TokenType::CharacterLiteral);
	ASSERT_EQ(res9[0].m_Content, SGE_STR("a"));
	ASSERT_EQ(res9[1].m_Type, Lexer::TokenType::CharacterLiteral);
	ASSERT_EQ(res9[1].m_Content, SGE_STR("\n"));

	auto res10 = Lexer::GetTokens(SGE_STR("\"test string\ttest\"b"), formatter);
	ASSERT_EQ(res10.GetSize(), 2);
	ASSERT_EQ(res10[0].m_Type, Lexer::TokenType::StringLiteral);
	ASSERT_EQ(res10[0].m_Content, SGE_STR("test string\ttest"));
	ASSERT_EQ(res10[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res10[1].m_Content, SGE_STR("b"));

	auto res11 = Lexer::GetTokens(SGE_STR("R\"(test string\\ttest)\"R"), formatter);
	ASSERT_EQ(res11.GetSize(), 2);
	ASSERT_EQ(res11[0].m_Type, Lexer::TokenType::StringLiteral);
	ASSERT_EQ(res11[0].m_Content, SGE_STR("test string\\ttest"));
	ASSERT_EQ(res11[1].m_Type, Lexer::TokenType::Identifier);
	ASSERT_EQ(res11[1].m_Content, SGE_STR("R"));
}