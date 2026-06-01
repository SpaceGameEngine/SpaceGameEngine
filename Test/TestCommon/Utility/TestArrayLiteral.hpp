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
#include "Utility/ArrayLiteral.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(ArrayLiteral, Test)
{
	constexpr auto wstring_literal = ArrayLiteral(SGE_WSTR("TestArrayLiteral"));
	ASSERT_TRUE(memcmp(wstring_literal.m_Value, SGE_WSTR("TestArrayLiteral"), 17 * sizeof(Char16)) == 0);
	ASSERT_EQ(wstring_literal.Size, 17);
	constexpr auto u8string_literal = ArrayLiteral(SGE_U8STR("TestArrayLiteral"));
	ASSERT_TRUE(memcmp(u8string_literal.m_Value, SGE_U8STR("TestArrayLiteral"), 17 * sizeof(Char8)) == 0);
	ASSERT_EQ(u8string_literal.Size, 17);

	constexpr auto int_array_literal = ArrayLiteral<int, 3>{{1, 2, 3}};
	int int_array_expected[] = {1, 2, 3};
	ASSERT_TRUE(memcmp(int_array_literal.m_Value, int_array_expected, sizeof(int_array_expected)) == 0);
	ASSERT_EQ(decltype(int_array_literal)::Size, 3);
}

TEST(ArrayLiteral, ConcatTest)
{
	// concat two int ArrayLiterals
	constexpr ArrayLiteral<int, 3> iarr1{{1, 2, 3}};
	constexpr ArrayLiteral<int, 2> iarr2{{4, 5}};
	constexpr auto iconcat = ConcatArrayLiteral(iarr1, iarr2);
	ASSERT_EQ(decltype(iconcat)::Size, 5);
	int iexpected[] = {1, 2, 3, 4, 5};
	ASSERT_TRUE(memcmp(iconcat.m_Value, iexpected, sizeof(iexpected)) == 0);

	// concat three int ArrayLiterals (variadic overload)
	constexpr ArrayLiteral<int, 2> iarr3{{6, 7}};
	constexpr auto iconcat3 = ConcatArrayLiteral(iarr1, iarr2, iarr3);
	ASSERT_EQ(decltype(iconcat3)::Size, 7);
	int iexpected3[] = {1, 2, 3, 4, 5, 6, 7};
	ASSERT_TRUE(memcmp(iconcat3.m_Value, iexpected3, sizeof(iexpected3)) == 0);
}

TEST(ArrayLiteral, ConcatCStringTest)
{
	// concat two Char16 string literals: null terminator of first is dropped
	constexpr auto wconcat = ConcatCStringLiteral(ArrayLiteral(SGE_WSTR("Hello")), ArrayLiteral(SGE_WSTR("World")));
	// Size1=6, Size2=6 -> 6+6-1=11
	ASSERT_EQ(decltype(wconcat)::Size, 11);
	ASSERT_TRUE(memcmp(wconcat.m_Value, SGE_WSTR("HelloWorld"), 11 * sizeof(Char16)) == 0);

	// concat two Char8 string literals
	constexpr auto u8concat = ConcatCStringLiteral(ArrayLiteral(SGE_U8STR("Foo")), ArrayLiteral(SGE_U8STR("Bar")));
	// Size1=4, Size2=4 -> 4+4-1=7
	ASSERT_EQ(decltype(u8concat)::Size, 7);
	ASSERT_TRUE(memcmp(u8concat.m_Value, SGE_U8STR("FooBar"), 7 * sizeof(Char8)) == 0);

	// concat three Char16 string literals (variadic overload): only one null terminator at the end
	constexpr auto wconcat3 = ConcatCStringLiteral(ArrayLiteral(SGE_WSTR("Foo")), ArrayLiteral(SGE_WSTR("Bar")), ArrayLiteral(SGE_WSTR("Baz")));
	// Size1=4, Size2=4, Size3=4 -> 4+4+4-1=11
	ASSERT_EQ(decltype(wconcat3)::Size, 10);
	ASSERT_TRUE(memcmp(wconcat3.m_Value, SGE_WSTR("FooBarBaz"), 10 * sizeof(Char16)) == 0);

	// concat three Char8 string literals (variadic overload)
	constexpr auto u8concat3 = ConcatCStringLiteral(ArrayLiteral(SGE_U8STR("Hello")), ArrayLiteral(SGE_U8STR("World")), ArrayLiteral(SGE_U8STR("!")));
	// Size1=6, Size2=6, Size3=2 -> 6+6+2-1=13
	ASSERT_EQ(decltype(u8concat3)::Size, 12);
	ASSERT_TRUE(memcmp(u8concat3.m_Value, SGE_U8STR("HelloWorld!"), 12 * sizeof(Char8)) == 0);
}

TEST(ArrayLiteral, JoinCStringLiteralsTest)
{
	// join two Char16 strings with separator
	constexpr auto wjoin2 = JoinCStringLiterals(ArrayLiteral(SGE_WSTR(", ")), ArrayLiteral(SGE_WSTR("foo")), ArrayLiteral(SGE_WSTR("bar")));
	// "foo"=4, ", "=3, "bar"=4 -> 4+4+3-2=9
	ASSERT_EQ(decltype(wjoin2)::Size, 9);
	ASSERT_TRUE(memcmp(wjoin2.m_Value, SGE_WSTR("foo, bar"), 9 * sizeof(Char16)) == 0);

	// join two Char8 strings with separator
	constexpr auto u8join2 = JoinCStringLiterals(ArrayLiteral(SGE_U8STR("-")), ArrayLiteral(SGE_U8STR("hello")), ArrayLiteral(SGE_U8STR("world")));
	// "hello"=6, "-"=2, "world"=6 -> 6+6+2-2=12
	ASSERT_EQ(decltype(u8join2)::Size, 12);
	ASSERT_TRUE(memcmp(u8join2.m_Value, SGE_U8STR("hello-world"), 12 * sizeof(Char8)) == 0);

	// join three Char16 strings with separator (variadic overload)
	constexpr auto wjoin3 = JoinCStringLiterals(ArrayLiteral(SGE_WSTR("/")), ArrayLiteral(SGE_WSTR("a")), ArrayLiteral(SGE_WSTR("b")), ArrayLiteral(SGE_WSTR("c")));
	// "a"=2, "b"=2, "c"=2, "/"=2 -> result="a/b/c", size=6
	ASSERT_EQ(decltype(wjoin3)::Size, 6);
	ASSERT_TRUE(memcmp(wjoin3.m_Value, SGE_WSTR("a/b/c"), 6 * sizeof(Char16)) == 0);

	// join four Char8 strings with separator (variadic overload)
	constexpr auto u8join4 = JoinCStringLiterals(ArrayLiteral(SGE_U8STR(", ")), ArrayLiteral(SGE_U8STR("one")), ArrayLiteral(SGE_U8STR("two")), ArrayLiteral(SGE_U8STR("three")), ArrayLiteral(SGE_U8STR("four")));
	// "one"=4, "two"=4, "three"=6, "four"=5, sep=", "(3) -> "one, two, three, four"=22
	ASSERT_EQ(decltype(u8join4)::Size, 22);
	ASSERT_TRUE(memcmp(u8join4.m_Value, SGE_U8STR("one, two, three, four"), 22 * sizeof(Char8)) == 0);

	// composable with ToCStringLiteral
	constexpr auto composed = JoinCStringLiterals(ArrayLiteral(SGE_WSTR("=")), ArrayLiteral(SGE_WSTR("val")), ToCStringLiteral<42, Char16>());
	// "val"=4, "="=2, "42"=3 -> "val=42"=7
	ASSERT_EQ(decltype(composed)::Size, 7);
	ASSERT_TRUE(memcmp(composed.m_Value, SGE_WSTR("val=42"), 7 * sizeof(Char16)) == 0);
}

TEST(ArrayLiteral, ToCStringLiteralTest)
{
	// zero
	constexpr auto zero = ToCStringLiteral<0, Char16>();
	ASSERT_EQ(decltype(zero)::Size, 2);
	ASSERT_TRUE(memcmp(zero.m_Value, SGE_WSTR("0"), 2 * sizeof(Char16)) == 0);

	// positive decimal (Char16)
	constexpr auto pos = ToCStringLiteral<42, Char16>();
	ASSERT_EQ(decltype(pos)::Size, 3);
	ASSERT_TRUE(memcmp(pos.m_Value, SGE_WSTR("42"), 3 * sizeof(Char16)) == 0);

	// negative decimal (Char16)
	constexpr auto neg = ToCStringLiteral<-42, Char16>();
	ASSERT_EQ(decltype(neg)::Size, 4);
	ASSERT_TRUE(memcmp(neg.m_Value, SGE_WSTR("-42"), 4 * sizeof(Char16)) == 0);

	// positive decimal (Char8)
	constexpr auto pos_u8 = ToCStringLiteral<1234, Char8>();
	ASSERT_EQ(decltype(pos_u8)::Size, 5);
	ASSERT_TRUE(memcmp(pos_u8.m_Value, SGE_U8STR("1234"), 5 * sizeof(Char8)) == 0);

	// negative decimal (Char8)
	constexpr auto neg_u8 = ToCStringLiteral<-1, Char8>();
	ASSERT_EQ(decltype(neg_u8)::Size, 3);
	ASSERT_TRUE(memcmp(neg_u8.m_Value, SGE_U8STR("-1"), 3 * sizeof(Char8)) == 0);

	// hexadecimal (base 16)
	constexpr auto hex = ToCStringLiteral<255, Char16, 16>();
	ASSERT_EQ(decltype(hex)::Size, 3);
	ASSERT_TRUE(memcmp(hex.m_Value, SGE_WSTR("ff"), 3 * sizeof(Char16)) == 0);

	// binary (base 2)
	constexpr auto bin = ToCStringLiteral<0b1010, Char16, 2>();
	ASSERT_EQ(decltype(bin)::Size, 5);
	ASSERT_TRUE(memcmp(bin.m_Value, SGE_WSTR("1010"), 5 * sizeof(Char16)) == 0);

	// octal (base 8)
	constexpr auto oct = ToCStringLiteral<0777, Char16, 8>();
	ASSERT_EQ(decltype(oct)::Size, 4);
	ASSERT_TRUE(memcmp(oct.m_Value, SGE_WSTR("777"), 4 * sizeof(Char16)) == 0);
}