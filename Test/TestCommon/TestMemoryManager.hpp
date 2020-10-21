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
#include <Memory/Detail/AllocatorUtil.h>
#include <Memory/Detail/SegregatedFitAllocator.h>
#include "Memory/MemoryManager.h"
#include "Concurrent/Thread.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(MemoryManager, StdAllocatorNewDeleteTest)
{
	Int32* pint = SpaceGameEngine::StdAllocator::New<Int32>(3);
	ASSERT_EQ(*pint, 3);
	SpaceGameEngine::StdAllocator::Delete(pint);
}

TEST(MemoryManager, StdAllocatorRawNewDeleteTest)
{
	Int32* pint = reinterpret_cast<Int32*>(SpaceGameEngine::StdAllocator::RawNew(sizeof(Int32)));
	*pint = 3;
	ASSERT_EQ(*pint, 3);
	SpaceGameEngine::StdAllocator::RawDelete(pint, sizeof(Int32));
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
		new (reinterpret_cast<MemoryManager::MemoryPageHeader*>(SpaceGameEngine::StdAllocator::RawNew(
			sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader))))
			MemoryManager::MemoryPageHeader();
	ASSERT_EQ(ppageheader->m_Offset, 0);
	ASSERT_EQ(reinterpret_cast<AddressType>(ppageheader->GetFirstMemoryBlock()),
			  reinterpret_cast<AddressType>(ppageheader) + sizeof(MemoryManager::MemoryPageHeader));
	SpaceGameEngine::StdAllocator::RawDelete(ppageheader,
											 (sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader)),
											 alignof(MemoryManager::MemoryPageHeader));
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
	MemoryManagerAllocator::RawDelete(pint, sizeof(Int32));
}

template<int i>
struct DammyAllocator
{
	void* allocate(SizeType size, SizeType alignment)
	{
		return new int(i);
	}
	void deallocate(void* pointer, SizeType size, SizeType alignment)
	{
		delete (int*)pointer;
	}
};

TEST(MM, MM)
{
	SpaceGameEngine::Detail::DoubleGradedAllocator<DammyAllocator<1>, 10, DammyAllocator<2>> dga{};
	SpaceGameEngine::Detail::TripleGradedAllocator<DammyAllocator<1>, 10, DammyAllocator<2>, 100, DammyAllocator<3>> tga{};

	int* p1 = static_cast<int*>(dga.allocate(1, 0));
	int* p2 = static_cast<int*>(dga.allocate(11, 0));
	int* p3 = static_cast<int*>(tga.allocate(1, 0));
	int* p4 = static_cast<int*>(tga.allocate(11, 0));
	int* p5 = static_cast<int*>(tga.allocate(111, 0));

	ASSERT_EQ(*p1, 1);
	ASSERT_EQ(*p2, 2);
	ASSERT_EQ(*p3, 1);
	ASSERT_EQ(*p4, 2);
	ASSERT_EQ(*p5, 3);

	dga.deallocate(p1, 1, 0);
	dga.deallocate(p2, 11, 0);
	tga.deallocate(p3, 1, 0);
	tga.deallocate(p4, 11, 0);
	tga.deallocate(p5, 111, 0);
}

void concurrent_test()
{
	for (int i = 0; i < 100; i++)
	{
	}
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
			Int32* pint = AllocatorType::template New<Int32>(1);
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
using AllocatorsToBeTested = ::testing::Types<SpaceGameEngine::StdAllocator, SpaceGameEngine::MemoryManagerAllocator>;
TYPED_TEST_CASE(ThreadSafetyTester, AllocatorsToBeTested);
TYPED_TEST(ThreadSafetyTester, ThreadSafetyTest)
{
	std::vector<Thread> threads;
	for (int i = 0; i < 20; i++)
	{
		threads.emplace_back([=]() { this->run_test(i); });
	}
	for (int i = 0; i < 20; i++)
	{
		threads[i].Join();
	}
}