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
#include "Utility/LockFreeFixedSizeRingBuffer.hpp"
#include "Concurrent/Lock.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(LockFreeFixedSizeRingBuffer, InstanceTest)
{
	LockFreeFixedSizeRingBuffer<16> buffer;
	LockFreeFixedSizeRingBuffer<4, 4> buffer2;
}

TEST(LockFreeFixedSizeRingBuffer, TryPushTest)
{
	LockFreeFixedSizeRingBuffer<16> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 16);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - sizeof(test_data));
	ASSERT_EQ(buffer.Pop(16, [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	}),
			  sizeof(test_data));
	test_data = 789101;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - sizeof(test_data));
	test_data = 124810;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), 2 * sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - 2 * sizeof(test_data));
	test_data = 135791;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), 3 * sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - 3 * sizeof(test_data));
	test_data = 235711;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), 4 * sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - 4 * sizeof(test_data));
	bool flag = false;
	buffer.Pop(16, [test_data, &flag](void* ptr, SizeType size) {
		UInt32* data_ptr = (UInt32*)ptr;
		if (!flag)
		{
			ASSERT_EQ(data_ptr[0], 789101);
			ASSERT_EQ(data_ptr[1], 124810);
			ASSERT_EQ(data_ptr[2], 135791);
			ASSERT_EQ(size, 3 * sizeof(test_data));
			flag = true;
		}
		else
		{
			ASSERT_EQ(*data_ptr, 235711);
			ASSERT_EQ(size, sizeof(test_data));
		}
	});

	LockFreeFixedSizeRingBuffer<4, 4> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 16);
	test_data = 123456;
	ASSERT_TRUE(buffer2.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 16 - sizeof(test_data));
	ASSERT_EQ(buffer2.Pop(16, [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	}),
			  sizeof(test_data));
	test_data = 789101;
	ASSERT_TRUE(buffer2.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 16 - sizeof(test_data));
	test_data = 124810;
	ASSERT_TRUE(buffer2.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer2.GetSize(), 2 * sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 16 - 2 * sizeof(test_data));
	test_data = 135791;
	ASSERT_TRUE(buffer2.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer2.GetSize(), 3 * sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 16 - 3 * sizeof(test_data));
	test_data = 235711;
	ASSERT_TRUE(buffer2.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer2.GetSize(), 4 * sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 16 - 4 * sizeof(test_data));
	int cnt = 0;
	UInt32 test_data2[] = {789101, 124810, 135791, 235711};
	ASSERT_EQ(buffer2.Pop(16, [test_data2, &cnt](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data2[cnt++]);
		ASSERT_EQ(size, sizeof(test_data2[0]));
	}),
			  sizeof(test_data2));
}

TEST(LockFreeFixedSizeRingBuffer, MultiThreadTryPushTest)
{
	LockFreeFixedSizeRingBuffer<1024> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 1024);
	Atomic<bool> is_start(false);
	Vector<Thread> threads;
	for (int i = 0; i < 1024 / 8; ++i)
	{
		Thread t(
			[&buffer, &is_start, i]() {
				while (!is_start.Load(MemoryOrder::Acquire))
					;
				UInt64 test_data = i;
				while (!buffer.TryPush(&test_data, sizeof(test_data)))
					;
			});
		threads.EmplaceBack(std::move(t));
	}
	is_start.Store(true, MemoryOrder::Release);
	for (auto iter = threads.GetBegin(); iter != threads.GetEnd(); ++iter)
	{
		iter->Join();
	}
	ASSERT_EQ(buffer.GetSize(), 1024);
	ASSERT_EQ(buffer.GetFreeSize(), 0);
	int cnt[1024 / 8];
	memset(cnt, 0, sizeof(cnt));
	ASSERT_EQ(buffer.Pop(1024, [&cnt](void* ptr, SizeType size) {
		ASSERT_EQ(size, 1024);
		for (SizeType i = 0; i < 1024 / 8; ++i)
		{
			UInt64 data = *(UInt64*)((Byte*)ptr + i * sizeof(UInt64));
			++cnt[data];
		}
	}),
			  1024);
	for (SizeType i = 0; i < 1024 / 8; ++i)
		ASSERT_EQ(cnt[i], 1);
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 1024);

	LockFreeFixedSizeRingBuffer<256, 4> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 1024);
	is_start.Store(false, MemoryOrder::Release);
	threads.Clear();
	for (int i = 0; i < 1024 / 8; ++i)
	{
		Thread t(
			[&buffer2, &is_start, i]() {
				while (!is_start.Load(MemoryOrder::Acquire))
					;
				UInt64 test_data = i;
				while (!buffer2.TryPush(&test_data, sizeof(test_data)))
					;
			});
		threads.EmplaceBack(std::move(t));
	}
	is_start.Store(true, MemoryOrder::Release);
	for (auto iter = threads.GetBegin(); iter != threads.GetEnd(); ++iter)
	{
		iter->Join();
	}
	ASSERT_EQ(buffer2.GetSize(), 1024);
	ASSERT_EQ(buffer2.GetFreeSize(), 0);
	memset(cnt, 0, sizeof(cnt));
	ASSERT_EQ(buffer2.Pop(1024, [&cnt](void* ptr, SizeType size) {
		ASSERT_EQ(size, 256);
		for (SizeType i = 0; i < 256 / 8; ++i)
		{
			UInt64 data = *(UInt64*)((Byte*)ptr + i * sizeof(UInt64));
			++cnt[data];
		}
	}),
			  1024);
	for (SizeType i = 0; i < 1024 / 8; ++i)
		ASSERT_EQ(cnt[i], 1);
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 1024);
}

TEST(LockFreeFixedSizeRingBuffer, TopTest)
{
	LockFreeFixedSizeRingBuffer<4, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 16);

	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - sizeof(test_data));

	ASSERT_EQ(buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	}),
			  sizeof(test_data));

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - sizeof(test_data));
}

TEST(LockFreeFixedSizeRingBuffer, PopTest)
{
	LockFreeFixedSizeRingBuffer<4, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 16);

	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 16 - sizeof(test_data));

	ASSERT_EQ(buffer.Pop(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	}),
			  sizeof(test_data));

	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 16);
}

TEST(LockFreeFixedSizeRingBuffer, CopyConstructionTest)
{
	LockFreeFixedSizeRingBuffer<8, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, MoveConstructionTest)
{
	LockFreeFixedSizeRingBuffer<8, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, CopyOperatorTest)
{
	LockFreeFixedSizeRingBuffer<8, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, MoveOperatorTest)
{
	LockFreeFixedSizeRingBuffer<8, 4> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, AnotherAllocatorCopyConstructionTest)
{
	LockFreeFixedSizeRingBuffer<8, 4, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4, StdAllocator> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, AnotherAllocatorMoveConstructionTest)
{
	LockFreeFixedSizeRingBuffer<8, 4, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4, StdAllocator> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, AnotherAllocatorCopyOperatorTest)
{
	LockFreeFixedSizeRingBuffer<8, 4, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeFixedSizeRingBuffer, AnotherAllocatorMoveOperatorTest)
{
	LockFreeFixedSizeRingBuffer<8, 4, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	ASSERT_TRUE(buffer.TryPush(&test_data, sizeof(test_data)));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	buffer.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});

	LockFreeFixedSizeRingBuffer<8, 4, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}