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
#include "gtest/gtest.h"
#include "Stream/CumulateStream.hpp"

using namespace SpaceGameEngine;

TEST(CumulateStream, Test)
{
	CumulateStream stream;

	const char* testData1 = "Hello, ";
	const char* testData2 = "World!";
	size_t dataSize1 = strlen(testData1);
	size_t dataSize2 = strlen(testData2);

	MemoryData data1 = ReferenceMemoryData((void*)testData1, dataSize1);
	ASSERT_TRUE(stream.Write(data1));

	MemoryData data2 = ReferenceMemoryData((void*)testData2, dataSize2);
	ASSERT_TRUE(stream.Write(data2));

	MemoryData readData = NewMemoryData(stream.GetSize());
	ASSERT_TRUE(stream.Read(readData));

	ASSERT_EQ(readData.GetSize(), dataSize1 + dataSize2);

	const char* expectedCombined = "Hello, World!";
	ASSERT_EQ(memcmp(readData.GetData(), expectedCombined, dataSize1 + dataSize2), 0);

	MemoryData emptyReadData;
	ASSERT_FALSE(stream.Read(emptyReadData));

	MemoryData emptyData;
	ASSERT_FALSE(stream.Write(emptyData));

	ASSERT_TRUE(stream.Write(data1));

	MemoryData readData2 = NewMemoryData(stream.GetSize());
	ASSERT_TRUE(stream.Read(readData2));

	ASSERT_EQ(readData2.GetSize(), dataSize1);
	ASSERT_EQ(memcmp(readData2.GetData(), testData1, dataSize1), 0);
}