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
#include "Stream/StreamReader.hpp"
#include "Stream/CumulateStream.hpp"
#include "Serializer/StringSerializer.hpp"
#include "TestStream.hpp"

using namespace SpaceGameEngine;

TEST(IsStreamReader, Test)
{
	ASSERT_TRUE(IsStreamReader<StreamReader<StringSerializer<UCS2String>>>);
	ASSERT_TRUE(IsStreamReader<StreamReader<StringSerializer<UTF8String>>>);
}

TEST(StreamReader, StringSerializerTest)
{
	Char16 ucs2_test_str[] = SGE_WSTR("123456");
	Char8 utf8_test_str[] = SGE_U8STR("789101");

	CumulateStream ucs2_stream, utf8_stream;
	ASSERT_TRUE(ucs2_stream.Write(ReferenceMemoryData((void*)ucs2_test_str, 6 * sizeof(Char16))));
	ASSERT_TRUE(utf8_stream.Write(ReferenceMemoryData((void*)utf8_test_str, 6 * sizeof(Char8))));

	int test_value = 0;

	StreamReader<StringSerializer<UCS2String>> ucs2_reader(ucs2_stream);
	ASSERT_FALSE(ucs2_reader >> test_value);	// touch the end of the stream
	ASSERT_EQ(test_value, 123456);

	StreamReader<StringSerializer<UTF8String>> utf8_reader(utf8_stream);
	ASSERT_FALSE(utf8_reader >> test_value);	// touch the end of the stream
	ASSERT_EQ(test_value, 789101);
}