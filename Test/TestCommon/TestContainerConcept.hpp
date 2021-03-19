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
#include <vector>
#include "Container/Vector.hpp"
#include <forward_list>

using namespace SpaceGameEngine;

TEST(ContainerConcept, GetIteratorValueTypeTest)
{
	ASSERT_TRUE((std::is_same_v<IteratorValueType<int*>, int>));
	ASSERT_TRUE((std::is_same_v<IteratorValueType<const int*>, const int>));
	ASSERT_TRUE((std::is_same_v<IteratorValueType<std::vector<int>::iterator>, int>));
	ASSERT_TRUE((std::is_same_v<IteratorValueType<std::vector<int>::const_iterator>, int>));
	ASSERT_TRUE((std::is_same_v<IteratorValueType<Vector<int>::Iterator>, int>));
	ASSERT_TRUE((std::is_same_v<IteratorValueType<Vector<int>::ConstIterator>, const int>));
}

TEST(ContainerConcept, IsBaseIteratorTest)
{
	ASSERT_TRUE((IsBaseIterator<int*>::Value));
	ASSERT_TRUE((IsBaseIterator<const int*>::Value));
	ASSERT_TRUE((IsBaseIterator<std::vector<int>::iterator>::Value));
	ASSERT_TRUE((IsBaseIterator<std::vector<int>::const_iterator>::Value));
	ASSERT_TRUE((IsBaseIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsBaseIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_FALSE((IsBaseIterator<int>::Value));
}

TEST(ContainerConcept, IsBidirectionalBaseIteratorTest)
{
	ASSERT_TRUE((IsBidirectionalBaseIterator<int*>::Value));
	ASSERT_TRUE((IsBidirectionalBaseIterator<const int*>::Value));
	ASSERT_TRUE((IsBidirectionalBaseIterator<std::vector<int>::iterator>::Value));
	ASSERT_TRUE((IsBidirectionalBaseIterator<std::vector<int>::const_iterator>::Value));
	ASSERT_TRUE((IsBidirectionalBaseIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsBidirectionalBaseIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_FALSE((IsBidirectionalBaseIterator<int>::Value));
	ASSERT_TRUE((IsBaseIterator<std::forward_list<int>::iterator>::Value));
	ASSERT_FALSE((IsBidirectionalBaseIterator<std::forward_list<int>::iterator>::Value));
}

TEST(ContainerConcept, IsSequentialIteratorTest)
{
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_TRUE((IsSequentialIterator<std::vector<int>::iterator>::Value));
	ASSERT_TRUE((IsSequentialIterator<std::vector<int>::const_iterator>::Value));
	ASSERT_FALSE((IsSequentialIterator<std::forward_list<int>::const_iterator>::Value));
	ASSERT_FALSE((IsSequentialIterator<int>::Value));
}

TEST(ContainerConcept, IsBidirectionalSequentialIteratorTest)
{
	ASSERT_TRUE((IsBidirectionalSequentialIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((IsBidirectionalSequentialIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_TRUE((IsBidirectionalSequentialIterator<std::vector<int>::iterator>::Value));
	ASSERT_TRUE((IsBidirectionalSequentialIterator<std::vector<int>::const_iterator>::Value));
	ASSERT_FALSE((IsBidirectionalSequentialIterator<std::forward_list<int>::const_iterator>::Value));
	ASSERT_FALSE((IsBidirectionalSequentialIterator<int>::Value));
}
