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
#include "../../ThirdParty/Catch2/catch.hpp"
#include "../../Source/Common/Public/MemoryManager.h"

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
		StdAllocator::RawDelete(pint);
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
		MemoryManager::MemoryPageHeader* ppageheader = reinterpret_cast<MemoryManager::MemoryPageHeader*>(StdAllocator::RawNew(sizeof(MemoryManager::MemoryPageHeader) + sizeof(MemoryManager::MemoryBlockHeader)));
		REQUIRE(reinterpret_cast<AddressType>(ppageheader->GetFirstMemoryBlock()) == reinterpret_cast<AddressType>(ppageheader) + sizeof(MemoryManager::MemoryBlockHeader));
		StdAllocator::RawDelete(ppageheader);
	}
}