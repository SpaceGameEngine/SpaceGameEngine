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
#include "Container/Concurrent/LockFreeSCSPFixedSizeByteRingBuffer.hpp"
#include "Concurrent/Lock.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(LockFreeSCSPFixedSizeByteRingBuffer, InstanceTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<16> buffer;
}

TEST(LockFreeSCSPFixedSizeByteRingBuffer, TryPushTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<16> buffer;
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
}

TEST(LockFreeSCSPFixedSizeByteRingBuffer, SCSPTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<1024> buffer;
	int test_cnt[1024 / 4];
	memset(test_cnt, 0, sizeof(test_cnt));
	Atomic<bool> is_start = false;
	Thread producer([&buffer, &is_start]() {
		while (!is_start.Load(MemoryOrder::Acquire))
			;
		for (UInt32 i = 0; i < 1024 / 4; ++i)
		{
			while (!buffer.TryPush(&i, sizeof(i)))
				;
		}
	});
	Thread consumer([&buffer, &test_cnt, &is_start]() {
		while (!is_start.Load(MemoryOrder::Acquire))
			;
		for (UInt32 i = 0; i < 1024 / 4; ++i)
		{
			while (!buffer.Pop(sizeof(i), [&test_cnt, i](void* ptr, SizeType size) {
				ASSERT_EQ(size, sizeof(i));
				test_cnt[*(UInt32*)ptr]++;
			}))
				;
		}
	});
	is_start.Store(true, MemoryOrder::Release);
	producer.Join();
	consumer.Join();
	for (SizeType i = 0; i < 1024 / 4; ++i)
		ASSERT_EQ(test_cnt[i], 1);
}

TEST(LockFreeSCSPFixedSizeByteRingBuffer, TopTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<16> buffer;
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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, PopTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<16> buffer;
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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, CopyConstructionTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer2(buffer);

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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, MoveConstructionTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeSCSPFixedSizeByteRingBuffer, CopyOperatorTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer2;
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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, MoveOperatorTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32> buffer2;
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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, AnotherAllocatorCopyConstructionTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32, MemoryManagerAllocator> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32, StdAllocator> buffer2(buffer);

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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, AnotherAllocatorMoveConstructionTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32, MemoryManagerAllocator> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32, StdAllocator> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	buffer2.Top(sizeof(test_data), [test_data](void* ptr, SizeType size) {
		ASSERT_EQ(*(UInt32*)ptr, test_data);
		ASSERT_EQ(size, sizeof(test_data));
	});
}

TEST(LockFreeSCSPFixedSizeByteRingBuffer, AnotherAllocatorCopyOperatorTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32, MemoryManagerAllocator> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32, StdAllocator> buffer2;
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

TEST(LockFreeSCSPFixedSizeByteRingBuffer, AnotherAllocatorMoveOperatorTest)
{
	LockFreeSCSPFixedSizeByteRingBuffer<32, MemoryManagerAllocator> buffer;
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

	LockFreeSCSPFixedSizeByteRingBuffer<32, StdAllocator> buffer2;
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