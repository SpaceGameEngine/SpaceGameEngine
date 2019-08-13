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
#include "Utility.hpp"

using namespace SpaceGameEngine;

class test_cmp
{
	int i;
public:
	bool operator == (const test_cmp&)const = delete;
};
class test_cmp2
{
	int i;
};
bool operator == (const test_cmp2&, const test_cmp2&) = delete;

TEST_CASE("Test IsComparable", "[Common][Utility]")
{
	REQUIRE(IsComparable<int>::Value == true);
	REQUIRE(IsComparable<char>::Value == true);
	REQUIRE(IsComparable<test_cmp>::Value == false);
	REQUIRE(IsComparable<test_cmp2>::Value == false);
}

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

	test_controllable_object_class& operator = (const test_controllable_object_class& obj)
	{
		i = 4;
		return *this;
	}
	test_controllable_object_class& operator = (test_controllable_object_class&& obj)
	{
		i = 5;
		return *this;
	}

	int i;
};

TEST_CASE("Test ControllableObject", "[Common][Utility]")
{
	SECTION("test instance")
	{
		ControllableObject<int> test;
		test.Init(1);
		REQUIRE(test.IsInitialized());
		REQUIRE(test.Get() == 1);
		test.Release();
		REQUIRE(!test.IsInitialized());
	}
	SECTION("test copy")
	{
		ControllableObject<test_controllable_object_class> test;
		test.Init();
		REQUIRE(test.Get().i == 1);
		ControllableObject<test_controllable_object_class> test2(test);
		REQUIRE(test2.Get().i == 2);
		ControllableObject<test_controllable_object_class> test3(std::move(test2));
		REQUIRE(test3.Get().i == 3);
		ControllableObject<test_controllable_object_class> test4;
		test4.Init();
		test4 = test;
		REQUIRE(test4.Get().i == 4);
		ControllableObject<test_controllable_object_class> test5;
		test5.Init();
		test5 = std::move(test);
		REQUIRE(test5.Get().i == 5);
		test_controllable_object_class _test,_test2;
		ControllableObject<test_controllable_object_class> test6(_test);
		REQUIRE(test6.Get().i == 2);
		ControllableObject<test_controllable_object_class> test7(std::move(_test));
		REQUIRE(test7.Get().i == 3);
		ControllableObject<test_controllable_object_class> test8;
		test8.Init();
		test8 = _test2;
		REQUIRE(test8.Get().i == 4);
		test8 = std::move(_test2);
		REQUIRE(test8.Get().i == 5);

	}
	SECTION("test comparison")
	{
		ControllableObject<int> test(1), test2(2), test3(1);
		REQUIRE(!(test == test2));
		REQUIRE(test == test3);
		REQUIRE(!(test == 2));
		REQUIRE(test2 == 2);
		ControllableObject<int> test4, test5;
		REQUIRE(!(test4 == test));
		REQUIRE(test4 == test5);
		REQUIRE(!(test4 == 0));
	}
}
