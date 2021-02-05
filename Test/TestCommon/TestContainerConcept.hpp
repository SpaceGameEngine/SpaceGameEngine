/*
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
#include "gtest/gtest.h"
#include "Container/ContainerConcept.hpp"

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

TEST(ContainerConcept, IsEqualityComparableTest)
{
	ASSERT_TRUE((IsEqualityComparable<int, long>::Value));
	ASSERT_FALSE((IsEqualityComparable<std::string, float>::Value));
	ASSERT_TRUE((IsEqualityComparable<int, int>::Value));
	ASSERT_TRUE((IsEqualityComparable<double>::Value));
}

TEST(ContainerConcept, IsTotallyOrderedTest)
{
	ASSERT_TRUE((IsTotallyOrdered<int, long>::Value));
	ASSERT_FALSE((IsTotallyOrdered<std::string, float>::Value));
	ASSERT_TRUE((IsTotallyOrdered<int, int>::Value));
	ASSERT_TRUE((IsTotallyOrdered<double>::Value));
}

TEST(ContainerConcept, IsSequentialIteratorTest)
{
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_FALSE((IsSequentialIterator<int>::Value));
}

TEST(ContainerConcept, IsTrivialTest)
{
	ASSERT_TRUE(IsTrivial<int>::Value);
	ASSERT_TRUE(IsTrivial<test_trivial>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_constructor>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_constructor>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_move_constructor>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_operator>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_move_operator>::Value);
	ASSERT_FALSE(IsTrivial<test_trivial_destructor>::Value);
}

TEST(ContainerConcept, IsBidirectionalSequentialIteratorTest)
{
	ASSERT_TRUE((IsBidirectionalSequentialIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsBidirectionalSequentialIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_FALSE((IsBidirectionalSequentialIterator<int>::Value));
}
