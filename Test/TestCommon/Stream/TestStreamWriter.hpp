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
#include "Stream/StreamWriter.hpp"
#include "Stream/CumulateStream.hpp"
#include "Serializer/StringSerializer.hpp"
#include "TestStream.hpp"

using namespace SpaceGameEngine;

TEST(IsStreamWriter, Test)
{
	ASSERT_TRUE(IsStreamWriter<StreamWriter<StringSerializer<UCS2String>>>);
	ASSERT_TRUE(IsStreamWriter<StreamWriter<StringSerializer<UTF8String>>>);
}

TEST(StreamWriter, StringSerializerTest)
{
	Char16 ucs2_test_str[] = SGE_WSTR("123456");
	Char8 utf8_test_str[] = SGE_U8STR("789101");

	CumulateStream stream;

	StreamWriter<StringSerializer<UCS2String>> ucs2_writer(stream);
	ASSERT_TRUE(ucs2_writer << 123456);

	StreamWriter<StringSerializer<UTF8String>> utf8_writer(stream);
	ASSERT_TRUE(utf8_writer << 789101);

	MemoryData ucs2_data = NewMemoryData(6 * sizeof(Char16));
	MemoryData utf8_data = NewMemoryData(6 * sizeof(Char8));

	ASSERT_TRUE(stream.Read(ucs2_data));
	ASSERT_TRUE(stream.Read(utf8_data));

	ASSERT_EQ(ucs2_data.GetSize(), 6 * sizeof(Char16));
	ASSERT_EQ(memcmp(ucs2_data.GetData(), ucs2_test_str, 6 * sizeof(Char16)), 0);

	ASSERT_EQ(utf8_data.GetSize(), 6 * sizeof(Char8));
	ASSERT_EQ(memcmp(utf8_data.GetData(), utf8_test_str, 6 * sizeof(Char8)), 0);
}