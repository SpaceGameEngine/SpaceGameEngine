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
#include "Utility/LockFreeFixedSizeBuffer.hpp"
#include "Concurrent/Lock.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(LockFreeFixedSizeBuffer, InstanceTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
}

TEST(LockFreeFixedSizeBuffer, TryAppendTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, MultiThreadTryAppendTest)
{
	LockFreeFixedSizeBuffer<1024> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 1024);
	Atomic<bool> is_start{false};
	Vector<Thread> threads;
	for (int i = 0; i < 1024 / 8; ++i)
	{
		Thread t(
			[&buffer, &is_start, i]() {
				while (!is_start.Load(MemoryOrder::Acquire))
					;
				UInt64 test_data = i;
				while (!buffer.TryAppend(&test_data, sizeof(test_data)))
					;
			});
		threads.EmplaceBack(std::move(t));
	}
	is_start.Store(true, MemoryOrder::Release);
	for (auto iter = threads.GetBegin(); iter != threads.GetEnd(); ++iter)
	{
		iter->Join();
	}
	int cnt[1024 / 8];
	memset(cnt, 0, sizeof(cnt));
	for (SizeType i = 0; i < 1024 / 8; ++i)
	{
		UInt64 data = *(UInt64*)((Byte*)buffer.GetData() + i * sizeof(UInt64));
		++cnt[data];
	}
	for (SizeType i = 0; i < 1024 / 8; ++i)
		ASSERT_EQ(cnt[i], 1);
	ASSERT_EQ(buffer.GetSize(), 1024);
	ASSERT_EQ(buffer.GetFreeSize(), 0);
}

TEST(LockFreeFixedSizeBuffer, SetSizeAndClearTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
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

TEST(LockFreeFixedSizeBuffer, CopyConstructionTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, MoveConstructionTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, CopyOperatorTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32> buffer2;
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

TEST(LockFreeFixedSizeBuffer, MoveOperatorTest)
{
	LockFreeFixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, AnotherAllocatorCopyConstructionTest)
{
	LockFreeFixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32, StdAllocator> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, AnotherAllocatorMoveConstructionTest)
{
	LockFreeFixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32, StdAllocator> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(LockFreeFixedSizeBuffer, AnotherAllocatorCopyOperatorTest)
{
	LockFreeFixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32, StdAllocator> buffer2;
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

TEST(LockFreeFixedSizeBuffer, AnotherAllocatorMoveOperatorTest)
{
	LockFreeFixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryAppend(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	LockFreeFixedSizeBuffer<32, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}