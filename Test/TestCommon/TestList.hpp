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
#include "Container/List.hpp"

using namespace SpaceGameEngine;

TEST(List, InstanceTest)
{
	List<int> l;
	ASSERT_TRUE((std::is_same_v<decltype(l)::ValueType, int>));
	ASSERT_TRUE((std::is_same_v<decltype(l)::AllocatorType, DefaultAllocator>));
	ASSERT_EQ(l.GetSize(), 0);
}

TEST(List, ClearTest)
{
	List<int> l;	//todo : add more content
	ASSERT_EQ(l.GetSize(), 0);
	l.Clear();
	ASSERT_EQ(l.GetSize(), 0);
}

TEST(ListIterator, OutOfRangeErrorTest)
{
	//todo : make list more content, add more test
	List<int> l;
	ASSERT_TRUE(List<int>::Iterator::OutOfRangeError::Judge(l.GetEnd()));
	ASSERT_TRUE(List<int>::ConstIterator::OutOfRangeError::Judge(l.GetConstEnd()));
	ASSERT_TRUE(List<int>::ReverseIterator::OutOfRangeError::Judge(l.GetReverseEnd()));
	ASSERT_TRUE(List<int>::ConstReverseIterator::OutOfRangeError::Judge(l.GetConstReverseEnd()));
}

TEST(ListIterator, GetTest)
{
	//todo : make list more content
	List<int> l;

	auto iter1 = l.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<int>::Iterator>));
	ASSERT_EQ(iter1, l.GetEnd());

	auto iter2 = l.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<int>::Iterator>));
	ASSERT_EQ(iter2, l.GetEnd());

	auto riter1 = l.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(riter1), List<int>::ReverseIterator>));
	ASSERT_EQ(riter1, l.GetReverseEnd());

	auto riter2 = l.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(riter2), List<int>::ReverseIterator>));
	ASSERT_EQ(riter2, l.GetReverseEnd());
}

TEST(ListIterator, GetConstTest)
{
	//todo : make list more content
	const List<int> l;

	auto iter1 = l.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<int>::ConstIterator>));
	ASSERT_EQ(iter1, l.GetConstEnd());

	auto iter2 = l.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<int>::ConstIterator>));
	ASSERT_EQ(iter2, l.GetConstEnd());

	auto riter1 = l.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(riter1), List<int>::ConstReverseIterator>));
	ASSERT_EQ(riter1, l.GetConstReverseEnd());

	auto riter2 = l.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(riter2), List<int>::ConstReverseIterator>));
	ASSERT_EQ(riter2, l.GetConstReverseEnd());
}