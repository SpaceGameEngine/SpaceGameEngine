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
#include "Utility/LineBreak.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(GetLineBreak, Test)
{
	ASSERT_EQ((GetLineBreak<Char16, UCS2Trait>(SGE_WSTR(' '), SGE_WSTR(' '))), LineBreak::Unknown);
	ASSERT_EQ((GetLineBreak<Char16, UCS2Trait>(SGE_WSTR('\n'), SGE_WSTR(' '))), LineBreak::LF);
	ASSERT_EQ((GetLineBreak<Char16, UCS2Trait>(SGE_WSTR('\r'), SGE_WSTR(' '))), LineBreak::CR);
	ASSERT_EQ((GetLineBreak<Char16, UCS2Trait>(SGE_WSTR('\r'), SGE_WSTR('\n'))), LineBreak::CRLF);

	ASSERT_EQ((GetLineBreak<Char8, UTF8Trait>(SGE_U8STR(" "), SGE_U8STR(" "))), LineBreak::Unknown);
	ASSERT_EQ((GetLineBreak<Char8, UTF8Trait>(SGE_U8STR("\n"), SGE_U8STR(" "))), LineBreak::LF);
	ASSERT_EQ((GetLineBreak<Char8, UTF8Trait>(SGE_U8STR("\r"), SGE_U8STR(" "))), LineBreak::CR);
	ASSERT_EQ((GetLineBreak<Char8, UTF8Trait>(SGE_U8STR("\r"), SGE_U8STR("\n"))), LineBreak::CRLF);
}

TEST(GetLineBreakString, Test)
{
	ASSERT_EQ((GetLineBreakString<Char16, UCS2Trait>(LineBreak::CR)), SGE_WSTR("\r"));
	ASSERT_EQ((GetLineBreakString<Char16, UCS2Trait>(LineBreak::LF)), SGE_WSTR("\n"));
	ASSERT_EQ((GetLineBreakString<Char16, UCS2Trait>(LineBreak::CRLF)), SGE_WSTR("\r\n"));

	ASSERT_EQ((GetLineBreakString<Char8, UTF8Trait>(LineBreak::CR)), SGE_U8STR("\r"));
	ASSERT_EQ((GetLineBreakString<Char8, UTF8Trait>(LineBreak::LF)), SGE_U8STR("\n"));
	ASSERT_EQ((GetLineBreakString<Char8, UTF8Trait>(LineBreak::CRLF)), SGE_U8STR("\r\n"));
}

TEST(GetSystemLineBreak, Test)
{
#ifdef SGE_WINDOWS
	ASSERT_EQ(GetSystemLineBreak(), LineBreak::CRLF);
#elif defined(SGE_UNIX)
	ASSERT_EQ(GetSystemLineBreak(), LineBreak::LF);
#endif
}