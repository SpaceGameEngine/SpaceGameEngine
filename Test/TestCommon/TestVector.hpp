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

struct test_vector_class
{
	test_vector_class()
	{
		content = 0;
		mi = 0;
	}
	test_vector_class(int i)
	{
		content = i;
		mi = 0;
	}
	test_vector_class(const test_vector_class& t)
	{
		content = t.content;
		mi = 1;
		// std::cout << "copy construction" << std::endl;
	}
	test_vector_class& operator=(const test_vector_class& t)
	{
		content = t.content;
		mi = 2;
		// std::cout << "copy assignment" << std::endl;
		return *this;
	}
	test_vector_class(test_vector_class&& t)
	{
		content = t.content;
		mi = 3;
		// std::cout << "move construction" << std::endl;
	}
	test_vector_class& operator=(test_vector_class&& t)
	{
		content = t.content;
		mi = 4;
		// std::cout << "move assignment" << std::endl;
		return *this;
	}
	~test_vector_class()
	{
		// std::cout << "destruction" << std::endl;
	}
	int mi;
	int content;
};

struct test_vector_class2
{
	test_vector_class2() = delete;
	test_vector_class2(int i)
	{
		content = i;
	}
	int content;
};

TEST(Vector, InstanceTest)
{
	Vector<int> test;
	ASSERT_TRUE((std::is_same_v<decltype(test)::ValueType, int>));
	ASSERT_TRUE((std::is_same_v<decltype(test)::AllocatorType, DefaultAllocator>));
	ASSERT_TRUE(test.GetRealSize() == 4);
	ASSERT_TRUE(test.GetSize() == 0);
}

TEST(Vector, InitializerListConstructionTest)
{
	Vector<int> test({1, 2, 3});
	ASSERT_TRUE(test.GetRealSize() == 6);
	ASSERT_TRUE(test.GetSize() == 3);
}

TEST(Vector, STLContainerConstructionTest)
{
	Vector<test_vector_class> test(std::vector<test_vector_class>({4, 5, 6}));
	ASSERT_TRUE(test.GetRealSize() == 6);
	ASSERT_TRUE(test.GetSize() == 3);
	for (SizeType i = 0; i < test.GetSize(); i++)
		ASSERT_TRUE(test.GetObject(i).mi == 3);

	std::vector<test_vector_class> stl_container({7, 8, 9});
	Vector<test_vector_class> test2(stl_container);
	ASSERT_TRUE(test2.GetRealSize() == 6);
	ASSERT_TRUE(test2.GetSize() == 3);
	for (SizeType i = 0; i < test2.GetSize(); i++)
		ASSERT_TRUE(test2.GetObject(i).mi == 1);
}

TEST(Vector, SizeConstructionTest)
{
	Vector<test_vector_class> test(3, test_vector_class(1));
	ASSERT_TRUE(test.GetRealSize() == 6);
	ASSERT_TRUE(test.GetSize() == 3);
	for (SizeType i = 0; i < test.GetSize(); i++)
	{
		ASSERT_TRUE(test.GetObject(i).mi == 1);
		ASSERT_TRUE(test.GetObject(i).content == 1);
	}

	Vector<test_vector_class2> test2(3, test_vector_class2(2));
	ASSERT_TRUE(test2.GetRealSize() == 6);
	ASSERT_TRUE(test2.GetSize() == 3);
	for (SizeType i = 0; i < test2.GetSize(); i++)
	{
		ASSERT_TRUE(test2.GetObject(i).content == 2);
	}
}

TEST(Vector, CopyConstructionTest)
{
	//todo
}

TEST(Vector, MoveConstructionTest)
{
	//todo
}

TEST(Vector, CopyAssignmentTest)
{
	//todo
}

TEST(Vector, MoveAssignmentTest)
{
	//todo
}

TEST(Vector, SetRealSizeTest)
{
	Vector<int> test;
	test.SetRealSize(2);
	ASSERT_TRUE(test.GetSize() == 0);
	ASSERT_TRUE(test.GetRealSize() == 2);
	//todo
}

TEST(Vector, GetObjectTest)
{
	//todo
}

TEST(StdVector, CopyTest)
{
	std::vector<test_vector_class> test = {0, 1, 2};
	std::vector<test_vector_class> test2 = {0, 0, 0};
	std::vector<test_vector_class> test3 = {1};
	test2 = test;
	test3 = test2;
}