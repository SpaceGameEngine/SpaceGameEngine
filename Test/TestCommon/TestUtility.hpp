﻿/*
Copyright 2021 creatorlxd

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
#include "Utility/ControllableObject.hpp"
#include "Utility/Utility.hpp"
#include "Utility/AutoReleaseBuffer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

class test_cmp
{
	int i;

public:
	bool operator==(const test_cmp&) const = delete;
};
class test_cmp2
{
	int i;
};
bool operator==(const test_cmp2&, const test_cmp2&) = delete;

struct test_controllable_object_class
{
	test_controllable_object_class()
	{
		i = 1;
	}
	~test_controllable_object_class()
	{
		i = 0;
	}
	test_controllable_object_class(const test_controllable_object_class& obj)
	{
		i = 2;
	}
	test_controllable_object_class(test_controllable_object_class&& obj)
	{
		i = 3;
	}

	test_controllable_object_class& operator=(const test_controllable_object_class& obj)
	{
		i = 4;
		return *this;
	}
	test_controllable_object_class& operator=(test_controllable_object_class&& obj)
	{
		i = 5;
		return *this;
	}

	int i;
};

struct test_auto_release_buffer_class
{
	test_auto_release_buffer_class(const Function<void()> func)
		: m_Function(func)
	{
	}

	~test_auto_release_buffer_class()
	{
		m_Function();
	}

	Function<void()> m_Function;
};

TEST(Utility_ControllableObject, InstanceTest)
{
	ControllableObject<int> test;
	test.Init(1);
	ASSERT_TRUE(test.IsInitialized());
	ASSERT_EQ(test.Get(), 1);
	test.Release();
	ASSERT_FALSE(test.IsInitialized());
}

TEST(Utility_ControllableObject, CopyTest)
{
	ControllableObject<test_controllable_object_class> test;
	test.Init();
	ASSERT_EQ(test.Get().i, 1);
	ControllableObject<test_controllable_object_class> test2(test);
	ASSERT_EQ(test2.Get().i, 2);
	ControllableObject<test_controllable_object_class> test3(std::move(test2));
	ASSERT_EQ(test3.Get().i, 3);
	ControllableObject<test_controllable_object_class> test4;
	test4.Init();
	test4 = test;
	ASSERT_EQ(test4.Get().i, 4);
	ControllableObject<test_controllable_object_class> test5;
	test5.Init();
	test5 = std::move(test);
	ASSERT_EQ(test5.Get().i, 5);
	test_controllable_object_class _test, _test2;
	ControllableObject<test_controllable_object_class> test6(_test);
	ASSERT_EQ(test6.Get().i, 2);
	ControllableObject<test_controllable_object_class> test7(std::move(_test));
	ASSERT_EQ(test7.Get().i, 3);
	ControllableObject<test_controllable_object_class> test8;
	test8.Init();
	test8 = _test2;
	ASSERT_EQ(test8.Get().i, 4);
	test8 = std::move(_test2);
	ASSERT_EQ(test8.Get().i, 5);

	ControllableObject<int, StdAllocator> test9(9);
	ControllableObject<int, MemoryManagerAllocator> test10(10);
	test10 = test9;
	ASSERT_EQ(test9, test10);
	ASSERT_EQ(test10.Get(), 9);
}

TEST(Utility_ControllableObject, ComparasionTest)
{
	ControllableObject<int> test(1), test2(2), test3(1);
	ASSERT_FALSE(test == test2);
	ASSERT_TRUE(test == test3);
	ASSERT_FALSE(test == 2);
	ASSERT_TRUE(test2 == 2);
	ControllableObject<int> test4, test5;
	ASSERT_FALSE(test4 == test);
	ASSERT_TRUE(test4 == test5);
	ASSERT_FALSE(test4 == 0);
}

TEST(Utility_AutoReleaseBuffer, InstanceAndNewObjectTest)
{
	AutoReleaseBuffer* arbuf = DefaultAllocator::New<AutoReleaseBuffer>();
	int* pi = arbuf->NewObject<int>();
	*pi = 10;
	ASSERT_EQ(*pi, 10);
	int flag = 0;
	test_auto_release_buffer_class* pc = arbuf->NewObject<test_auto_release_buffer_class>(
		[&]() {
			flag = 1;
		});
	ASSERT_EQ(flag, 0);
	DefaultAllocator::Delete(arbuf);
	ASSERT_EQ(flag, 1);
}