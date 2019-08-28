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
#include "Error.h"

using namespace SpaceGameEngine;

struct TestError
{
	inline static const TChar sm_pContent[] = SGE_TSTR("Test Error");
	inline static bool Judge()
	{
		return true;
	}
};

//TEST_CASE("Test error ", "[Common][Error]")
//{
//	SGE_ASSERT(TestError);
//}

TEST_CASE("Test normal error ", "[Common][Error]")
{
	SGE_ASSERT(NullPointerError, (void*)1);
	SGE_ASSERT(InvalidSizeError, 5, 1, 10);
}

TEST_CASE("Test error handle", "[Common][Error]")
{
	int test = 0;
#ifdef SGE_DEBUG
	SGE_ASSERT(TestError).Handle([&]() {test = 1; });
	REQUIRE(test == 1);
#endif
	SGE_CHECK(TestError).Handle([&]() {test = 2; });
	REQUIRE(test == 2);
}