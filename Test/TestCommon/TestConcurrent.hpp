/*
Copyright 2019 Chenxi Xu (@xsun2001)

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
#include "Concurrent/Thread.h"
#include <atomic>
#include <chrono>

using namespace SpaceGameEngine;
using namespace std::chrono_literals;

std::atomic_flag flag = ATOMIC_FLAG_INIT;

void foo1()
{
	Thread::Sleep( 2s );
	flag.test_and_set();
	std::cout << "foo1() is running in thread " << Thread::GetCurrentThreadID() << std::endl;
}

TEST_CASE( "basic thread test", "[Common][Concurrent][Thread]" )
{
	std::cout << "main test thead " << Thread::GetCurrentThreadID() << std::endl;

	REQUIRE_FALSE( flag.test_and_set());
	flag.clear();

	Thread t( foo1 );

	REQUIRE_FALSE( flag.test_and_set());
	flag.clear();

	Thread::Sleep( 4s );

	REQUIRE( flag.test_and_set());
}