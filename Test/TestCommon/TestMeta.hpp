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
#include "gtest/gtest.h"
#include "Meta/Trait.hpp"
#include "Meta/Concept.hpp"

using namespace SpaceGameEngine;

struct test_trivial
{
};

struct test_trivial_constructor
{
	test_trivial_constructor()
	{
	}
};

struct test_trivial_copy_constructor
{
	test_trivial_copy_constructor(test_trivial_copy_constructor& t)
	{
	}
};

struct test_trivial_move_constructor
{
	test_trivial_move_constructor(test_trivial_move_constructor&& t)
	{
	}
};

struct test_trivial_copy_operator
{
	test_trivial_copy_operator& operator=(const test_trivial_copy_operator& t)
	{
		return *this;
	}
};

struct test_trivial_move_operator
{
	test_trivial_move_operator& operator=(test_trivial_move_operator&& t)
	{
		return *this;
	}
};

struct test_trivial_destructor
{
	~test_trivial_destructor()
	{
	}
};

TEST(MetaTrait, RemoveCVRefTest)
{
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int&&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<int&&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<volatile int>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<volatile int&>, int>));
}

TEST(MetaTrait, TypeWrapperTest)
{
	ASSERT_TRUE((std::is_same_v<TypeWrapper<const int&>::Type, const int&>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int&>::Type, int&>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int>::Type, int>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int*>::Type, int*>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int&&>::Type, int&&>));
}

TEST(MetaConcept, IsWeakEqualityComparableTest)
{
	ASSERT_TRUE((IsWeakEqualityComparable<int, long>));
	ASSERT_FALSE((IsWeakEqualityComparable<std::string, float>));
	ASSERT_TRUE((IsWeakEqualityComparable<int, int>));
	ASSERT_TRUE((IsWeakEqualityComparable<double>));
}

TEST(MetaConcept, IsEqualityComparableTest)
{
	ASSERT_TRUE((IsEqualityComparable<int, long>));
	ASSERT_FALSE((IsEqualityComparable<std::string, float>));
	ASSERT_TRUE((IsEqualityComparable<int, int>));
	ASSERT_TRUE((IsEqualityComparable<double>));
}

TEST(MetaConcept, IsTotallyOrderedTest)
{
	ASSERT_TRUE((IsTotallyOrdered<int, long>));
	ASSERT_FALSE((IsTotallyOrdered<std::string, float>));
	ASSERT_TRUE((IsTotallyOrdered<int, int>));
	ASSERT_TRUE((IsTotallyOrdered<double>));
}

TEST(MetaConcept, IsTrivialTest)
{
	ASSERT_TRUE(IsTrivial<int>);
	ASSERT_TRUE(IsTrivial<test_trivial>);
	ASSERT_FALSE(IsTrivial<test_trivial_constructor>);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_constructor>);
	ASSERT_FALSE(IsTrivial<test_trivial_move_constructor>);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_operator>);
	ASSERT_FALSE(IsTrivial<test_trivial_move_operator>);
	ASSERT_FALSE(IsTrivial<test_trivial_destructor>);
}