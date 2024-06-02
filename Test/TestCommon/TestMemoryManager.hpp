/*
Copyright 2024 creatorlxd

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
#include "MemoryManager.h"
#include "Concurrent/Thread.h"
#include "Concurrent/Lock.h"
#include "gtest/gtest.h"
#include <unordered_map>
#include <cstddef>

using namespace SpaceGameEngine;

TEST(MemoryManager, StdAllocatorNewDeleteTest)
{
	Int32* pint = StdAllocator::New<Int32>(3);
	ASSERT_EQ(*pint, 3);
	StdAllocator::Delete(pint);
}

TEST(MemoryManager, StdAllocatorRawNewDeleteTest)
{
	Int32* pint = reinterpret_cast<Int32*>(StdAllocator::RawNew(sizeof(Int32)));
	*pint = 3;
	ASSERT_EQ(*pint, 3);
	StdAllocator::RawDelete(pint);
}

TEST(MemoryManager, MemoryAlignMacroTest)
{
	ASSERT_EQ(SGE_MEMORY_ALIGN(1, 2), 2);
	ASSERT_EQ(SGE_MEMORY_ALIGN(2, 2), 2);
	ASSERT_EQ(SGE_MEMORY_ALIGN(3, 2), 4);
	ASSERT_EQ(SGE_MEMORY_ALIGN(5, 2), 6);
	ASSERT_EQ(SGE_MEMORY_ALIGN(1, 4), 4);
	ASSERT_EQ(SGE_MEMORY_ALIGN(3, 4), 4);
	ASSERT_EQ(SGE_MEMORY_ALIGN(4, 4), 4);
	ASSERT_EQ(SGE_MEMORY_ALIGN(5, 4), 8);
	ASSERT_EQ(SGE_MEMORY_ALIGN(8, 4), 8);
	ASSERT_EQ(SGE_MEMORY_ALIGN(11, 4), 12);
	ASSERT_EQ(SGE_MEMORY_ALIGN(4, 8), 8);
	ASSERT_EQ(SGE_MEMORY_ALIGN(7, 8), 8);
	ASSERT_EQ(SGE_MEMORY_ALIGN(11, 8), 16);
}

TEST(MemoryManager, MemoryPageTest)
{
	MemoryManager::MemoryPageHeader* ppageheader =
		new (reinterpret_cast<MemoryManager::MemoryPageHeader*>(StdAllocator::RawNew(
			sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader))))
			MemoryManager::MemoryPageHeader();
	ASSERT_EQ(ppageheader->m_Offset, 0);
	ASSERT_EQ(reinterpret_cast<AddressType>(ppageheader->GetFirstMemoryBlock()),
			  reinterpret_cast<AddressType>(ppageheader) + sizeof(MemoryManager::MemoryPageHeader));
	StdAllocator::RawDelete(ppageheader);
}

TEST(MemoryManager, InvalidAlignmentErrorTest)
{
	ASSERT_FALSE(InvalidAlignmentError::Judge(0));
	ASSERT_FALSE(InvalidAlignmentError::Judge(1));
	ASSERT_FALSE(InvalidAlignmentError::Judge(2));
	ASSERT_FALSE(InvalidAlignmentError::Judge(4));
	ASSERT_FALSE(InvalidAlignmentError::Judge(8));
	ASSERT_TRUE(InvalidAlignmentError::Judge(3));
	ASSERT_TRUE(InvalidAlignmentError::Judge(5));
	ASSERT_TRUE(InvalidAlignmentError::Judge(6));
	ASSERT_TRUE(InvalidAlignmentError::Judge(7));
}

TEST(MemoryManager, GetDefaultAlignmentTest)
{
	ASSERT_EQ(GetDefaultAlignment(1), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(2), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(3), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(4), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(5), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(6), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(7), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(8), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(9), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(15), alignof(void*));
	ASSERT_EQ(GetDefaultAlignment(16), 16);
	ASSERT_EQ(GetDefaultAlignment(17), 16);
}

TEST(MemoryManager, FixedSizeAllocatorTest)
{
	MemoryManager::FixedSizeAllocator test(4, 0xffff, 4);
	Int32* pint = (Int32*)test.Allocate();
	*pint = 123456789;
	test.Free(pint);
	Int32* pint2 = (Int32*)test.Allocate();
	ASSERT_EQ(pint, pint2);
	test.Free(pint2);
}

TEST(MemoryManager, FixedSizeAllocatorMultiThreadTest)
{
	MemoryManager::FixedSizeAllocator test(4, 0xffff, 4);

	const int test_size = 64;
	Thread ts[test_size];
	AddressType res[test_size];
	memset(res, 0, sizeof(res));
	for (int i = 0; i < test_size; i++)
	{
		ts[i] = std::move(Thread(
			[&](int idx) {
				SleepFor(MakeTimeDuration<Second, TimeType>(1));
				res[idx] = (AddressType)test.Allocate();
				*((Int32*)res[idx]) = idx;
			},
			i));
	}
	for (int i = 0; i < test_size; i++)
	{
		ts[i].Join();
	}
	std::unordered_map<AddressType, int> cnt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(res[i] != 0);
		ASSERT_EQ(*((Int32*)res[i]), i);
		cnt[res[i]] += 1;
	}
	for (const auto& i : cnt)
	{
		ASSERT_TRUE(i.second == 1);
	}
	for (int i = 0; i < test_size; i++)
	{
		test.Free((void*)res[i]);
	}
}

/*
TEST(MemoryManager, MultiThreadBufferedFixedSizeAllocatorMultiThreadTest)
{
	MemoryManager::MultiThreadBufferedFixedSizeAllocator test(4, 0xffff, 4);

	const int test_size = 64;
	Thread ts[test_size];
	Mutex mutex[test_size];
	AddressType res[test_size];
	memset(res, 0, sizeof(res));
	Condition start_c;
	bool is_start = false;
	for (int i = 0; i < test_size; i++)
	{
		ts[i] = std::move(Thread(
			[&](int idx) {
				RecursiveLock lock(mutex[idx]);
				lock.Lock();
				start_c.Wait(lock, [&]() { return is_start; });
				res[idx] = (AddressType)test.Allocate();
				*((Int32*)res[idx]) = idx;
			},
			i));
	}
	is_start = true;
	start_c.NodifyAll();
	for (int i = 0; i < test_size; i++)
	{
		ts[i].Join();
	}
	std::unordered_map<AddressType, int> cnt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(res[i] != 0);
		ASSERT_EQ(*((Int32*)res[i]), i);
		cnt[res[i]] += 1;
	}
	for (const auto& i : cnt)
	{
		ASSERT_TRUE(i.second == 1);
	}
	for (int i = 0; i < test_size; i++)
	{
		test.Free((void*)res[i]);
	}
}
*/

TEST(MemoryManager, IndexConvertTest)
{
	auto rtoi = [](const Pair<SizeType, SizeType>& request_info) -> UInt32 {
		return (request_info.m_First << 8) | (request_info.m_Second);
	};
	auto index = rtoi(Pair<SizeType, SizeType>(1024, 128));
	ASSERT_EQ(index, 262272);
}

TEST(MemoryManager, MemoryManagerTest)
{
	Int32* pint = (Int32*)(MemoryManager::GetSingleton().Allocate(sizeof(Int32), alignof(Int32)));
	*pint = 123456789;
	ASSERT_EQ(*pint, 123456789);
	MemoryManager::GetSingleton().Free(pint, sizeof(Int32), alignof(Int32));
}

TEST(MemoryManager, MMAllocatorNewDeleteTest)
{
	Int32* pint = MemoryManagerAllocator::New<Int32>(3);
	ASSERT_EQ(*pint, 3);
	MemoryManagerAllocator::Delete(pint);
}

TEST(MemoryManager, MMAllocatorRawNewDeleteTest)
{
	Int32* pint = reinterpret_cast<Int32*>(MemoryManagerAllocator::RawNew(sizeof(Int32)));
	*pint = 3;
	ASSERT_EQ(*pint, 3);
	MemoryManagerAllocator::RawDelete(pint);
}