/*
Copyright 2023 creatorlxd

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
#include "Function.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

void func_(int i)
{
}
void func_2(int i, int i2)
{
}
int func_3(int i, int i2)
{
	return 0;
}

struct functor
{
	int operator()()
	{
		return 1;
	}
};

struct test_func_class
{
	int test()
	{
		return 1;
	}
	int test2() const
	{
		return 2;
	}
};

TEST(Function, IsCorrectFunctionTest)
{
	ASSERT_TRUE((IsCorrectFunction<decltype(func_), void(int)>::Value));
	ASSERT_FALSE((IsCorrectFunction<decltype(func_2), void(int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(func_2), void(int, int)>::Value));
	ASSERT_FALSE((IsCorrectFunction<decltype(func_3), void(int, int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(func_3), int(int, int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<functor, int(void)>::Value));
	ASSERT_FALSE((IsCorrectFunction<int, void()>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(&test_func_class::test), int(test_func_class*)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(&test_func_class::test2), int(const test_func_class*)>::Value));
}
TEST(Function, IsFunctionTest)
{
	Function<void()> func([]() {});
	ASSERT_TRUE(Function<void()>::IsFunction<decltype(func)>::Value);
	ASSERT_TRUE(!Function<void()>::IsFunction<int>::Value);
}
TEST(Function, ConstructionTest)
{
	auto lambda = [](void) -> int { return 1; };
	Function<int(void)> func(lambda);
	ASSERT_TRUE((int (*)(void))lambda == (int (*)(void))func.Get<decltype(lambda)>());
	ASSERT_TRUE(lambda() == func());
	Function<int(void)> func2 = func;
	ASSERT_TRUE((int (*)(void))func2.Get<decltype(lambda)>() ==
				(int (*)(void))func.Get<decltype(lambda)>());
	Function<int(void)> func3([]() -> int { return 2; });
	func3 = func2;
	ASSERT_TRUE(func3() == func2());
	Function<void(int)> func5 = &func_;	   // use function pointer
	ASSERT_TRUE(func5.Get<decltype(&func_)>() == &func_);
	Function<int(test_func_class*)> func6 = &test_func_class::test;
	test_func_class tc;
	ASSERT_TRUE(func6(&tc) == tc.test());
	Function<int(void)> func7 = functor();
	ASSERT_TRUE(func7() == functor()());	// use functor
	Function<int(int)> func8 = [](int i) { return i; };
	ASSERT_TRUE(func8(1) == 1);
}
TEST(Function, MetaDataTest)
{
	Function<void(int)> func(&func_);
	ASSERT_TRUE(func.GetMetaData() == GetMetaData<decltype(&func_)>());
}
TEST(Function, ComparisionTest)
{
	Function<void(int)> func(&func_);
	Function<void(int)> func2 = func;
	ASSERT_EQ(func, func2);
}
TEST(Function, CopyTest)
{
	Function<void(int)> func(&func_);
	Function<void(int), MemoryManagerAllocator> func2([](int) -> void {});
	Function<void(int), StdAllocator> func3([](int) -> void {});
	func2 = func;
	func3 = func2;
	ASSERT_EQ(func, func2);
	ASSERT_EQ(func2, func3);
}