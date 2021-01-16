/*
Copyright 2019 creatorlxd

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
#include "Memory/Allocators.h"
#include "Concurrent/Thread.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

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
	ASSERT_EQ(GetDefaultAlignment(1), 4);
	ASSERT_EQ(GetDefaultAlignment(2), 4);
	ASSERT_EQ(GetDefaultAlignment(3), 4);
	ASSERT_EQ(GetDefaultAlignment(4), 4);
	ASSERT_EQ(GetDefaultAlignment(5), 4);
	ASSERT_EQ(GetDefaultAlignment(6), 4);
	ASSERT_EQ(GetDefaultAlignment(7), 4);
	ASSERT_EQ(GetDefaultAlignment(8), 4);
	ASSERT_EQ(GetDefaultAlignment(9), 4);
	ASSERT_EQ(GetDefaultAlignment(15), 4);
	ASSERT_EQ(GetDefaultAlignment(16), 16);
	ASSERT_EQ(GetDefaultAlignment(17), 16);
}

TEST(SegregatedFitAllocator, IndexConvertTest)
{
	auto rtoi = [](const Pair<SizeType, SizeType>& request_info) -> UInt32 {
		return (request_info.m_First << 8) | (request_info.m_Second);
	};
	auto index = rtoi(Pair<SizeType, SizeType>(1024, 128));
	ASSERT_EQ(index, 262272);
}

using AllocatorsToBeTested = ::testing::Types<SpaceGameEngine::NativeAllocator, SpaceGameEngine::SegregatedFitAllocator, SpaceGameEngine::NewSegregatedFitAllocator>;

template<typename AllocatorType>
class AllocatorTestBase : public ::testing::Test
{
};
TYPED_TEST_CASE(AllocatorTestBase, AllocatorsToBeTested);

TYPED_TEST(AllocatorTestBase, BasicAllocateTest)
{
	using AllocatorType = SpaceGameEngine::AllocatorWrapper<TypeParam>;
	Int32* pint = AllocatorType::template New<Int32>(3);
	ASSERT_EQ(*pint, 3);
	AllocatorType::template Delete(pint);
}

template<typename AllocatorType>
class ThreadSafetyTester : public ::testing::Test
{
public:
	void run_test(int idx)
	{
		std::vector<Int32*> pints;
		for (int i = 0; i < 1e5; i++)
		{
			Int32* pint = AllocatorWrapper<AllocatorType>::template New<Int32>(1);
			*pint = idx * 1e6 + i;
			pints.push_back(pint);
		}
		bool ok = true;
		for (int i = 0; i < 1e5; i++)
		{
			if (*pints[i] != idx * 1e6 + i)
			{
				ok = false;
				break;
			}
		}
		ASSERT_TRUE(ok);
	}
};
#define THREAD_NUMBER 10
TYPED_TEST_CASE(ThreadSafetyTester, AllocatorsToBeTested);
TYPED_TEST(ThreadSafetyTester, ThreadSafetyTest)
{
	std::vector<Thread> threads;
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		threads.emplace_back([=]() { this->run_test(i); });
	}
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		threads[i].Join();
	}
}
