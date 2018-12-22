﻿/*
Copyright 2018 creatorlxd

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