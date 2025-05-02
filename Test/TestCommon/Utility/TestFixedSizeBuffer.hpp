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
#include "Utility/FixedSizeBuffer.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(FixedSizeBuffer, InstanceTest)
{
	FixedSizeBuffer<32> buffer;
}

TEST(FixedSizeBuffer, AppendTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, SetSizeAndClearTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
	buffer.SetSize(2);
	ASSERT_EQ(buffer.GetSize(), 2);
	ASSERT_EQ(buffer.GetFreeSize(), 30);
	buffer.Clear();
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
}

TEST(FixedSizeBuffer, CopyConstructionTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, MoveConstructionTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, CopyOperatorTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, MoveOperatorTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorCopyConstructionTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorMoveConstructionTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorCopyOperatorTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorMoveOperatorTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}