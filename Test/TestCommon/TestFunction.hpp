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
	void operator ()()
	{}
};

struct test_func_class
{
	int test()
	{
		return 1;
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
		REQUIRE(IsCorrectFunction<functor, void()>::Value == true);
		REQUIRE(IsCorrectFunction<int, void()>::Value == false);
	}
	SECTION("test IsFunction")
	{
		Function<void()> func([]() {});
		REQUIRE(Function<void()>::IsFunction<decltype(func)>::Value);
		REQUIRE(!Function<void()>::IsFunction<int>::Value);
	}
	SECTION("test instance and copy")
	{
		auto lambda = [](int) {};
		Function<void(int)> func(lambda);
		REQUIRE((void(*)(int))lambda == (void(*)(int))func.Get<decltype(lambda)>());
		Function<void(int)> func2 = func;
		REQUIRE((void(*)(int))func2.Get<decltype(lambda)>() == (void(*)(int))func.Get<decltype(lambda)>());
		Function<void(int)> func3 ([](int) {});	//can support lambda
		Function<void(int)> func4 = [](int) {};	//^^^^^^^^^^^^^^^^^^
		Function<void(int)> func5 = &func_;		//use function pointer
		REQUIRE(func5.Get<decltype(&func_)>() == &func_);
		Function<int(test_func_class*)> func6 = &test_func_class::test;
		test_func_class tc;
		REQUIRE(func6(&tc) == tc.test());
		//Function<void()> func_err1 = functor();	//error example
		//Function<void(int)> func_err2 = 0;	//error example
	}
}