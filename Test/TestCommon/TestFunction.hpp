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
#include "Function.hpp"

using namespace SpaceGameEngine;

void func_(int i)
{}
void func_2(int i, int i2)
{}
int func_3(int i, int i2)
{
	return 0;
}

struct functor
{
	int operator ()()
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
	int test2()const
	{
		return 2;
	}
};

TEST_CASE("Test Function", "[Common][Function]")
{
	SECTION("test IsCorrectFunction")
	{
		REQUIRE(IsCorrectFunction<decltype(func_), void(int)>::Value == true);
		REQUIRE(IsCorrectFunction<decltype(func_2), void(int)>::Value == false);
		REQUIRE(IsCorrectFunction<decltype(func_2), void(int,int)>::Value == true);
		REQUIRE(IsCorrectFunction<decltype(func_3), void(int,int)>::Value == false);
		REQUIRE(IsCorrectFunction<decltype(func_3), int(int,int)>::Value == true);
		REQUIRE(IsCorrectFunction<functor, int(void)>::Value == true);
		REQUIRE(IsCorrectFunction<int, void()>::Value == false);
		REQUIRE(IsCorrectFunction<decltype(&test_func_class::test), int(test_func_class*)>::Value == true);
		REQUIRE(IsCorrectFunction<decltype(&test_func_class::test2), int(const test_func_class*)>::Value == true);
	}
	SECTION("test IsFunction")
	{
		Function<void()> func([]() {});
		REQUIRE(Function<void()>::IsFunction<decltype(func)>::Value);
		REQUIRE(!Function<void()>::IsFunction<int>::Value);
	}
	SECTION("test instance&copy&invoke")
	{
		auto lambda = [](void)->int {return 1; };
		Function<int(void)> func(lambda);
		REQUIRE((int(*)(void))lambda == (int(*)(void))func.Get<decltype(lambda)>());
		REQUIRE(lambda() == func());
		Function<int(void)> func2 = func;
		REQUIRE((int(*)(void))func2.Get<decltype(lambda)>() == (int(*)(void))func.Get<decltype(lambda)>());
		Function<int(void)> func3([]()->int {return 2; });
		func3 = func2;
		REQUIRE(func3() == func2());
		Function<void(int)> func5 = &func_;		//use function pointer
		REQUIRE(func5.Get<decltype(&func_)>() == &func_);
		Function<int(test_func_class*)> func6 = &test_func_class::test;	//use member function
		test_func_class tc;
		REQUIRE(func6(&tc) == tc.test());
		Function<int(void)> func7 = functor();	//use functor
		REQUIRE(func7() == functor()());
		Function<int(const test_func_class*)> func8 = &test_func_class::test2;	//use const member function
		REQUIRE(func8(&tc) == tc.test2());
	}
	SECTION("test get metadata")
	{
		Function<void(int)> func(&func_);
		REQUIRE(func.GetMetaData() == GetMetaData<decltype(&func_)>());
	}
	SECTION("test comparison")
	{
		Function<void(int)> func(&func_);
		Function<void(int)> func2 = func;
		REQUIRE(func == func2);
	}
}