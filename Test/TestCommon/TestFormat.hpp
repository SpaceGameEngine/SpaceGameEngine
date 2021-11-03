/*
Copyright 2021 creatorlxd

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
#include "gtest/gtest.h"
#include "Utility/Format.hpp"

TEST(Format, IsFormatStringTest)
{
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{{}")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{}}")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{#}")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{0")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{:")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{}{0}")))));
	ASSERT_FALSE((IsFormatString(UCS2String(SGE_WSTR("test{0}{}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{{")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test}}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{{}}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{}{}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{0}{1}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{:test}{}")))));
	ASSERT_TRUE((IsFormatString(UCS2String(SGE_WSTR("test{0}{1:test}")))));

	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{{}")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{}}")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{#}")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{0")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{:")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{}{0}")))));
	ASSERT_FALSE((IsFormatString(UTF8String(SGE_U8STR("test{0}{}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{{")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test}}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{{}}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{}{}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{0}{1}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{:test}{}")))));
	ASSERT_TRUE((IsFormatString(UTF8String(SGE_U8STR("test{0}{1:test}")))));
}