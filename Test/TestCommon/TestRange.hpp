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