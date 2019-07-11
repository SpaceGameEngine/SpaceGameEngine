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
#include "Concurrent/Lock.h"
#include <atomic>
#include <chrono>

using namespace SpaceGameEngine;
using namespace std::chrono_literals;

#define LOG( s ) std::cout << s << std::endl

std::atomic_flag flag = ATOMIC_FLAG_INIT;

void foo1()
{
	Thread::Sleep( 2s );
	flag.test_and_set();
	LOG( "foo1() is running in thread " << Thread::GetCurrentThreadId());
}

TEST_CASE( "basic thread test", "[Common][Concurrent][Thread]" )
{
	LOG( "main test thead " << Thread::GetCurrentThreadId());

	REQUIRE_FALSE( flag.test_and_set());
	flag.clear();

	Thread t( foo1 );

	REQUIRE_FALSE( flag.test_and_set());
	flag.clear();

	Thread::Sleep( 4s );

	REQUIRE( flag.test_and_set());
}

bool flag1, flag2;
Mutex mutex;

void foo2( bool &run_flag )
{
	LOG( "foo2 thread " << Thread::GetCurrentThreadId());
	ReentrantLock lock( mutex );
	run_flag = true;
	Thread::Sleep( 1s );
	run_flag = false;
	lock.Unlock();
}

TEST_CASE( "ReentrantLock test", "[Common][Concurrent][Lock]" )
{
	Thread t1( foo2, std::ref( flag1 )), t2( foo2, std::ref( flag2 ));

	Thread::Sleep( .5s );
	REQUIRE( flag1 != flag2 );

	bool &next = flag1 ? flag2 : flag1;
	Thread::Sleep( 1s );
	REQUIRE( next );
	REQUIRE(( flag1 != flag2 ));

	Thread::Sleep( 1.5s );
	REQUIRE(( !flag1 && !flag2 ));
}