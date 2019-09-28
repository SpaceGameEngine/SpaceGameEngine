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
#include "MetaData/MetaData.h"
#include "MetaData/MetaObject.hpp"

using namespace SpaceGameEngine;

struct test_metadata_class
{
	test_metadata_class()
	{
		i = 1;
	}
	~test_metadata_class()
	{
		i = 0;
	}
	test_metadata_class(const test_metadata_class& obj)
	{
		i = 2;
	}
	test_metadata_class(test_metadata_class&& obj)
	{
		i = 3;
	}

	test_metadata_class& operator = (const test_metadata_class& obj)
	{
		i = 4;
		return *this;
	}
	test_metadata_class& operator = (test_metadata_class&& obj)
	{
		i = 5;
		return *this;
	}

	int i;
};

bool operator == (const test_metadata_class& t1, const test_metadata_class& t2)
{
	return t1.i == t2.i;
}

TEST_CASE("Test MetaData", "[Common][MetaData]")
{
	SECTION("test GetMetaData")
	{
		decltype(auto) test = GetMetaData<int>();
		REQUIRE(strcmp(test.m_pName, typeid(int).name()) == 0);
		REQUIRE(test.m_Size == sizeof(int));
		REQUIRE(test.m_Alignment == alignof(int));
		REQUIRE(test.m_pDefaultConstructor != nullptr);
		REQUIRE(test.m_pCopyConstructor != nullptr);
		REQUIRE(test.m_pMoveConstructor != nullptr);
		REQUIRE(test.m_pCopyAssignment != nullptr);
		REQUIRE(test.m_pMoveAssignment != nullptr);
		REQUIRE(test.m_pDestructor != nullptr);
		REQUIRE(test.m_pComparison != nullptr);
	}
	SECTION("test metadata's functions")
	{
		decltype(auto) test = GetMetaData<test_metadata_class>();
		test_metadata_class t1, t2;
		t1.i = 10;
		test.m_pDefaultConstructor(&t1);
		REQUIRE(t1.i == 1);
		test.m_pCopyConstructor(&t1, &t2);
		REQUIRE(t1.i == 2);
		test.m_pMoveConstructor(&t1, &t2);
		REQUIRE(t1.i == 3);
		test.m_pCopyAssignment(&t1, &t2);
		REQUIRE(t1.i == 4);
		test.m_pMoveAssignment(&t1, &t2);;
		REQUIRE(t1.i == 5);
		t1.i = t2.i;
		REQUIRE(test.m_pComparison(&t1, &t2));
		t1.i = 10;
		REQUIRE(test.m_pComparison(&t1, &t2) == false);
	}
	SECTION("test equal operator")
	{
		MetaData test1 = GetMetaData<int>();
		MetaData test2 = test1;
		MetaData test3 = GetMetaData<char>();
		REQUIRE((test1 == test2) == true);
		REQUIRE((test1 == test3) == false);
	}
}

TEST_CASE("Test MetaObject", "[Common][MetaData]")
{
	MetaObject test(GetMetaData<test_metadata_class>());
	REQUIRE(test.Get<test_metadata_class>().i == 1);
	SECTION("test copy")
	{
		MetaObject test2(test);
		REQUIRE(test2.Get<test_metadata_class>().i == 2);
		MetaObject test3(std::move(test2));
		REQUIRE(test3.Get<test_metadata_class>().i == 3);
		test2 = test;
		REQUIRE(test2.Get<test_metadata_class>().i == 4);
		test2 = std::move(test3);
		REQUIRE(test2.Get<test_metadata_class>().i == 5);
		test_metadata_class _test;
		MetaObject test4(GetMetaData<test_metadata_class>(), _test);
		REQUIRE(test4.Get<test_metadata_class>().i == 2);
		MetaObject test5(GetMetaData<test_metadata_class>(), std::move(_test));
		REQUIRE(test5.Get<test_metadata_class>().i == 3);
	}
	SECTION("test getmetadata&getdata")
	{
		REQUIRE(test.GetMetaData() == GetMetaData<test_metadata_class>());
		int i1 = 1;
		REQUIRE(memcmp(test.GetData(), &i1, sizeof(i1)) == 0);
		REQUIRE(std::is_same_v<decltype(std::declval<MetaObject<>>().GetData()), void*>);
		REQUIRE(std::is_same_v<decltype(std::declval<const MetaObject<>>().GetData()), const void*>);
	}
	SECTION("test get")
	{
		REQUIRE(test.Get<test_metadata_class>().i == 1);
		REQUIRE(std::is_same_v<decltype(std::declval<MetaObject<>>().Get<int>()), int&>);
		REQUIRE(std::is_same_v<decltype(std::declval<const MetaObject<>>().Get<int>()), const int&>);
	}
	SECTION("test comparison")
	{
		MetaObject test2(GetMetaData<test_metadata_class>());
		REQUIRE(test == test2);
	}
}