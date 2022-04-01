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

TEST(Format, FormatTest)
{
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{}{}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{0}{0}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{1}{1}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{0}{1}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{1}{0}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test format{1:test}{0:format}!"))), SGE_WSTR("test format!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{}, {}!")), SGE_WSTR("Hello"), SGE_WSTR("world")), SGE_WSTR("Hello, world!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0}, {1}!")), SGE_WSTR("Hello"), SGE_WSTR("world")), SGE_WSTR("Hello, world!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{1}, {0}!")), SGE_WSTR("world"), SGE_WSTR("Hello")), SGE_WSTR("Hello, world!"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{},{},{},{}")), SGE_WSTR("0"), SGE_WSTR("1")), SGE_WSTR("0,1,,"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0},{1},{2},{3}")), SGE_WSTR("0"), SGE_WSTR("1")), SGE_WSTR("0,1,,"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{2},{1},{0},{3}")), SGE_WSTR("0"), SGE_WSTR("1")), SGE_WSTR(",1,0,"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{2},{1},{0}")), SGE_WSTR("0"), SGE_WSTR("1"), SGE_WSTR("2"), SGE_WSTR("3"), SGE_WSTR("4"), SGE_WSTR("5")), SGE_WSTR("2,1,0"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0}+{0}={0}")), SGE_WSTR("0"), SGE_WSTR("1"), SGE_WSTR("2"), SGE_WSTR("3"), SGE_WSTR("4"), SGE_WSTR("5")), SGE_WSTR("0+0=0"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{{}} == {}{}")), SGE_WSTR("{"), SGE_WSTR("}")), SGE_WSTR("{} == {}"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0:D}=={0:B}=={0:H}")), 12), SGE_WSTR("12==1100==c"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{:D},{:B},{:H}")), -10, 11, 12), SGE_WSTR("-10,1011,c"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0},{0:2},{0:4}")), 1.234567f), SGE_WSTR("1.2346,1.23,1.2346"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{:},{:2},{:4}")), -1.0f, 2.234567f, 3.234567f), SGE_WSTR("-1.0000,2.23,3.2346"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{0},{0:2},{0:4}")), 1.234567), SGE_WSTR("1.234567,1.23,1.2346"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("{:},{:2},{:4}")), -1.0, 2.234567, 3.234567), SGE_WSTR("-1.000000,2.23,3.2346"));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("this is {0},{1:2},{2:4} a {3} test.")), 1, 2.22f, 3.3333, SGE_WSTR("good")), SGE_WSTR("this is 1,2.22,3.3333 a good test."));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("this is {},{:2},{:4} a {} test.")), 1, 2.22f, 3.3333, SGE_WSTR("good")), SGE_WSTR("this is 1,2.22,3.3333 a good test."));
	ASSERT_EQ(Format(UCS2String(SGE_WSTR("test number format: {:4} {:8B} {:4D} {:4H}.")), 12, 0b1101, -34, 17), SGE_WSTR("test number format: 0012 00001101 -034 0011."));

	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{}{}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{0}{0}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{1}{1}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{0}{1}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{1}{0}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test format{1:test}{0:format}!"))), SGE_U8STR("test format!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{}, {}!")), SGE_U8STR("Hello"), SGE_U8STR("world")), SGE_U8STR("Hello, world!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0}, {1}!")), SGE_U8STR("Hello"), SGE_U8STR("world")), SGE_U8STR("Hello, world!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{1}, {0}!")), SGE_U8STR("world"), SGE_U8STR("Hello")), SGE_U8STR("Hello, world!"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{},{},{},{}")), SGE_U8STR("0"), SGE_U8STR("1")), SGE_U8STR("0,1,,"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0},{1},{2},{3}")), SGE_U8STR("0"), SGE_U8STR("1")), SGE_U8STR("0,1,,"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{2},{1},{0},{3}")), SGE_U8STR("0"), SGE_U8STR("1")), SGE_U8STR(",1,0,"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{2},{1},{0}")), SGE_U8STR("0"), SGE_U8STR("1"), SGE_U8STR("2"), SGE_U8STR("3"), SGE_U8STR("4"), SGE_U8STR("5")), SGE_U8STR("2,1,0"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0}+{0}={0}")), SGE_U8STR("0"), SGE_U8STR("1"), SGE_U8STR("2"), SGE_U8STR("3"), SGE_U8STR("4"), SGE_U8STR("5")), SGE_U8STR("0+0=0"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{{}} == {}{}")), SGE_U8STR("{"), SGE_U8STR("}")), SGE_U8STR("{} == {}"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0:D}=={0:B}=={0:H}")), 12), SGE_U8STR("12==1100==c"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{:D},{:B},{:H}")), -10, 11, 12), SGE_U8STR("-10,1011,c"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0},{0:2},{0:4}")), 1.234567f), SGE_U8STR("1.2346,1.23,1.2346"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{:},{:2},{:4}")), -1.0f, 2.234567f, 3.234567f), SGE_U8STR("-1.0000,2.23,3.2346"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{0},{0:2},{0:4}")), 1.234567), SGE_U8STR("1.234567,1.23,1.2346"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("{:},{:2},{:4}")), -1.0, 2.234567, 3.234567), SGE_U8STR("-1.000000,2.23,3.2346"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("this is {0},{1:2},{2:4} a {3} test.")), 1, 2.22f, 3.3333, SGE_U8STR("good")), SGE_U8STR("this is 1,2.22,3.3333 a good test."));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("this is {},{:2},{:4} a {} test.")), 1, 2.22f, 3.3333, SGE_U8STR("good")), SGE_U8STR("this is 1,2.22,3.3333 a good test."));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("这是{0}{{多字节的编码}}的特殊{1}测试")), SGE_U8STR("a"), SGE_U8STR("α")), SGE_U8STR("这是a{多字节的编码}的特殊α测试"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("这是{}{{多字节的编码}}的特殊{}测试")), SGE_U8STR("a"), SGE_U8STR("α")), SGE_U8STR("这是a{多字节的编码}的特殊α测试"));
	ASSERT_EQ(Format(UTF8String(SGE_U8STR("test number format: {:4} {:8B} {:4D} {:4H}.")), 12, 0b1101, -34, 17), SGE_U8STR("test number format: 0012 00001101 -034 0011."));
}