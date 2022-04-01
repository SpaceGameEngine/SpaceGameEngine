/*
Copyright 2022 creatorlxd

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

#include "Concurrent/Lock.h"
#include "Concurrent/Thread.h"
#include "gtest/gtest.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <vector>

using namespace SpaceGameEngine;
using namespace std::chrono_literals;

#define LOG(s) std::cout << s << std::endl

std::atomic_flag flag1 = ATOMIC_FLAG_INIT;

void foo1()
{
	Thread::Sleep(MakeTimeDuration<Second, double>(2));
	flag1.test_and_set();
	LOG("foo1() is running in thread " << Thread::GetCurrentThreadId());
}

TEST(Concurrent, BasicThreadTest)
{
	LOG("main test thead " << Thread::GetCurrentThreadId());

	ASSERT_FALSE(flag1.test_and_set());
	flag1.clear();

	Thread t(foo1);

	ASSERT_FALSE(flag1.test_and_set());
	flag1.clear();

	Thread::Sleep(MakeTimeDuration<Second, double>(4));

	ASSERT_TRUE(flag1.test_and_set());
}

bool flag2_1 = false, flag2_2 = false;

void foo2(Mutex& mutex, bool& run_flag)
{
	RecursiveLock lock(mutex);
	lock.Lock();
	LOG("foo2 thread " << Thread::GetCurrentThreadId());

	ASSERT_TRUE((!flag2_1 && !flag2_2));

	run_flag = true;
	ASSERT_TRUE((flag2_1 != flag2_2));

	Thread::Sleep(MakeTimeDuration<Second, double>(1));

	run_flag = false;
	ASSERT_TRUE((!flag2_1 && !flag2_2));

	lock.Unlock();	  // optional
}

TEST(Concurrent, RecursiveLockTest1)
{
	Mutex m;
	Thread t1(foo2, std::ref(m), std::ref(flag2_1));
	Thread t2(foo2, std::ref(m), std::ref(flag2_2));
	t1.Join();
	t2.Join();
}

bool flag3 = false;

void foo3_lock(Mutex& mutex)
{
	RecursiveLock lock(mutex);
	lock.Lock();
	LOG("foo3_lock thread " << Thread::GetCurrentThreadId());
	ASSERT_FALSE(flag3);
	Thread::Sleep(MakeTimeDuration<Second, double>(1));
	flag3 = true;
	lock.Unlock();
}

void foo3_trylock(Mutex& mutex)
{
	RecursiveLock lock(mutex);
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_FALSE(lock.TryLock());
	ASSERT_TRUE(lock.TryLock(MakeTimeDuration<Second, double>(1)));
	LOG("foo3_trylock thread " << Thread::GetCurrentThreadId());
	ASSERT_TRUE(flag3);
	lock.Unlock();
}

TEST(Concurrent, RecursiveTryLockTest2)
{
	Mutex m;
	Thread t1(foo3_lock, std::ref(m));
	Thread t2(foo3_trylock, std::ref(m));
	t1.Join();
	t2.Join();
}

bool flag4 = false;

void foo4(Mutex& m1, Mutex& m2, Mutex& m3)
{
	ScopedLock lock(m1, m2, m3);

	LOG("foo4 thread " << Thread::GetCurrentThreadId());

	ASSERT_FALSE(flag4);
	flag4 = true;

	Thread::Sleep(MakeTimeDuration<Second, double>(0.1));

	ASSERT_TRUE(flag4);
	flag4 = false;
}

TEST(Concurrent, ScopedLockTest)
{
	Mutex m1, m2, m3;
	std::vector<Thread> threads;
	for (int i = 0; i < 10; i++)
	{
		threads.emplace_back(foo4, std::ref(m1), std::ref(m2), std::ref(m3));
	}
	for (Thread& thread : threads)
	{
		thread.Join();
	}
}

TEST(Concurrent, ConditionTest)
{
	Mutex m;
	Condition condition;
	std::vector<Thread> threads;
	bool flag = false;
	bool run_flags[10] = {false, false, false, false, false, false, false, false, false, false};

	for (bool& run_flag : run_flags)
	{
		threads.emplace_back([&] {
			RecursiveLock lock(m);
			lock.Lock();
			LOG("foo5 thread " << Thread::GetCurrentThreadId());
			condition.Wait(lock, [&] { return flag; });
			run_flag = true;
		});
	}

	auto checker = [&](int need) {
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

	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(0)));

	condition.NodifyOne();
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(0)));

	condition.NodifyAll();
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(0)));

	flag = true;

	condition.NodifyOne();
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(1)));

	condition.NodifyOne();
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(2)));

	condition.NodifyAll();
	Thread::Sleep(MakeTimeDuration<Second, double>(0.5));
	ASSERT_TRUE((checker(10)));

	for (Thread& thread : threads)
	{
		thread.Join();
	}
}