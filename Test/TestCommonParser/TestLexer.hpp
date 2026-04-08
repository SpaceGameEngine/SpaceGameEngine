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
#include "Lexer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(Token, Test)
{
	Lexer::Token token1;
	ASSERT_EQ(token1.GetType(), Lexer::TokenTypes::Unknown);
	ASSERT_EQ(token1.GetContent().GetSize(), 0);

	Lexer::Token token2(Lexer::TokenTypes::Identifier, SGE_STR("test"), 2, 4);
	ASSERT_EQ(token2.GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(token2.GetContent(), SGE_STR("test"));
	ASSERT_EQ(token2.GetLine(), 2);
	ASSERT_EQ(token2.GetColumn(), 4);
}

TEST(SymbolSet, Test)
{
	auto& ss = Lexer::SymbolSet::GetSingleton();
	ASSERT_EQ(ss.Get(SGE_STR(';')), Lexer::TokenTypes::Semicolon);
	ASSERT_EQ(ss.Get(SGE_STR('A')), Lexer::TokenTypes::Unknown);
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
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"()\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"(test\\a)\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"1_/a*(test)1_/a*\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"abc(test)bbc\")abc\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"abc(\"bbc(test)bbc\")abc\""), formatter));
	ASSERT_FALSE(sm.Judge(SGE_STR("R\"abc()123)abc\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"("), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"()"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"123(test)456"), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"123(test)456\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"123(test)\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"(test)456\""), formatter));
	ASSERT_TRUE(sm.Judge(SGE_STR("R\"\"(test)\"\""), formatter));
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
	ASSERT_EQ(res1[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[0].GetContent(), SGE_STR("this"));
	ASSERT_EQ(res1[0].GetLine(), 1);
	ASSERT_EQ(res1[0].GetColumn(), 1);
	ASSERT_EQ(res1[1].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[1].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1[1].GetLine(), 1);
	ASSERT_EQ(res1[1].GetColumn(), 5);
	ASSERT_EQ(res1[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[2].GetContent(), SGE_STR("is"));
	ASSERT_EQ(res1[2].GetLine(), 1);
	ASSERT_EQ(res1[2].GetColumn(), 6);
	ASSERT_EQ(res1[3].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[3].GetContent(), SGE_STR("\t"));
	ASSERT_EQ(res1[3].GetLine(), 1);
	ASSERT_EQ(res1[3].GetColumn(), 8);
	ASSERT_EQ(res1[4].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[4].GetContent(), SGE_STR("a_test_for"));
	ASSERT_EQ(res1[4].GetLine(), 1);
	ASSERT_EQ(res1[4].GetColumn(), 9);
	ASSERT_EQ(res1[5].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[5].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1[5].GetLine(), 1);
	ASSERT_EQ(res1[5].GetColumn(), 19);
	ASSERT_EQ(res1[6].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[6].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res1[6].GetLine(), 1);
	ASSERT_EQ(res1[6].GetColumn(), 20);
	ASSERT_EQ(res1[7].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1[7].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1[7].GetLine(), 1);
	ASSERT_EQ(res1[7].GetColumn(), 21);
	ASSERT_EQ(res1[8].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1[8].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1[8].GetLine(), 2);
	ASSERT_EQ(res1[8].GetColumn(), 1);
	ASSERT_EQ(res1[9].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res1[9].GetContent(), SGE_STR("123"));
	ASSERT_EQ(res1[9].GetLine(), 3);
	ASSERT_EQ(res1[9].GetColumn(), 1);

	auto res2 = sm.Run(SGE_STR("\r\n\r\n\r\n"));
	ASSERT_EQ(res2.GetSize(), 3);
	for (auto i = 0; i < 3; ++i)
	{
		ASSERT_EQ(res2[i].GetType(), Lexer::TokenTypes::LineSeparator);
		ASSERT_EQ(res2[i].GetContent(), SGE_STR("\r\n"));
		ASSERT_EQ(res2[i].GetLine(), i + 1);
		ASSERT_EQ(res2[i].GetColumn(), 1);
	}

	auto res3 = sm.Run(SGE_STR("123.12"));
	ASSERT_EQ(res3.GetSize(), 1);
	ASSERT_EQ(res3[0].GetType(), Lexer::TokenTypes::DoubleLiteral);
	ASSERT_EQ(res3[0].GetContent(), SGE_STR("123.12"));
	ASSERT_EQ(res3[0].GetLine(), 1);
	ASSERT_EQ(res3[0].GetColumn(), 1);

	auto res4 = sm.Run(SGE_STR("123.45f"));
	ASSERT_EQ(res4.GetSize(), 1);
	ASSERT_EQ(res4[0].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res4[0].GetContent(), SGE_STR("123.45"));
	ASSERT_EQ(res4[0].GetLine(), 1);
	ASSERT_EQ(res4[0].GetColumn(), 1);

	auto res5 = sm.Run(SGE_STR("0xA02f+0b1011"));
	ASSERT_EQ(res5.GetSize(), 3);
	ASSERT_EQ(res5[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5[0].GetContent(), SGE_STR("0xA02f"));
	ASSERT_EQ(res5[0].GetLine(), 1);
	ASSERT_EQ(res5[0].GetColumn(), 1);
	ASSERT_EQ(res5[1].GetType(), Lexer::TokenTypes::Add);
	ASSERT_EQ(res5[1].GetContent(), SGE_STR("+"));
	ASSERT_EQ(res5[1].GetLine(), 1);
	ASSERT_EQ(res5[1].GetColumn(), 7);
	ASSERT_EQ(res5[2].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5[2].GetContent(), SGE_STR("0b1011"));
	ASSERT_EQ(res5[2].GetLine(), 1);
	ASSERT_EQ(res5[2].GetColumn(), 8);

	auto res6 = sm.Run(SGE_STR("-12--12.345f"));
	ASSERT_EQ(res6.GetSize(), 5);
	ASSERT_EQ(res6[0].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[0].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[0].GetLine(), 1);
	ASSERT_EQ(res6[0].GetColumn(), 1);
	ASSERT_EQ(res6[1].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res6[1].GetContent(), SGE_STR("12"));
	ASSERT_EQ(res6[1].GetLine(), 1);
	ASSERT_EQ(res6[1].GetColumn(), 2);
	ASSERT_EQ(res6[2].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[2].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[2].GetLine(), 1);
	ASSERT_EQ(res6[2].GetColumn(), 4);
	ASSERT_EQ(res6[3].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[3].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[3].GetLine(), 1);
	ASSERT_EQ(res6[3].GetColumn(), 5);
	ASSERT_EQ(res6[4].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res6[4].GetContent(), SGE_STR("12.345"));
	ASSERT_EQ(res6[4].GetLine(), 1);
	ASSERT_EQ(res6[4].GetColumn(), 6);

	auto res7 = sm.Run(SGE_STR("//test comment line"));
	ASSERT_EQ(res7.GetSize(), 1);
	ASSERT_EQ(res7[0].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res7[0].GetContent(), SGE_STR("test comment line"));
	ASSERT_EQ(res7[0].GetLine(), 1);
	ASSERT_EQ(res7[0].GetColumn(), 1);

	auto res8 = sm.Run(SGE_STR("/*test comment block*/a"));
	ASSERT_EQ(res8.GetSize(), 2);
	ASSERT_EQ(res8[0].GetType(), Lexer::TokenTypes::CommentBlock);
	ASSERT_EQ(res8[0].GetContent(), SGE_STR("test comment block"));
	ASSERT_EQ(res8[0].GetLine(), 1);
	ASSERT_EQ(res8[0].GetColumn(), 1);
	ASSERT_EQ(res8[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res8[1].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res8[1].GetLine(), 1);
	ASSERT_EQ(res8[1].GetColumn(), 23);

	auto res9 = sm.Run(SGE_STR("1/b//"));
	ASSERT_EQ(res9.GetSize(), 4);
	ASSERT_EQ(res9[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res9[0].GetContent(), SGE_STR("1"));
	ASSERT_EQ(res9[0].GetLine(), 1);
	ASSERT_EQ(res9[0].GetColumn(), 1);
	ASSERT_EQ(res9[1].GetType(), Lexer::TokenTypes::Slash);
	ASSERT_EQ(res9[1].GetContent(), SGE_STR("/"));
	ASSERT_EQ(res9[1].GetLine(), 1);
	ASSERT_EQ(res9[1].GetColumn(), 2);
	ASSERT_EQ(res9[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res9[2].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res9[2].GetLine(), 1);
	ASSERT_EQ(res9[2].GetColumn(), 3);
	ASSERT_EQ(res9[3].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res9[3].GetContent(), SGE_STR(""));
	ASSERT_EQ(res9[3].GetLine(), 1);
	ASSERT_EQ(res9[3].GetColumn(), 4);

	auto res10 = sm.Run(SGE_STR("'a''\n'"));
	ASSERT_EQ(res10.GetSize(), 2);
	ASSERT_EQ(res10[0].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10[0].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res10[0].GetLine(), 1);
	ASSERT_EQ(res10[0].GetColumn(), 1);
	ASSERT_EQ(res10[1].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10[1].GetContent(), SGE_STR("\n"));
	ASSERT_EQ(res10[1].GetLine(), 1);
	ASSERT_EQ(res10[1].GetColumn(), 4);

	auto res11 = sm.Run(SGE_STR("\"test string\ttest\"b"));
	ASSERT_EQ(res11.GetSize(), 2);
	ASSERT_EQ(res11[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res11[0].GetContent(), SGE_STR("test string\ttest"));
	ASSERT_EQ(res11[0].GetLine(), 1);
	ASSERT_EQ(res11[0].GetColumn(), 1);
	ASSERT_EQ(res11[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res11[1].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res11[1].GetLine(), 1);
	ASSERT_EQ(res11[1].GetColumn(), 19);

	auto res12 = sm.Run(SGE_STR("R\"(test string\\ttest)\"R"));
	ASSERT_EQ(res12.GetSize(), 2);
	ASSERT_EQ(res12[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res12[0].GetContent(), SGE_STR("test string\\ttest"));
	ASSERT_EQ(res12[0].GetLine(), 1);
	ASSERT_EQ(res12[0].GetColumn(), 1);
	ASSERT_EQ(res12[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res12[1].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res12[1].GetLine(), 1);
	ASSERT_EQ(res12[1].GetColumn(), 23);

	auto res13 = sm.Run(SGE_STR("R\"test_@#'(1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\")test_@#'\""));
	ASSERT_EQ(res13.GetSize(), 1);
	ASSERT_EQ(res13[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res13[0].GetContent(), SGE_STR("1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\""));
	ASSERT_EQ(res13[0].GetLine(), 1);
	ASSERT_EQ(res13[0].GetColumn(), 1);
}

TEST(GetTokens, Test)
{
	String formatter(SGE_STR("line:{} column:{}, {}"));

	auto res1 = Lexer::GetTokens(SGE_STR("this is\ta_test_for R\r\n\r\n123"), formatter);
	ASSERT_EQ(res1.GetSize(), 10);
	ASSERT_EQ(res1[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[0].GetContent(), SGE_STR("this"));
	ASSERT_EQ(res1[0].GetLine(), 1);
	ASSERT_EQ(res1[0].GetColumn(), 1);
	ASSERT_EQ(res1[1].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[1].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1[1].GetLine(), 1);
	ASSERT_EQ(res1[1].GetColumn(), 5);
	ASSERT_EQ(res1[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[2].GetContent(), SGE_STR("is"));
	ASSERT_EQ(res1[2].GetLine(), 1);
	ASSERT_EQ(res1[2].GetColumn(), 6);
	ASSERT_EQ(res1[3].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[3].GetContent(), SGE_STR("\t"));
	ASSERT_EQ(res1[3].GetLine(), 1);
	ASSERT_EQ(res1[3].GetColumn(), 8);
	ASSERT_EQ(res1[4].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[4].GetContent(), SGE_STR("a_test_for"));
	ASSERT_EQ(res1[4].GetLine(), 1);
	ASSERT_EQ(res1[4].GetColumn(), 9);
	ASSERT_EQ(res1[5].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1[5].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1[5].GetLine(), 1);
	ASSERT_EQ(res1[5].GetColumn(), 19);
	ASSERT_EQ(res1[6].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1[6].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res1[6].GetLine(), 1);
	ASSERT_EQ(res1[6].GetColumn(), 20);
	ASSERT_EQ(res1[7].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1[7].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1[7].GetLine(), 1);
	ASSERT_EQ(res1[7].GetColumn(), 21);
	ASSERT_EQ(res1[8].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1[8].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1[8].GetLine(), 2);
	ASSERT_EQ(res1[8].GetColumn(), 1);
	ASSERT_EQ(res1[9].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res1[9].GetContent(), SGE_STR("123"));
	ASSERT_EQ(res1[9].GetLine(), 3);
	ASSERT_EQ(res1[9].GetColumn(), 1);

	auto res2 = Lexer::GetTokens(SGE_STR("\r\n\r\n\r\n"), formatter);
	ASSERT_EQ(res2.GetSize(), 3);
	for (auto i = 0; i < 3; ++i)
	{
		ASSERT_EQ(res2[i].GetType(), Lexer::TokenTypes::LineSeparator);
		ASSERT_EQ(res2[i].GetContent(), SGE_STR("\r\n"));
		ASSERT_EQ(res2[i].GetLine(), i + 1);
		ASSERT_EQ(res2[i].GetColumn(), 1);
	}

	auto res3 = Lexer::GetTokens(SGE_STR("123.12"), formatter);
	ASSERT_EQ(res3.GetSize(), 1);
	ASSERT_EQ(res3[0].GetType(), Lexer::TokenTypes::DoubleLiteral);
	ASSERT_EQ(res3[0].GetContent(), SGE_STR("123.12"));
	ASSERT_EQ(res3[0].GetLine(), 1);
	ASSERT_EQ(res3[0].GetColumn(), 1);

	auto res4 = Lexer::GetTokens(SGE_STR("123.45f"), formatter);
	ASSERT_EQ(res4.GetSize(), 1);
	ASSERT_EQ(res4[0].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res4[0].GetContent(), SGE_STR("123.45"));
	ASSERT_EQ(res4[0].GetLine(), 1);
	ASSERT_EQ(res4[0].GetColumn(), 1);

	auto res5 = Lexer::GetTokens(SGE_STR("0xA02f+0b1011"), formatter);
	ASSERT_EQ(res5.GetSize(), 3);
	ASSERT_EQ(res5[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5[0].GetContent(), SGE_STR("0xA02f"));
	ASSERT_EQ(res5[0].GetLine(), 1);
	ASSERT_EQ(res5[0].GetColumn(), 1);
	ASSERT_EQ(res5[1].GetType(), Lexer::TokenTypes::Add);
	ASSERT_EQ(res5[1].GetContent(), SGE_STR("+"));
	ASSERT_EQ(res5[1].GetLine(), 1);
	ASSERT_EQ(res5[1].GetColumn(), 7);
	ASSERT_EQ(res5[2].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5[2].GetContent(), SGE_STR("0b1011"));
	ASSERT_EQ(res5[2].GetLine(), 1);
	ASSERT_EQ(res5[2].GetColumn(), 8);

	auto res6 = Lexer::GetTokens(SGE_STR("-12--12.345f"), formatter);
	ASSERT_EQ(res6.GetSize(), 5);
	ASSERT_EQ(res6[0].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[0].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[0].GetLine(), 1);
	ASSERT_EQ(res6[0].GetColumn(), 1);
	ASSERT_EQ(res6[1].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res6[1].GetContent(), SGE_STR("12"));
	ASSERT_EQ(res6[1].GetLine(), 1);
	ASSERT_EQ(res6[1].GetColumn(), 2);
	ASSERT_EQ(res6[2].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[2].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[2].GetLine(), 1);
	ASSERT_EQ(res6[2].GetColumn(), 4);
	ASSERT_EQ(res6[3].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6[3].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6[3].GetLine(), 1);
	ASSERT_EQ(res6[3].GetColumn(), 5);
	ASSERT_EQ(res6[4].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res6[4].GetContent(), SGE_STR("12.345"));
	ASSERT_EQ(res6[4].GetLine(), 1);
	ASSERT_EQ(res6[4].GetColumn(), 6);

	auto res7 = Lexer::GetTokens(SGE_STR("//test comment line"), formatter);
	ASSERT_EQ(res7.GetSize(), 1);
	ASSERT_EQ(res7[0].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res7[0].GetContent(), SGE_STR("test comment line"));
	ASSERT_EQ(res7[0].GetLine(), 1);
	ASSERT_EQ(res7[0].GetColumn(), 1);

	auto res8 = Lexer::GetTokens(SGE_STR("/*test comment block*/a"), formatter);
	ASSERT_EQ(res8.GetSize(), 2);
	ASSERT_EQ(res8[0].GetType(), Lexer::TokenTypes::CommentBlock);
	ASSERT_EQ(res8[0].GetContent(), SGE_STR("test comment block"));
	ASSERT_EQ(res8[0].GetLine(), 1);
	ASSERT_EQ(res8[0].GetColumn(), 1);
	ASSERT_EQ(res8[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res8[1].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res8[1].GetLine(), 1);
	ASSERT_EQ(res8[1].GetColumn(), 23);

	auto res9 = Lexer::GetTokens(SGE_STR("1/b//"), formatter);
	ASSERT_EQ(res9.GetSize(), 4);
	ASSERT_EQ(res9[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res9[0].GetContent(), SGE_STR("1"));
	ASSERT_EQ(res9[0].GetLine(), 1);
	ASSERT_EQ(res9[0].GetColumn(), 1);
	ASSERT_EQ(res9[1].GetType(), Lexer::TokenTypes::Slash);
	ASSERT_EQ(res9[1].GetContent(), SGE_STR("/"));
	ASSERT_EQ(res9[1].GetLine(), 1);
	ASSERT_EQ(res9[1].GetColumn(), 2);
	ASSERT_EQ(res9[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res9[2].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res9[2].GetLine(), 1);
	ASSERT_EQ(res9[2].GetColumn(), 3);
	ASSERT_EQ(res9[3].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res9[3].GetContent(), SGE_STR(""));
	ASSERT_EQ(res9[3].GetLine(), 1);
	ASSERT_EQ(res9[3].GetColumn(), 4);

	auto res10 = Lexer::GetTokens(SGE_STR("'a''\n'"), formatter);
	ASSERT_EQ(res10.GetSize(), 2);
	ASSERT_EQ(res10[0].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10[0].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res10[0].GetLine(), 1);
	ASSERT_EQ(res10[0].GetColumn(), 1);
	ASSERT_EQ(res10[1].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10[1].GetContent(), SGE_STR("\n"));
	ASSERT_EQ(res10[1].GetLine(), 1);
	ASSERT_EQ(res10[1].GetColumn(), 4);

	auto res11 = Lexer::GetTokens(SGE_STR("\"test string\ttest\"b"), formatter);
	ASSERT_EQ(res11.GetSize(), 2);
	ASSERT_EQ(res11[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res11[0].GetContent(), SGE_STR("test string\ttest"));
	ASSERT_EQ(res11[0].GetLine(), 1);
	ASSERT_EQ(res11[0].GetColumn(), 1);
	ASSERT_EQ(res11[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res11[1].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res11[1].GetLine(), 1);
	ASSERT_EQ(res11[1].GetColumn(), 19);

	auto res12 = Lexer::GetTokens(SGE_STR("R\"(test string\\ttest)\"R"), formatter);
	ASSERT_EQ(res12.GetSize(), 2);
	ASSERT_EQ(res12[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res12[0].GetContent(), SGE_STR("test string\\ttest"));
	ASSERT_EQ(res12[0].GetLine(), 1);
	ASSERT_EQ(res12[0].GetColumn(), 1);
	ASSERT_EQ(res12[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res12[1].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res12[1].GetLine(), 1);
	ASSERT_EQ(res12[1].GetColumn(), 23);

	auto res13 = Lexer::GetTokens(SGE_STR("R\"test_@#'(1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\")test_@#'\""), formatter);
	ASSERT_EQ(res13.GetSize(), 1);
	ASSERT_EQ(res13[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res13[0].GetContent(), SGE_STR("1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\""));
	ASSERT_EQ(res13[0].GetLine(), 1);
	ASSERT_EQ(res13[0].GetColumn(), 1);
}

TEST(MatchCharsCondition, Test)
{
	using TestMatchCharsCondition = Lexer::Experimental::MatchCharsCondition<SGE_STR("Test")>;
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestMatchCharsCondition, Lexer::Experimental::BaseContext>));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('T'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('e'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('s'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('t'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('a'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('A'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(0, context));

	using TestMatchCharsCondition2 = Lexer::Experimental::MatchCharsCondition<SGE_STR("123\0")>;
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestMatchCharsCondition2, Lexer::Experimental::BaseContext>));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('1'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('2'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('3'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('\0'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(0, context));
	ASSERT_FALSE(TestMatchCharsCondition2::Get(SGE_STR('0'), context));
}

TEST(MatchCharRangeCondition, Test)
{
	using TestMatchCharRangeCondition = Lexer::Experimental::MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>;
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestMatchCharRangeCondition, Lexer::Experimental::BaseContext>));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('a'), context));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('m'), context));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('z'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(SGE_STR('A'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(0, context));
}

TEST(NegateCondition, Test)
{
	using TestNegateCondition = Lexer::Experimental::NegateCondition<Lexer::Experimental::BaseContext, Lexer::Experimental::MatchCharsCondition<SGE_STR("Test")>>;
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestNegateCondition, Lexer::Experimental::BaseContext>));
	ASSERT_FALSE(TestNegateCondition::Get(SGE_STR('T'), context));
	ASSERT_FALSE(TestNegateCondition::Get(SGE_STR('e'), context));
	ASSERT_FALSE(TestNegateCondition::Get(SGE_STR('s'), context));
	ASSERT_FALSE(TestNegateCondition::Get(SGE_STR('t'), context));
	ASSERT_TRUE(TestNegateCondition::Get(SGE_STR('a'), context));
	ASSERT_TRUE(TestNegateCondition::Get(SGE_STR('A'), context));
	ASSERT_TRUE(TestNegateCondition::Get(SGE_STR('0'), context));
}

TEST(OrCondition, Test)
{
	using TestOrCondition = Lexer::Experimental::OrCondition<Lexer::Experimental::BaseContext, Lexer::Experimental::MatchCharsCondition<SGE_STR("Test")>, Lexer::Experimental::MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>>;
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestOrCondition, Lexer::Experimental::BaseContext>));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('T'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('e'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('s'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('t'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('0'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('5'), context));
	ASSERT_TRUE(TestOrCondition::Get(SGE_STR('9'), context));
	ASSERT_FALSE(TestOrCondition::Get(SGE_STR('a'), context));
	ASSERT_FALSE(TestOrCondition::Get(SGE_STR('A'), context));
}

TEST(AndCondition, Test)
{
	using TestAndCondition = Lexer::Experimental::AndCondition<Lexer::Experimental::BaseContext, Lexer::Experimental::MatchCharsCondition<SGE_STR("Test")>, Lexer::Experimental::MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>>;
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<TestAndCondition, Lexer::Experimental::BaseContext>));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('T'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('e'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('s'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('t'), context));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('A'), context));
}

TEST(DefaultCondition, Test)
{
	Lexer::Experimental::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::Experimental::IsCondition<Lexer::Experimental::DefaultCondition, Lexer::Experimental::BaseContext>));
	for (Char c = 0; c < UINT16_MAX; ++c)
		ASSERT_TRUE(Lexer::Experimental::DefaultCondition::Get(c, context));
}

TEST(Transition, Test)
{
	using namespace Lexer::Experimental;
	static_assert(IsTransition<Transition<BaseContext, DefaultCondition, EmptyAction, SGE_STR("TestState")>, BaseContext>);
}

TEST(State, Test)
{
	using namespace Lexer::Experimental;
	State<BaseContext, SGE_STR("TestState"), Transition<BaseContext, DefaultCondition, EmptyAction, SGE_STR("TestState")>> state;
	static_assert(IsState<decltype(state), BaseContext>);
}

TEST(ExperimentalGetTokens, Test)
{
	using namespace Lexer::Experimental;
	{
		using TestState = State<BaseContext, SGE_STR("TestState"),
								Transition<BaseContext, MatchCharsCondition<SGE_STR("_")>, ChainAction<BaseContext, ThrowAction<12345>, SkipAction>, SGE_STR("TestState")>,
								Transition<BaseContext, DefaultCondition, ChainAction<BaseContext, SetTokenLineAndColumnAction, AdvanceAction, SubmitAction<123>>, SGE_STR("TestState")>>;
		String source = SGE_STR("test_string");
		auto result = Lexer::Experimental::GetTokens<BaseContext, SGE_STR("TestState"), SGE_STR("TestState"), TestState>(source);
		ASSERT_EQ(result.m_First.GetSize(), source.GetSize() - 1);
		for (SizeType i = 0, j = 0; i < source.GetSize(); ++i)
		{
			if (i == 4)
				continue;
			ASSERT_EQ(result.m_First[j].GetType(), 123);
			ASSERT_EQ(result.m_First[j].GetContent(), String(1, source[i]));
			ASSERT_EQ(result.m_First[j].GetLine(), 1);
			ASSERT_EQ(result.m_First[j].GetColumn(), i + 1);
			++j;
		}
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), 12345);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
		ASSERT_EQ(result.m_Second[0].GetAdditionalInformation().GetSize(), 0);
	}
	{
		using IdleState = State<BaseContext, SGE_STR("IdleState"),
								Transition<BaseContext, MatchCharsCondition<SGE_STR("0")>, SetTokenLineAndColumnAction, SGE_STR("0State")>,
								Transition<BaseContext, MatchCharsCondition<SGE_STR("1")>, SetTokenLineAndColumnAction, SGE_STR("1State")>>;
		using ZeroState = State<BaseContext, SGE_STR("0State"),
								Transition<BaseContext, MatchCharsCondition<SGE_STR("0")>, AdvanceAction, SGE_STR("0State")>,
								Transition<BaseContext, DefaultCondition, SubmitAction<0>, SGE_STR("IdleState")>>;
		using OneState = State<BaseContext, SGE_STR("1State"),
							   Transition<BaseContext, MatchCharsCondition<SGE_STR("1")>, AdvanceAction, SGE_STR("1State")>,
							   Transition<BaseContext, DefaultCondition, SubmitAction<1>, SGE_STR("IdleState")>>;
		String source = SGE_STR("001101");
		auto result = Lexer::Experimental::GetTokens<BaseContext, SGE_STR("IdleState"), SGE_STR("IdleState"), IdleState, ZeroState, OneState>(source);
		ASSERT_EQ(result.m_First.GetSize(), 4);
		ASSERT_EQ(result.m_First[0].GetType(), 0);
		ASSERT_EQ(result.m_First[0].GetContent(), SGE_STR("00"));
		ASSERT_EQ(result.m_First[0].GetLine(), 1);
		ASSERT_EQ(result.m_First[0].GetColumn(), 1);
		ASSERT_EQ(result.m_First[1].GetType(), 1);
		ASSERT_EQ(result.m_First[1].GetContent(), SGE_STR("11"));
		ASSERT_EQ(result.m_First[1].GetLine(), 1);
		ASSERT_EQ(result.m_First[1].GetColumn(), 3);
		ASSERT_EQ(result.m_First[2].GetType(), 0);
		ASSERT_EQ(result.m_First[2].GetContent(), SGE_STR("0"));
		ASSERT_EQ(result.m_First[2].GetLine(), 1);
		ASSERT_EQ(result.m_First[2].GetColumn(), 5);
		ASSERT_EQ(result.m_First[3].GetType(), 1);
		ASSERT_EQ(result.m_First[3].GetContent(), SGE_STR("1"));
		ASSERT_EQ(result.m_First[3].GetLine(), 1);
		ASSERT_EQ(result.m_First[3].GetColumn(), 6);
		ASSERT_EQ(result.m_Second.GetSize(), 0);
	}
}

TEST(CppLikeStyleLexerGetTokens, Test)
{
	using namespace Lexer::Experimental::CppLikeStyleLexer;
	auto res1 = GetTokens(SGE_STR("this is\ta_test_for R\r\n\r\n123"));
	ASSERT_EQ(res1.m_Second.GetSize(), 0);
	ASSERT_EQ(res1.m_First.GetSize(), 10);
	ASSERT_EQ(res1.m_First[0].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1.m_First[0].GetContent(), SGE_STR("this"));
	ASSERT_EQ(res1.m_First[0].GetLine(), 1);
	ASSERT_EQ(res1.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res1.m_First[1].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1.m_First[1].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1.m_First[1].GetLine(), 1);
	ASSERT_EQ(res1.m_First[1].GetColumn(), 5);
	ASSERT_EQ(res1.m_First[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1.m_First[2].GetContent(), SGE_STR("is"));
	ASSERT_EQ(res1.m_First[2].GetLine(), 1);
	ASSERT_EQ(res1.m_First[2].GetColumn(), 6);
	ASSERT_EQ(res1.m_First[3].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1.m_First[3].GetContent(), SGE_STR("\t"));
	ASSERT_EQ(res1.m_First[3].GetLine(), 1);
	ASSERT_EQ(res1.m_First[3].GetColumn(), 8);
	ASSERT_EQ(res1.m_First[4].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1.m_First[4].GetContent(), SGE_STR("a_test_for"));
	ASSERT_EQ(res1.m_First[4].GetLine(), 1);
	ASSERT_EQ(res1.m_First[4].GetColumn(), 9);
	ASSERT_EQ(res1.m_First[5].GetType(), Lexer::TokenTypes::WordSeparator);
	ASSERT_EQ(res1.m_First[5].GetContent(), SGE_STR(" "));
	ASSERT_EQ(res1.m_First[5].GetLine(), 1);
	ASSERT_EQ(res1.m_First[5].GetColumn(), 19);
	ASSERT_EQ(res1.m_First[6].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res1.m_First[6].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res1.m_First[6].GetLine(), 1);
	ASSERT_EQ(res1.m_First[6].GetColumn(), 20);
	ASSERT_EQ(res1.m_First[7].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1.m_First[7].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1.m_First[7].GetLine(), 1);
	ASSERT_EQ(res1.m_First[7].GetColumn(), 21);
	ASSERT_EQ(res1.m_First[8].GetType(), Lexer::TokenTypes::LineSeparator);
	ASSERT_EQ(res1.m_First[8].GetContent(), SGE_STR("\r\n"));
	ASSERT_EQ(res1.m_First[8].GetLine(), 2);
	ASSERT_EQ(res1.m_First[8].GetColumn(), 1);
	ASSERT_EQ(res1.m_First[9].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res1.m_First[9].GetContent(), SGE_STR("123"));
	ASSERT_EQ(res1.m_First[9].GetLine(), 3);
	ASSERT_EQ(res1.m_First[9].GetColumn(), 1);

	auto res2 = GetTokens(SGE_STR("\r\n\r\n\r\n"));
	ASSERT_EQ(res2.m_Second.GetSize(), 0);
	ASSERT_EQ(res2.m_First.GetSize(), 3);
	for (auto i = 0; i < 3; ++i)
	{
		ASSERT_EQ(res2.m_First[i].GetType(), Lexer::TokenTypes::LineSeparator);
		ASSERT_EQ(res2.m_First[i].GetContent(), SGE_STR("\r\n"));
		ASSERT_EQ(res2.m_First[i].GetLine(), i + 1);
		ASSERT_EQ(res2.m_First[i].GetColumn(), 1);
	}

	auto res3 = GetTokens(SGE_STR("123.12"));
	ASSERT_EQ(res3.m_Second.GetSize(), 0);
	ASSERT_EQ(res3.m_First.GetSize(), 1);
	ASSERT_EQ(res3.m_First[0].GetType(), Lexer::TokenTypes::DoubleLiteral);
	ASSERT_EQ(res3.m_First[0].GetContent(), SGE_STR("123.12"));
	ASSERT_EQ(res3.m_First[0].GetLine(), 1);
	ASSERT_EQ(res3.m_First[0].GetColumn(), 1);

	auto res4 = GetTokens(SGE_STR("123.45f"));
	ASSERT_EQ(res4.m_Second.GetSize(), 0);
	ASSERT_EQ(res4.m_First.GetSize(), 1);
	ASSERT_EQ(res4.m_First[0].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res4.m_First[0].GetContent(), SGE_STR("123.45"));
	ASSERT_EQ(res4.m_First[0].GetLine(), 1);
	ASSERT_EQ(res4.m_First[0].GetColumn(), 1);

	auto res5 = GetTokens(SGE_STR("0xA02f+0b1011"));
	ASSERT_EQ(res5.m_Second.GetSize(), 0);
	ASSERT_EQ(res5.m_First.GetSize(), 3);
	ASSERT_EQ(res5.m_First[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5.m_First[0].GetContent(), SGE_STR("0xA02f"));
	ASSERT_EQ(res5.m_First[0].GetLine(), 1);
	ASSERT_EQ(res5.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res5.m_First[1].GetType(), Lexer::TokenTypes::Add);
	ASSERT_EQ(res5.m_First[1].GetContent(), SGE_STR("+"));
	ASSERT_EQ(res5.m_First[1].GetLine(), 1);
	ASSERT_EQ(res5.m_First[1].GetColumn(), 7);
	ASSERT_EQ(res5.m_First[2].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res5.m_First[2].GetContent(), SGE_STR("0b1011"));
	ASSERT_EQ(res5.m_First[2].GetLine(), 1);
	ASSERT_EQ(res5.m_First[2].GetColumn(), 8);

	auto res6 = GetTokens(SGE_STR("-12--12.345f"));
	ASSERT_EQ(res6.m_Second.GetSize(), 0);
	ASSERT_EQ(res6.m_First.GetSize(), 5);
	ASSERT_EQ(res6.m_First[0].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6.m_First[0].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6.m_First[0].GetLine(), 1);
	ASSERT_EQ(res6.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res6.m_First[1].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res6.m_First[1].GetContent(), SGE_STR("12"));
	ASSERT_EQ(res6.m_First[1].GetLine(), 1);
	ASSERT_EQ(res6.m_First[1].GetColumn(), 2);
	ASSERT_EQ(res6.m_First[2].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6.m_First[2].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6.m_First[2].GetLine(), 1);
	ASSERT_EQ(res6.m_First[2].GetColumn(), 4);
	ASSERT_EQ(res6.m_First[3].GetType(), Lexer::TokenTypes::Subtract);
	ASSERT_EQ(res6.m_First[3].GetContent(), SGE_STR("-"));
	ASSERT_EQ(res6.m_First[3].GetLine(), 1);
	ASSERT_EQ(res6.m_First[3].GetColumn(), 5);
	ASSERT_EQ(res6.m_First[4].GetType(), Lexer::TokenTypes::FloatLiteral);
	ASSERT_EQ(res6.m_First[4].GetContent(), SGE_STR("12.345"));
	ASSERT_EQ(res6.m_First[4].GetLine(), 1);
	ASSERT_EQ(res6.m_First[4].GetColumn(), 6);

	auto res7 = GetTokens(SGE_STR("//test comment line"));
	ASSERT_EQ(res7.m_Second.GetSize(), 0);
	ASSERT_EQ(res7.m_First.GetSize(), 1);
	ASSERT_EQ(res7.m_First[0].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res7.m_First[0].GetContent(), SGE_STR("test comment line"));
	ASSERT_EQ(res7.m_First[0].GetLine(), 1);
	ASSERT_EQ(res7.m_First[0].GetColumn(), 1);

	auto res8 = GetTokens(SGE_STR("/*test comment block*/a"));
	ASSERT_EQ(res8.m_Second.GetSize(), 0);
	ASSERT_EQ(res8.m_First.GetSize(), 2);
	ASSERT_EQ(res8.m_First[0].GetType(), Lexer::TokenTypes::CommentBlock);
	ASSERT_EQ(res8.m_First[0].GetContent(), SGE_STR("test comment block"));
	ASSERT_EQ(res8.m_First[0].GetLine(), 1);
	ASSERT_EQ(res8.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res8.m_First[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res8.m_First[1].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res8.m_First[1].GetLine(), 1);
	ASSERT_EQ(res8.m_First[1].GetColumn(), 23);

	auto res9 = GetTokens(SGE_STR("1/b//"));
	ASSERT_EQ(res9.m_Second.GetSize(), 0);
	ASSERT_EQ(res9.m_First.GetSize(), 4);
	ASSERT_EQ(res9.m_First[0].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res9.m_First[0].GetContent(), SGE_STR("1"));
	ASSERT_EQ(res9.m_First[0].GetLine(), 1);
	ASSERT_EQ(res9.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res9.m_First[1].GetType(), Lexer::TokenTypes::Slash);
	ASSERT_EQ(res9.m_First[1].GetContent(), SGE_STR("/"));
	ASSERT_EQ(res9.m_First[1].GetLine(), 1);
	ASSERT_EQ(res9.m_First[1].GetColumn(), 2);
	ASSERT_EQ(res9.m_First[2].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res9.m_First[2].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res9.m_First[2].GetLine(), 1);
	ASSERT_EQ(res9.m_First[2].GetColumn(), 3);
	ASSERT_EQ(res9.m_First[3].GetType(), Lexer::TokenTypes::CommentLine);
	ASSERT_EQ(res9.m_First[3].GetContent(), SGE_STR(""));
	ASSERT_EQ(res9.m_First[3].GetLine(), 1);
	ASSERT_EQ(res9.m_First[3].GetColumn(), 4);

	auto res10 = GetTokens(SGE_STR("'a''\\n'"));
	ASSERT_EQ(res10.m_Second.GetSize(), 0);
	ASSERT_EQ(res10.m_First.GetSize(), 2);
	ASSERT_EQ(res10.m_First[0].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10.m_First[0].GetContent(), SGE_STR("a"));
	ASSERT_EQ(res10.m_First[0].GetLine(), 1);
	ASSERT_EQ(res10.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res10.m_First[1].GetType(), Lexer::TokenTypes::CharacterLiteral);
	ASSERT_EQ(res10.m_First[1].GetContent(), SGE_STR("\n"));
	ASSERT_EQ(res10.m_First[1].GetLine(), 1);
	ASSERT_EQ(res10.m_First[1].GetColumn(), 4);

	auto res11 = GetTokens(SGE_STR("\"test string\ttest\"b"));
	ASSERT_EQ(res11.m_Second.GetSize(), 0);
	ASSERT_EQ(res11.m_First.GetSize(), 2);
	ASSERT_EQ(res11.m_First[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res11.m_First[0].GetContent(), SGE_STR("test string\ttest"));
	ASSERT_EQ(res11.m_First[0].GetLine(), 1);
	ASSERT_EQ(res11.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res11.m_First[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res11.m_First[1].GetContent(), SGE_STR("b"));
	ASSERT_EQ(res11.m_First[1].GetLine(), 1);
	ASSERT_EQ(res11.m_First[1].GetColumn(), 19);

	auto res12 = GetTokens(SGE_STR("R\"(test string\\ttest)\"R"));
	ASSERT_EQ(res12.m_Second.GetSize(), 0);
	ASSERT_EQ(res12.m_First.GetSize(), 2);
	ASSERT_EQ(res12.m_First[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res12.m_First[0].GetContent(), SGE_STR("test string\\ttest"));
	ASSERT_EQ(res12.m_First[0].GetLine(), 1);
	ASSERT_EQ(res12.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res12.m_First[1].GetType(), Lexer::TokenTypes::Identifier);
	ASSERT_EQ(res12.m_First[1].GetContent(), SGE_STR("R"));
	ASSERT_EQ(res12.m_First[1].GetLine(), 1);
	ASSERT_EQ(res12.m_First[1].GetColumn(), 23);

	auto res13 = GetTokens(SGE_STR("R\"test_@#'(1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\")test_@#'\""));
	ASSERT_EQ(res13.m_Second.GetSize(), 0);
	ASSERT_EQ(res13.m_First.GetSize(), 1);
	ASSERT_EQ(res13.m_First[0].GetType(), Lexer::TokenTypes::StringLiteral);
	ASSERT_EQ(res13.m_First[0].GetContent(), SGE_STR("1(2(3)123__abc)ttt)aaa\"bbb)ccc)ddd\""));
	ASSERT_EQ(res13.m_First[0].GetLine(), 1);
	ASSERT_EQ(res13.m_First[0].GetColumn(), 1);
}