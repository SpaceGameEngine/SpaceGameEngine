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
#include "Serializer/StringSerializer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(StringSerializer, IsSerializerTest)
{
	ASSERT_TRUE(IsSerializer<StringSerializer<UCS2String>>);
	ASSERT_TRUE(IsSerializer<StringSerializer<UTF8String>>);
}

TEST(StringSerializer, Test)
{
	int input_value = 123456789;
	MemoryData ucs2_md = StringSerializer<UCS2String>::Serialize(input_value);
	ASSERT_EQ(ucs2_md.GetSize(), 9 * sizeof(Char16));
	ASSERT_EQ(memcmp(ucs2_md.GetData(), SGE_WSTR("123456789"), ucs2_md.GetSize()), 0);

	int output_value = StringSerializer<UCS2String>::Deserialize<int>(ucs2_md);
	ASSERT_EQ(output_value, input_value);

	MemoryData utf8_md = StringSerializer<UTF8String>::Serialize(input_value);
	ASSERT_EQ(utf8_md.GetSize(), 9 * sizeof(Char8));
	ASSERT_EQ(memcmp(utf8_md.GetData(), SGE_U8STR("123456789"), utf8_md.GetSize()), 0);

	output_value = StringSerializer<UTF8String>::Deserialize<int>(utf8_md);
	ASSERT_EQ(output_value, input_value);
}