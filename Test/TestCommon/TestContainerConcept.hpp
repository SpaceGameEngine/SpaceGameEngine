﻿/*
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
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::Iterator, int>::Result));
	ASSERT_TRUE((IsSequentialIterator<Vector<int>::ConstIterator, int>::Result));
	ASSERT_FALSE((IsSequentialIterator<int, int>::Result));
}