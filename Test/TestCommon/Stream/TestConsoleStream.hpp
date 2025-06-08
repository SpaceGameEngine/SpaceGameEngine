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
#include "gtest/gtest.h"
#include "Stream/ConsoleStream.h"
#include <locale>

using namespace SpaceGameEngine;

TEST(ConsoleStream, Test)
{
	ConsoleStream stream;

	// can not set locale to make the console support the UCS2 encoding, so the display of Chinese characters will be wrong here.

	StreamWriter<StringSerializer<UCS2String>> ucs2_writer(stream);

	ucs2_writer << SGE_WSTR("Test ConsoleStream 测试 ") << 123456 << SGE_WSTR("\n");

	std::locale::global(std::locale("en_US.UTF-8"));	// set locale to UTF-8 to make the console support UTF-8 encoding

	StreamWriter<StringSerializer<UTF8String>> utf8_writer(stream);

	utf8_writer << SGE_U8STR("Test ConsoleStream 测试 ") << 789101 << SGE_U8STR("\n");

	// int test_value = 0;

	// can not set locale to make the console support the UCS2 encoding, so reading UCS2 encoded string will be wrong here.
	// StreamReader<StringSerializer<UCS2String>> ucs2_reader(stream);

	//// input 123456 from console
	// ucs2_reader >> test_value;	  // should read 123456
	// ASSERT_EQ(test_value, 123456);

	// comment the test of reading to avoid blocking the console when running these tests on CI.
	// StreamReader<StringSerializer<UTF8String>> utf8_reader(stream);

	//// input 789101 from console
	// utf8_reader >> test_value;	  // should read 789101
	// ASSERT_EQ(test_value, 789101);
}