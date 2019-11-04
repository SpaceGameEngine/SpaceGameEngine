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
#include <type_traits>
#include "gtest/gtest.h"
#include "Container/Vector.hpp"

using namespace SpaceGameEngine;

TEST(Vector, InstanceTest)
{
	Vector<int> test;
	ASSERT_TRUE((std::is_same_v<decltype(test)::ValueType, int>));
	ASSERT_TRUE((std::is_same_v<decltype(test)::AllocatorType, DefaultAllocator>));
	ASSERT_TRUE(test.GetRealSize() == 4);
	ASSERT_TRUE(test.GetSize() == 0);
}

TEST(Vector, SetRealSizeTest)
{
	Vector<int> test;
	test.SetRealSize(2);
	ASSERT_TRUE(test.GetSize() == 0);
	ASSERT_TRUE(test.GetRealSize() == 2);
}

TEST(Vector, GetObjectTest)
{
	//todo
}

struct test_vector_class
{
	test_vector_class()
	{
	}
	test_vector_class(int i)
	{
		mi = 0;
	}
	test_vector_class(const test_vector_class&)
	{
		mi = 1;
		std::cout << "copy construction" << std::endl;
	}
	test_vector_class& operator=(const test_vector_class&)
	{
		mi = 2;
		std::cout << "copy assignment" << std::endl;
		return *this;
	}
	~test_vector_class()
	{
		std::cout << "destruction" << std::endl;
	}
	int mi;
};

TEST(StdVector, CopyTest)
{
	std::vector<test_vector_class> test = {0, 1, 2};
	std::vector<test_vector_class> test2 = {0, 0, 0};
	std::vector<test_vector_class> test3 = {1};
	test2 = test;
	test3 = test2;
}