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
		mi = -1;
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
	Vector<test_vector_class, MemoryManagerAllocator> v1{0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v2 = v1;
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == v1.GetObject(i).content);
		ASSERT_TRUE(v2.GetObject(i).mi == 1);
	}

	Vector<test_vector_class, StdAllocator> v3 = v1;
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == v1.GetObject(i).content);
		ASSERT_TRUE(v3.GetObject(i).mi == 1);
	}
}

TEST(Vector, MoveConstructionTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1_1{0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v1_2{0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v2 = std::move(v1_1);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == i);
		ASSERT_TRUE(v2.GetObject(i).mi == 1);
	}

	Vector<test_vector_class, StdAllocator> v3 = std::move(v1_2);
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == i);
		ASSERT_TRUE(v3.GetObject(i).mi == 3);
	}
}

TEST(Vector, CopyAssignmentTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1 = {0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v2 = {0};
	v2 = v1;
	ASSERT_TRUE(v2.GetSize() == v1.GetSize());
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == v1.GetObject(i).content);
	}

	Vector<test_vector_class, StdAllocator> v3 = {0};
	v3 = v1;
	ASSERT_TRUE(v3.GetSize() == v1.GetSize());
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == v1.GetObject(i).content);
	}
}

TEST(Vector, MoveAssignmentTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1_1 = {0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v1_2 = {0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v2 = {0};
	v2 = std::move(v1_1);
	ASSERT_TRUE(v2.GetSize() == 3);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == i);
	}

	Vector<test_vector_class, StdAllocator> v3 = {0};
	v3 = std::move(v1_2);
	ASSERT_TRUE(v3.GetSize() == 3);
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == i);
	}
}

TEST(Vector, SetRealSizeTest)
{
	Vector<int> test = {0, 1};
	ASSERT_TRUE(test.GetObject(0) == 0);
	ASSERT_TRUE(test.GetObject(1) == 1);
	ASSERT_TRUE(test.GetSize() == 2);
	ASSERT_TRUE(test.GetRealSize() == 4);
	test.SetRealSize(2);
	ASSERT_TRUE(test.GetSize() == 2);
	ASSERT_TRUE(test.GetRealSize() == 2);
	ASSERT_TRUE(test.GetObject(0) == 0);
	ASSERT_TRUE(test.GetObject(1) == 1);
}

TEST(Vector, GetObjectTest)
{
	Vector<int> v = {0, 1, 2};
	ASSERT_TRUE(v.GetObject(1) == 1);
	v.GetObject(1) = 3;
	ASSERT_TRUE(v.GetObject(1) == 3);
}

TEST(Vector, GetDataTest)
{
	Vector<int> v1 = {0, 1, 2};
	const Vector<int> v2 = {0, 1, 2};
	ASSERT_TRUE(v1.GetData() == &v1.GetObject(0));
	ASSERT_TRUE(*v1.GetData() == 0);
	ASSERT_TRUE(v2.GetData() == &v2.GetObject(0));
	ASSERT_TRUE(*v2.GetData() == 0);
}

TEST(Vector, ClearTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	ASSERT_TRUE(v.GetSize() == 3);
	ASSERT_TRUE(v.GetRealSize() == 6);
	v.Clear();
	ASSERT_TRUE(v.GetSize() == 0);
	ASSERT_TRUE(v.GetRealSize() == 4);
	ASSERT_TRUE(v.GetData() != nullptr);
}

TEST(Vector, SetSizeTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	ASSERT_TRUE(v.GetSize() == 3);
	ASSERT_TRUE(v.GetRealSize() == 6);
	v.SetSize(2, test_vector_class());
	ASSERT_TRUE(v.GetSize() == 2);
	ASSERT_TRUE(v.GetRealSize() == 6);
#ifdef SGE_DEBUG	//destructor action may be optimized in release mode
	ASSERT_TRUE((v.GetData() + 2)->mi == -1);
#endif
	v.SetSize(6, test_vector_class(1));
	ASSERT_TRUE(v.GetSize() == 6);
	ASSERT_TRUE(v.GetRealSize() == 6);
	ASSERT_TRUE((v.GetBegin() + 5)->content == 1);
	v.SetSize(8, test_vector_class());
	ASSERT_TRUE(v.GetSize() == 8);
	ASSERT_TRUE(v.GetRealSize() == 16);
	ASSERT_TRUE(v.GetBegin()->mi == 3);
	ASSERT_TRUE((v.GetBegin() + 6)->mi == 1);
}

TEST(Vector, GetObjectByOperatorTest)
{
	Vector<int> v = {0, 1, 2};
	const Vector<int> cv = {0, 1, 2};
	ASSERT_TRUE(v[1] == 1);
	ASSERT_TRUE(cv[1] == 1);
	v[2] = 0;
	ASSERT_TRUE(v.GetObject(2) == 0);
}

TEST(VectorIterator, GetBeginTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetBegin();
	ASSERT_TRUE(*iter1 == 0);
}

TEST(VectorIterator, GetEndTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetEnd();
	ASSERT_TRUE(*(iter1 - 1) == 2);
}

TEST(VectorIterator, CopyTest)
{
	Vector<int> v = {0, 1, 2};
	auto iter1 = v.GetBegin();
	auto iter2 = iter1;
	auto iter3 = v.GetBegin() + 1;
	ASSERT_TRUE(*iter1 == *iter2 && *iter1 == 0);
	ASSERT_TRUE(*iter3 == 1);
	iter3 = iter2;
	ASSERT_TRUE(*iter3 == *iter2 && *iter2 == 0);
}

TEST(VectorIterator, CalculationOperatorTest)
{
	Vector<int> v = {0, 1, 2, 3, 4};
	for (auto i = v.GetBegin(); i != v.GetBegin() + v.GetSize(); i += 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetBegin()));
	}
	for (auto i = v.GetBegin(); i != v.GetBegin() + v.GetSize(); i = i + 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetBegin()));
	}
	for (auto i = v.GetBegin() + v.GetSize() - 1; (i == v.GetBegin() - 1) == false; i -= 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetBegin()));
	}
	for (auto i = v.GetBegin() + v.GetSize() - 1; (i == v.GetBegin() - 1) == false; i = i - 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetBegin()));
	}
}

TEST(VectorIterator, GetTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	auto iter = v.GetBegin();
	ASSERT_TRUE((*iter).content == 0);
	ASSERT_TRUE(iter->content == 0);
}

TEST(VectorIterator, OutOfRangeErrorTest)
{
	Vector<int> test = {0, 1, 2};
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin(), test.GetData(), test.GetData() + test.GetSize()) == false));
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin() - 1, test.GetData(), test.GetData() + test.GetSize()) == true));
}