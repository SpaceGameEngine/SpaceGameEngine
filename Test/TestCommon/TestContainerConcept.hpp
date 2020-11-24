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
	ASSERT_TRUE((IsEqualityComparable<int, long>::Result));
	ASSERT_FALSE((IsEqualityComparable<std::string, float>::Result));
	ASSERT_TRUE((IsEqualityComparable<int, int>::Result));
	ASSERT_TRUE((IsEqualityComparable<double>::Result));
}

TEST(ContainerConcept, IsTotallyOrderedTest)
{
	ASSERT_TRUE((IsTotallyOrdered<int, long>::Result));
	ASSERT_FALSE((IsTotallyOrdered<std::string, float>::Result));
	ASSERT_TRUE((IsTotallyOrdered<int, int>::Result));
	ASSERT_TRUE((IsTotallyOrdered<double>::Result));
}

TEST(ContainerConcept, IsSequentialIteratorTest)
{
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::Iterator>::Result));
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::ConstIterator>::Result));
	ASSERT_FALSE((IsSequentialIterator<int>::Result));
}

TEST(ContainerConcept, IsTrivialTest)
{
	ASSERT_TRUE(IsTrivial<int>::Result);
	ASSERT_TRUE(IsTrivial<test_trivial>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_constructor>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_constructor>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_move_constructor>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_copy_operator>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_move_operator>::Result);
	ASSERT_FALSE(IsTrivial<test_trivial_destructor>::Result);
}

TEST(ContainerConcept, IsBidirectionalIteratorTest)
{
	ASSERT_TRUE((IsBidirectionalIterator<Vector<int>::Iterator>::Result));
	ASSERT_TRUE((IsBidirectionalIterator<Vector<int>::ConstIterator>::Result));
	ASSERT_FALSE((IsBidirectionalIterator<int>::Result));
}

TEST(ContainerConcept, IsRangeTest)
{
	ASSERT_TRUE((IsRange<Vector<int>>::Result));
	ASSERT_FALSE((IsRange<int>::Result));
}

TEST(ContainerConcept, IsRangeIteratorTest)
{
	ASSERT_TRUE((IsRangeIterator<Vector<int>::Iterator>::Result));
	ASSERT_TRUE((IsRangeIterator<Vector<int>::ConstIterator>::Result));
	ASSERT_FALSE((IsRangeIterator<int>::Result));
}

TEST(ContainerConcept, IsRangeSentinelTest)
{
	ASSERT_TRUE((IsRangeSentinel<Vector<int>::Iterator, Vector<int>::Iterator>::Result));
	ASSERT_TRUE((IsRangeSentinel<Vector<int>::ConstIterator, Vector<int>::ConstIterator>::Result));
	ASSERT_FALSE((IsRangeSentinel<Vector<int>::Iterator, int>::Result));
}