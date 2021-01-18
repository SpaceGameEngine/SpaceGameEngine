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
#include "Container/Range/RangeCore.hpp"
#include "Container/Vector.hpp"
#include <vector>
#include "Container/Range/TransformCore.hpp"
#include "Container/Range/Cast.hpp"
#include "Container/Range/FilterTransform.hpp"
#include "Container/Range/Sequence.hpp"
#include "Container/Range/TakeTransform.hpp"

using namespace SpaceGameEngine;

TEST(RangeCore, InstanceTest)
{
	Vector<int> v1 = {0, 1, 2, 3, 4, 5};
	Range r1(v1.GetBegin(), v1.GetEnd());
}

TEST(RangeCore, GetBeginAndGetEndTest)
{
	Vector<int> v1 = {0, 1, 2, 3, 4, 5};
	Range r1(v1.GetBegin(), v1.GetEnd());
	ASSERT_EQ(r1.GetEnd() - r1.GetBegin(), v1.GetSize());
	for (auto i1 = r1.GetBegin(); r1.GetEnd() != i1; i1 += 1)
	{
		ASSERT_EQ(*i1, i1 - r1.GetBegin());
	}
}

TEST(Cast, CastToVectorTest)
{
	Vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Transform t(Range(v.GetBegin(), v.GetEnd()));
	Vector<int> v2 = CastToVector(t);
	ASSERT_EQ(v.GetSize(), v2.GetSize());
	for (int i = 0; i < v.GetSize(); i++)
		ASSERT_EQ(v[i], v2[i]);
}

TEST(FilterTransform, MakeFilterTransformTest)
{
	Vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Transform t = MakeFilterTransform(Transform(Range(v.GetBegin(), v.GetEnd())), [](const int& i) { return i % 2 == 0; });
	Vector<int> v2 = CastToVector(t);
	Vector<int> cv1 = {0, 2, 4, 6, 8};
	ASSERT_EQ(v2.GetSize(), cv1.GetSize());
	for (int i = 0; i < v2.GetSize(); i++)
		ASSERT_EQ(v2[i], cv1[i]);

	Transform t2 = MakeFilterTransform<decltype(v.GetBegin()), decltype(v.GetEnd()), StdAllocator>(Transform(Range(v.GetBegin(), v.GetEnd())), [](const int& i) { return i % 2 == 0; });
	Vector<int> v3 = CastToVector(t2);
	ASSERT_EQ(v3.GetSize(), cv1.GetSize());
	for (int i = 0; i < v3.GetSize(); i++)
		ASSERT_EQ(v3[i], cv1[i]);
}

TEST(FilterTransform, FilterTransformTest)
{
	Vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Transform t = Transform(Range(v.GetBegin(), v.GetEnd())) | FilterTransform<int>([](const int& i) { return i % 2 == 0; });
	Vector<int> v2 = CastToVector(t);
	Vector<int> cv1 = {0, 2, 4, 6, 8};
	ASSERT_EQ(v2.GetSize(), cv1.GetSize());
	for (int i = 0; i < v2.GetSize(); i++)
		ASSERT_EQ(v2[i], cv1[i]);

	Transform t2 = Transform(Range(v.GetBegin(), v.GetEnd())) | FilterTransform<int, StdAllocator>([](const int& i) { return i % 2 == 0; });
	Vector<int> v3 = CastToVector(t2);
	ASSERT_EQ(v3.GetSize(), cv1.GetSize());
	for (int i = 0; i < v3.GetSize(); i++)
		ASSERT_EQ(v3[i], cv1[i]);
}

TEST(SequenceIterator, IsRangeIteratorTest)
{
	ASSERT_TRUE((IsRangeIterator<SequenceIterator<int, int>>::Result));
	ASSERT_TRUE((IsRangeIterator<SequenceIterator<int, char>>::Result));
	ASSERT_TRUE((IsRangeIterator<SequenceIterator<float, int>>::Result));
}

TEST(SequenceIterator, RangeConceptTest)
{
	ASSERT_TRUE((IsRangeBidirectionalIterator<SequenceIterator<int, int>>::Result));
}

TEST(SequenceIterator, MakeInfiniteSequenceTest)
{
	Range r = MakeInfiniteSequence(0, 1);
	int cot = 0;
	for (auto i = r.GetBegin(); r.GetEnd() != i && cot < 10; i += 1, cot += 1)
		ASSERT_EQ(*i, cot);
}

TEST(SequenceIterator, MakeSequenceTest)
{
	Range r = MakeSequence(0, 1, 10);
	Vector v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto iter = v.GetBegin();
	for (auto i = r.GetBegin(); r.GetEnd() != i; i += 1, iter += 1)
		ASSERT_EQ(*i, *iter);
	ASSERT_EQ(iter, v.GetEnd());
}

TEST(TakeTransform, MakeTakeTransformTest)
{
	Vector<int> v1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	Vector<int> rv1 = CastToVector(MakeTakeTransform(Transform(Range(v1.GetBegin(), v1.GetEnd())), 5));
	ASSERT_EQ(rv1.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv1[i]);

	Transform t1 = MakeTakeTransform(Transform(MakeSequence(0, 1, 10)), 5);
	Vector<int, StdAllocator> rv2 = CastToVector<Vector<int>::Iterator, Vector<int>::Iterator, StdAllocator>(t1);
	ASSERT_EQ(rv2.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv2[i]);

	Transform t2 = MakeTakeTransform<SequenceIterator<int, int>, InfiniteSentinel, StdAllocator>(Transform(MakeInfiniteSequence(1, 1)), 5);
	Vector<int> rv3 = CastToVector(t2);
	ASSERT_EQ(rv3.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv3[i] - 1);
}

TEST(TakeTransform, TakeTransformTest)
{
	Vector<int> v1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	Vector<int> rv1 = CastToVector(Transform(Range(v1.GetBegin(), v1.GetEnd())) | TakeTransform<int>(5));
	ASSERT_EQ(rv1.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv1[i]);

	Transform t1 = Transform(MakeSequence(0, 1, 10)) | TakeTransform(5);
	Vector<int, StdAllocator> rv2 = CastToVector<Vector<int>::Iterator, Vector<int>::Iterator, StdAllocator>(t1);
	ASSERT_EQ(rv2.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv2[i]);

	Transform t2 = Transform(MakeInfiniteSequence(1, 1)) | TakeTransform<StdAllocator>(5);
	Vector<int> rv3 = CastToVector(t2);
	ASSERT_EQ(rv3.GetSize(), 5);
	for (int i = 0; i < 5; i++)
		ASSERT_EQ(v1[i], rv3[i] - 1);
}