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
#include "Catch2/catch.hpp"
#include "MemoryManager.h"

using namespace SpaceGameEngine;

TEST_CASE("Test StdAllocator", "[Common][MemoryManager]")
{
	SECTION("test new/delete with type")
	{
		Int32* pint = StdAllocator::New<Int32>(3);
		REQUIRE(*pint == 3);
		StdAllocator::Delete(pint);
	}
	SECTION("test raw new/delete")
	{
		Int32* pint = reinterpret_cast<Int32*>(StdAllocator::RawNew(sizeof(Int32)));
		*pint = 3;
		REQUIRE(*pint == 3);
		StdAllocator::RawDelete(pint, sizeof(Int32));
	}
}

TEST_CASE("Test Fundamental Function", "[Common][MemoryManager]")
{
	SECTION("test memory align macro")
	{
		REQUIRE(SGE_MEMORY_ALIGN(1, 2) == 2);
		REQUIRE(SGE_MEMORY_ALIGN(2, 2) == 2);
		REQUIRE(SGE_MEMORY_ALIGN(3, 2) == 4);
		REQUIRE(SGE_MEMORY_ALIGN(5, 2) == 6);
		REQUIRE(SGE_MEMORY_ALIGN(1, 4) == 4);
		REQUIRE(SGE_MEMORY_ALIGN(3, 4) == 4);
		REQUIRE(SGE_MEMORY_ALIGN(4, 4) == 4);
		REQUIRE(SGE_MEMORY_ALIGN(5, 4) == 8);
		REQUIRE(SGE_MEMORY_ALIGN(8, 4) == 8);
		REQUIRE(SGE_MEMORY_ALIGN(11, 4) == 12);
		REQUIRE(SGE_MEMORY_ALIGN(4, 8) == 8);
		REQUIRE(SGE_MEMORY_ALIGN(7, 8) == 8);
		REQUIRE(SGE_MEMORY_ALIGN(11, 8) == 16);
	}
	SECTION("test memory page")
	{
		MemoryManager::MemoryPageHeader* ppageheader = new (reinterpret_cast<MemoryManager::MemoryPageHeader*>(StdAllocator::RawNew(sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader)))) MemoryManager::MemoryPageHeader();
		REQUIRE(ppageheader->m_Offset == 0);
		REQUIRE(reinterpret_cast<AddressType>(ppageheader->GetFirstMemoryBlock()) == reinterpret_cast<AddressType>(ppageheader) + sizeof(MemoryManager::MemoryPageHeader));
		StdAllocator::RawDelete(ppageheader, (sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader)), alignof(MemoryManager::MemoryPageHeader));
	}
	SECTION("test alignment check")
	{
		REQUIRE(!InvalidAlignmentError::Judge(0));
		REQUIRE(!InvalidAlignmentError::Judge(1));
		REQUIRE(!InvalidAlignmentError::Judge(2));
		REQUIRE(!InvalidAlignmentError::Judge(4));
		REQUIRE(!InvalidAlignmentError::Judge(8));
		REQUIRE(InvalidAlignmentError::Judge(3));
		REQUIRE(InvalidAlignmentError::Judge(5));
		REQUIRE(InvalidAlignmentError::Judge(6));
		REQUIRE(InvalidAlignmentError::Judge(7));
	}
	SECTION("test default alignment")
	{
		REQUIRE(GetDefaultAlignment(1) == 4);
		REQUIRE(GetDefaultAlignment(2) == 4);
		REQUIRE(GetDefaultAlignment(3) == 4);
		REQUIRE(GetDefaultAlignment(4) == 4);
		REQUIRE(GetDefaultAlignment(5) == 4);
		REQUIRE(GetDefaultAlignment(6) == 4);
		REQUIRE(GetDefaultAlignment(7) == 4);
		REQUIRE(GetDefaultAlignment(8) == 4);
		REQUIRE(GetDefaultAlignment(9) == 4);
		REQUIRE(GetDefaultAlignment(15) == 4);
		REQUIRE(GetDefaultAlignment(16) == 16);
		REQUIRE(GetDefaultAlignment(17) == 16);
	}
}

TEST_CASE("Test FixedSizeAllocator", "[Common][MemoryManager]")
{
	SECTION("test instance")
	{
		MemoryManager::FixedSizeAllocator test(4, 0xffff, 4);
	}
	SECTION("test allocate/free")
	{
		MemoryManager::FixedSizeAllocator test(4, 0xffff, 4);
		Int32* pint = (Int32*)test.Allocate();
		*pint = 123456789;
		test.Free(pint);
		Int32* pint2 = (Int32*)test.Allocate();
		REQUIRE(pint == pint2);
		test.Free(pint2);
	}
}

TEST_CASE("Test MemoryManager", "[Common][MemoryManager]")
{
	SECTION("test get singleton")
	{
		decltype(auto) mm = MemoryManager::GetSingleton();
	}
	SECTION("test index and request information convert")
	{
		auto rtoi = [](const Pair<SizeType, SizeType>& request_info) -> UInt32 { return (request_info.m_First << 8) | (request_info.m_Second); };
		auto index = rtoi(Pair<SizeType, SizeType>(1024, 128));
		REQUIRE(index == 262272);
	}
	SECTION("test allocate/free")
	{
		Int32* pint = (Int32*)(MemoryManager::GetSingleton().Allocate(sizeof(Int32), alignof(Int32)));
		*pint = 123456789;
		REQUIRE(*pint == 123456789);
		MemoryManager::GetSingleton().Free(pint, sizeof(Int32), alignof(Int32));
	}
}

TEST_CASE("Test MemoryManagerAllocator", "[Common][MemoryManager]")
{
	SECTION("test new/delete with type")
	{
		Int32* pint = MemoryManagerAllocator::New<Int32>(3);
		REQUIRE(*pint == 3);
		MemoryManagerAllocator::Delete(pint);
	}
	SECTION("test raw new/delete")
	{
		Int32* pint = reinterpret_cast<Int32*>(MemoryManagerAllocator::RawNew(sizeof(Int32)));
		*pint = 3;
		REQUIRE(*pint == 3);
		MemoryManagerAllocator::RawDelete(pint, sizeof(Int32));
	}
}