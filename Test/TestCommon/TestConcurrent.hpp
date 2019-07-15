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
#include <vector>

using namespace SpaceGameEngine;
using namespace std::chrono_literals;

#define LOG( s ) std::cout << s << std::endl

std::atomic_flag flag1 = ATOMIC_FLAG_INIT;

void foo1()
{
	Thread::Sleep( 2s );
	flag1.test_and_set();
	LOG( "foo1() is running in thread " << Thread::GetCurrentThreadId());
}

TEST_CASE( "basic thread test", "[Common][Concurrent][Thread]" )
{
	LOG( "main test thead " << Thread::GetCurrentThreadId());

	REQUIRE_FALSE( flag1.test_and_set());
	flag1.clear();

	Thread t( foo1 );

	REQUIRE_FALSE( flag1.test_and_set());
	flag1.clear();

	Thread::Sleep( 4s );

	REQUIRE( flag1.test_and_set());
}

bool flag2_1 = false, flag2_2 = false;

void foo2( Mutex &mutex, bool &run_flag )
{
	ReentrantLock lock( mutex );
	lock.Lock();
	LOG( "foo2 thread " << Thread::GetCurrentThreadId());

	REQUIRE(( !flag2_1 && !flag2_2 ));

	run_flag = true;
	REQUIRE(( flag2_1 != flag2_2 ));

	Thread::Sleep( 1s );

	run_flag = false;
	REQUIRE(( !flag2_1 && !flag2_2 ));

	lock.Unlock();  //optional
}

TEST_CASE( "ReentrantLock Lock/Unlock test", "[Common][Concurrent][Lock]" )
{
	Mutex m;
	Thread t1( foo2, std::ref( m ), std::ref( flag2_1 ));
	Thread t2( foo2, std::ref( m ), std::ref( flag2_2 ));
	t1.Join();
	t2.Join();
}

bool flag3 = false;

void foo3_lock( Mutex &mutex )
{
	ReentrantLock lock( mutex );
	lock.Lock();
	LOG( "foo3_lock thread " << Thread::GetCurrentThreadId());
	REQUIRE_FALSE( flag3 );
	Thread::Sleep( 1s );
	flag3 = true;
	lock.Unlock();
}

void foo3_trylock( Mutex &mutex )
{
	ReentrantLock lock( mutex );
	Thread::Sleep( .5s );
	REQUIRE_FALSE( lock.TryLock());
	REQUIRE( lock.TryLock( 1s ));
	LOG( "foo3_trylock thread " << Thread::GetCurrentThreadId());
	REQUIRE( flag3 );
	lock.Unlock();
}

TEST_CASE( "ReentrantLock TryLock test", "[Common][Concurrent][Lock]" )
{
	Mutex m;
	Thread t1( foo3_lock, std::ref( m ));
	Thread t2( foo3_trylock, std::ref( m ));
	t1.Join();
	t2.Join();
}

bool flag4 = false;

void foo4( Mutex &m1, Mutex &m2, Mutex &m3 )
{
	ScopedLock lock( m1, m2, m3 );

	LOG( "foo4 thread " << Thread::GetCurrentThreadId());

	REQUIRE_FALSE( flag4 );
	flag4 = true;

	Thread::Sleep( .1s );

	REQUIRE( flag4 );
	flag4 = false;
}

TEST_CASE( "ScopedLock test", "[Common][Concurrent][Lock]" )
{
	Mutex m1, m2, m3;
	std::vector<Thread> threads;
	for (int i = 0; i < 10; i++)
	{
		threads.emplace_back( foo4, std::ref( m1 ), std::ref( m2 ), std::ref( m3 ));
	}
	for (Thread &thread : threads)
	{
		thread.Join();
	}
}

TEST_CASE( "Condition test", "[Common][Concurrent][Lock]" )
{
	Mutex m;
	Condition condition;
	std::vector<Thread> threads;
	bool flag = false;
	bool run_flags[10] = { false, false, false, false, false, false, false, false, false, false };

	for (bool &run_flag : run_flags)
	{
		threads.emplace_back(
				[&]
				{
					ReentrantLock lock( m );
					lock.Lock();
					LOG( "foo5 thread " << Thread::GetCurrentThreadId());
					condition.Wait( lock, [&] { return flag; } );
					run_flag = true;
				} );
	}

	auto checker = [&]( int need )
	{
		int count = 0;
		for (bool run_flag : run_flags)
		{
			if (run_flag)
			{
				++count;
			}
		}
		return need == count;
	};

	Thread::Sleep( .5s );
	REQUIRE(( checker( 0 )));

	condition.NodifyOne();
	Thread::Sleep( .5s );
	REQUIRE(( checker( 0 )));

	condition.NodifyAll();
	Thread::Sleep( .5s );
	REQUIRE(( checker( 0 )));

	flag = true;

	condition.NodifyOne();
	Thread::Sleep( .5s );
	REQUIRE(( checker( 1 )));

	condition.NodifyOne();
	Thread::Sleep( .5s );
	REQUIRE(( checker( 2 )));

	condition.NodifyAll();
	Thread::Sleep( .5s );
	REQUIRE(( checker( 10 )));

	for (Thread &thread:threads)
	{
		thread.Join();
	}
}