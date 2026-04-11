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

TEST(BaseContext, Test)
{
	String source = SGE_STR("Test");
	Lexer::BaseContext context(source);

	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('T'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Advance();
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('e'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Append(SGE_STR('a'));
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('e'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Skip();
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Clear();
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.NextLine();
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.SetTokenLineAndColumn();
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.AddOffsetToTokenLine(1);
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.AddOffsetToTokenColumn(100);
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('s'));
	ASSERT_FALSE(context.IsEnd());
	ASSERT_EQ(context.GetTokens().GetSize(), 0);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Advance();
	context.Append(SGE_STR('e'));

	context.Submit(12);
	ASSERT_EQ(context.GetCurrentChar(), SGE_STR('t'));
	ASSERT_FALSE(context.IsEnd());
	auto tokens = context.GetTokens();
	ASSERT_EQ(tokens.GetSize(), 1);
	ASSERT_EQ(tokens[0].GetType(), 12);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("se"));
	ASSERT_EQ(tokens[0].GetLine(), 3);
	ASSERT_EQ(tokens[0].GetColumn(), 101);
	ASSERT_EQ(context.GetErrors().GetSize(), 0);

	context.Skip();
	ASSERT_EQ(context.GetCurrentChar(), 0);
	ASSERT_TRUE(context.IsEnd());

	context.Throw(789, {SGE_STR("test error")});
	ASSERT_EQ(context.GetCurrentChar(), 0);
	ASSERT_TRUE(context.IsEnd());
	tokens = context.GetTokens();
	ASSERT_EQ(tokens.GetSize(), 1);
	ASSERT_EQ(tokens[0].GetType(), 12);
	ASSERT_EQ(tokens[0].GetContent(), SGE_STR("se"));
	ASSERT_EQ(tokens[0].GetLine(), 3);
	ASSERT_EQ(tokens[0].GetColumn(), 101);
	auto errors = context.GetErrors();
	ASSERT_EQ(errors.GetSize(), 1);
	ASSERT_EQ(errors[0].GetTypeId(), 789);
	ASSERT_EQ(errors[0].GetLine(), 2);
	ASSERT_EQ(errors[0].GetColumn(), 3);
}

TEST(MatchCharsCondition, Test)
{
	using TestMatchCharsCondition = Lexer::MatchCharsCondition<SGE_STR("Test")>;
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<TestMatchCharsCondition, Lexer::BaseContext>));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('T'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('e'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('s'), context));
	ASSERT_TRUE(TestMatchCharsCondition::Get(SGE_STR('t'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('a'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('A'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestMatchCharsCondition::Get(0, context));

	using TestMatchCharsCondition2 = Lexer::MatchCharsCondition<SGE_STR("123\0")>;
	ASSERT_TRUE((Lexer::IsCondition<TestMatchCharsCondition2, Lexer::BaseContext>));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('1'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('2'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('3'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(SGE_STR('\0'), context));
	ASSERT_TRUE(TestMatchCharsCondition2::Get(0, context));
	ASSERT_FALSE(TestMatchCharsCondition2::Get(SGE_STR('0'), context));
}

TEST(MatchCharRangeCondition, Test)
{
	using TestMatchCharRangeCondition = Lexer::MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>;
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<TestMatchCharRangeCondition, Lexer::BaseContext>));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('a'), context));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('m'), context));
	ASSERT_TRUE(TestMatchCharRangeCondition::Get(SGE_STR('z'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(SGE_STR('A'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestMatchCharRangeCondition::Get(0, context));
}

TEST(NegateCondition, Test)
{
	using TestNegateCondition = Lexer::NegateCondition<Lexer::BaseContext, Lexer::MatchCharsCondition<SGE_STR("Test")>>;
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<TestNegateCondition, Lexer::BaseContext>));
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
	using TestOrCondition = Lexer::OrCondition<Lexer::BaseContext, Lexer::MatchCharsCondition<SGE_STR("Test")>, Lexer::MatchCharRangeCondition<SGE_STR('0'), SGE_STR('9')>>;
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<TestOrCondition, Lexer::BaseContext>));
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
	using TestAndCondition = Lexer::AndCondition<Lexer::BaseContext, Lexer::MatchCharsCondition<SGE_STR("Test")>, Lexer::MatchCharRangeCondition<SGE_STR('a'), SGE_STR('z')>>;
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<TestAndCondition, Lexer::BaseContext>));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('T'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('e'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('s'), context));
	ASSERT_TRUE(TestAndCondition::Get(SGE_STR('t'), context));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('0'), context));
	ASSERT_FALSE(TestAndCondition::Get(SGE_STR('A'), context));
}

TEST(DefaultCondition, Test)
{
	Lexer::BaseContext context(SGE_STR("Test"));
	ASSERT_TRUE((Lexer::IsCondition<Lexer::DefaultCondition, Lexer::BaseContext>));
	for (Char c = 0; c < UINT16_MAX; ++c)
		ASSERT_TRUE(Lexer::DefaultCondition::Get(c, context));
}

TEST(Transition, Test)
{
	using namespace Lexer;
	static_assert(IsTransition<Transition<BaseContext, DefaultCondition, EmptyAction, SGE_STR("TestState")>, BaseContext>);
}

TEST(State, Test)
{
	using namespace Lexer;
	State<BaseContext, SGE_STR("TestState"), Transition<BaseContext, DefaultCondition, EmptyAction, SGE_STR("TestState")>> state;
	static_assert(IsState<decltype(state), BaseContext>);
}

TEST(GetTokens, Test)
{
	using namespace Lexer;
	{
		using TestState = State<BaseContext, SGE_STR("TestState"),
								Transition<BaseContext, MatchCharsCondition<SGE_STR("_")>, ChainAction<BaseContext, ThrowAction<12345>, SkipAction>, SGE_STR("TestState")>,
								Transition<BaseContext, DefaultCondition, ChainAction<BaseContext, SetTokenLineAndColumnAction, AdvanceAction, SubmitAction<123>>, SGE_STR("TestState")>>;
		String source = SGE_STR("test_string");
		auto result = Lexer::GetTokens<BaseContext, SGE_STR("TestState"), SGE_STR("TestState"), TestState>(source);
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
		auto result = Lexer::GetTokens<BaseContext, SGE_STR("IdleState"), SGE_STR("IdleState"), IdleState, ZeroState, OneState>(source);
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

TEST(CppLikeStyleLexer, GetTokensTest)
{
	using namespace Lexer::CppLikeStyleLexer;
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

	auto res14 = GetTokens(SGE_STR("/*123\n456\n789*/1"));
	ASSERT_EQ(res14.m_Second.GetSize(), 0);
	ASSERT_EQ(res14.m_First.GetSize(), 2);
	ASSERT_EQ(res14.m_First[0].GetType(), Lexer::TokenTypes::CommentBlock);
	ASSERT_EQ(res14.m_First[0].GetContent(), SGE_STR("123\n456\n789"));
	ASSERT_EQ(res14.m_First[0].GetLine(), 1);
	ASSERT_EQ(res14.m_First[0].GetColumn(), 1);
	ASSERT_EQ(res14.m_First[1].GetType(), Lexer::TokenTypes::IntegerLiteral);
	ASSERT_EQ(res14.m_First[1].GetContent(), SGE_STR("1"));
	ASSERT_EQ(res14.m_First[1].GetLine(), 3);
	ASSERT_EQ(res14.m_First[1].GetColumn(), 6);
}

TEST(CppLikeStyleLexer, ErrorTest)
{
	using namespace Lexer::CppLikeStyleLexer;

	// No-error cases
	ASSERT_EQ(GetTokens(SGE_STR("")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("identifier_ID12")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\n234\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r234\r")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r\n234\r\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r\n234")).m_Second.GetSize(), 0);
	// Mixed line endings are no longer an error in the new lexer
	ASSERT_EQ(GetTokens(SGE_STR("123\r234\r\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\n234\r\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r\n234\r")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r\n234\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123\r\n234\n\r")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("this is test for 1\r\n 3m m4")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("1 - 0\n1-0\n-100")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("01234")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("0b1001")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("0x1234")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("0x1A2B3c4f")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123.345")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("123.345f")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("-123.345")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("-123.345f")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR(R"('a')")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR(R"('\r')")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR(R"("test string 123.321")")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR(R"("test string\n")")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"()\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"(test\\a)\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"1_/a*(test)1_/a*\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"abc(test)bbc\")abc\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"abc(\"bbc(test)bbc\")abc\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("R\"abc()123)abc\"")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("Rest")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("/**/")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("/*test*/")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("/*test\ntest2*/")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("//")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("//\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("//test")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("//test\n//test\n")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("//test\n//test")).m_Second.GetSize(), 0);
	ASSERT_EQ(GetTokens(SGE_STR("?\n/\\")).m_Second.GetSize(), 0);

	// BinaryIntegerInvalidCharacter
	{
		// "0b1234": '2' at col 4 is invalid in a binary literal
		auto result = GetTokens(SGE_STR("0b1234"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::BinaryIntegerInvalidCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 4);
	}
	// DoubleDotInvalidCharacter
	{
		// "123.": end-of-input at col 5 is invalid after the dot
		auto result = GetTokens(SGE_STR("123."));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::DoubleDotInvalidCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
	}
	{
		// "123.f": 'f' at col 5 is invalid directly after the dot
		auto result = GetTokens(SGE_STR("123.f"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::DoubleDotInvalidCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
	}
	{
		// "123..345": second '.' at col 5 is invalid directly after the first dot
		auto result = GetTokens(SGE_STR("123..345"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::DoubleDotInvalidCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
	}
	// EmptyCharacterLiteral
	{
		// "''": second '\'' at col 2 closes an empty literal
		auto result = GetTokens(SGE_STR(R"('')"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::EmptyCharacterLiteral);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 2);
	}
	// CharacterNotEnd
	{
		// "'": end-of-input at col 2 inside a character literal
		auto result = GetTokens(SGE_STR(R"(')"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CharacterNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 2);
	}
	// MultiplyCharacter
	{
		// "'s": end-of-input at col 3 after one character without closing '\''
		auto result = GetTokens(SGE_STR(R"('s)"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::MultiplyCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
	}
	// InvalidEscapeCharacter
	{
		// "'\a'": 'a' at col 3 is not a valid escape character.
		// The error leaves the iterator at 'a', which is parsed as an identifier; the
		// remaining '\'' at col 4 then starts an unterminated character literal -> CharacterNotEnd.
		auto result = GetTokens(SGE_STR(R"('\a')"));
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::InvalidEscapeCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CharacterNotEnd);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 5);
	}
	{
		// "'\": end-of-input at col 3 after the escape backslash
		auto result = GetTokens(SGE_STR(R"('\)"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::InvalidEscapeCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
	}
	{
		// "\"test string\": end-of-input at col 14 after the trailing backslash
		auto result = GetTokens(SGE_STR("\"test string\\"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::InvalidEscapeCharacter);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 14);
	}
	// StringNotEnd
	{
		// "\"test string\n": end-of-input at col 15 (the \n escape is valid, string is unterminated)
		auto result = GetTokens(SGE_STR(R"("test string\n)"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::StringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 15);
	}
	// RawStringInvalidPrefix
	{
		// "R\"": end-of-input at col 3 inside the raw string prefix
		auto result = GetTokens(SGE_STR("R\""));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringInvalidPrefix);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
	}
	{
		// "R\"\"": the second '\"' at col 3 is an invalid prefix character.
		// The error leaves the iterator at the second '\"', which starts an unterminated string -> StringNotEnd.
		auto result = GetTokens(SGE_STR("R\"\""));
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringInvalidPrefix);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::StringNotEnd);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 4);
	}
	{
		// "R\"\"(test)\"\"": same prefix error at col 3; the remaining input is parsed as
		// a string "(test)" (which closes) and then a second unterminated string -> StringNotEnd at col 12.
		auto result = GetTokens(SGE_STR("R\"\"(test)\"\""));
		ASSERT_EQ(result.m_Second.GetSize(), 2);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringInvalidPrefix);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
		ASSERT_EQ(result.m_Second[1].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::StringNotEnd);
		ASSERT_EQ(result.m_Second[1].GetLine(), 1);
		ASSERT_EQ(result.m_Second[1].GetColumn(), 12);
	}
	// RawStringNotEnd
	{
		// "R\"(": end-of-input at col 4 inside the raw string body
		auto result = GetTokens(SGE_STR("R\"("));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 4);
	}
	{
		// "R\"()": end-of-input at col 5 while matching the closing suffix
		auto result = GetTokens(SGE_STR("R\"()"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 5);
	}
	{
		// "R\"123(test)456": end-of-input at col 15 while matching the suffix "456"
		auto result = GetTokens(SGE_STR("R\"123(test)456"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 15);
	}
	{
		// "R\"123(test)456\"": suffix "456" doesn't match prefix "123"; the '\"' is absorbed
		// into the body and end-of-input is hit at col 16
		auto result = GetTokens(SGE_STR("R\"123(test)456\""));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 16);
	}
	{
		// "R\"123(test)\"": empty suffix doesn't match prefix "123"; the '\"' is absorbed
		// into the body and end-of-input is hit at col 13
		auto result = GetTokens(SGE_STR("R\"123(test)\""));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 13);
	}
	{
		// "R\"(test)456\"": suffix "456" doesn't match empty prefix; the '\"' is absorbed
		// into the body and end-of-input is hit at col 13
		auto result = GetTokens(SGE_STR("R\"(test)456\""));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::RawStringNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 13);
	}
	// CommentBlockNotEnd
	{
		// "/*": end-of-input at col 3 inside the comment block
		auto result = GetTokens(SGE_STR("/*"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CommentBlockNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 3);
	}
	{
		// "/*\n": end-of-input at col 4 (newlines inside comment blocks do reset the column)
		auto result = GetTokens(SGE_STR("/*\n"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CommentBlockNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 2);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
	}
	{
		// "/**": end-of-input at col 4 while matching the closing '*/'
		auto result = GetTokens(SGE_STR("/**"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CommentBlockNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 1);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 4);
	}
	{
		// "/**\n": end-of-input at col 5 (the '*' is re-added to the body, then '\n' is consumed)
		auto result = GetTokens(SGE_STR("/**\n"));
		ASSERT_EQ(result.m_Second.GetSize(), 1);
		ASSERT_EQ(result.m_Second[0].GetTypeId(), Lexer::CppLikeStyleLexer::Detail::ErrorTypeId::CommentBlockNotEnd);
		ASSERT_EQ(result.m_Second[0].GetLine(), 2);
		ASSERT_EQ(result.m_Second[0].GetColumn(), 1);
	}
}