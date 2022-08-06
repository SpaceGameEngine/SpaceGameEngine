/*
Copyright 2022 creatorlxd

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
#include <functional>
#include "gtest/gtest.h"
#include "Container/Vector.hpp"

using namespace SpaceGameEngine;

struct test_vector_class
{
	test_vector_class()
		: content(0), mi(0), destruction_hook([]() {})
	{
	}
	test_vector_class(int i)
		: content(i), mi(0), destruction_hook([]() {})
	{
	}
	test_vector_class(int i, const std::function<void()>& func)
		: content(i), mi(0), destruction_hook(func)
	{
	}
	test_vector_class(const test_vector_class& t)
		: content(t.content), mi(1), destruction_hook(t.destruction_hook)
	{
	}
	test_vector_class& operator=(const test_vector_class& t)
	{
		content = t.content;
		mi = 2;
		destruction_hook = t.destruction_hook;
		return *this;
	}
	test_vector_class(test_vector_class&& t)
		: content(t.content), mi(3), destruction_hook(std::move(t.destruction_hook))
	{
		t.destruction_hook = []() {};
	}
	test_vector_class& operator=(test_vector_class&& t)
	{
		content = t.content;
		mi = 4;
		destruction_hook = std::move(t.destruction_hook);
		t.destruction_hook = []() {};
		return *this;
	}
	~test_vector_class()
	{
		mi = -1;
		destruction_hook();
	}
	int mi;
	int content;
	std::function<void()> destruction_hook;
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
	ASSERT_EQ(test[0], 1);
	ASSERT_EQ(test[1], 2);
	ASSERT_EQ(test[2], 3);
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

TEST(Vector, IteratorPairConstructionTest)
{
	Vector<int> v1 = {2, 3, 5, 7, 9};
	Vector<int, StdAllocator> v2(v1.GetReverseBegin(), v1.GetReverseBegin() + 3);
	ASSERT_EQ(v2.GetSize(), 3);
	for (int i = 0; i < 3; i++)
		ASSERT_EQ(v1[4 - i], v2[i]);
}

TEST(Vector, CopyConstructionTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1{0, 1, 2};
	ASSERT_EQ(v1.GetSize(), 3);
	Vector<test_vector_class, MemoryManagerAllocator> v2 = v1;
	ASSERT_EQ(v2.GetSize(), v1.GetSize());
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == v1.GetObject(i).content);
		ASSERT_TRUE(v2.GetObject(i).mi == 1);
	}

	Vector<test_vector_class, StdAllocator> v3 = v1;
	ASSERT_EQ(v3.GetSize(), v1.GetSize());
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == v1.GetObject(i).content);
		ASSERT_TRUE(v3.GetObject(i).mi == 1);
	}

	Vector<int, StdAllocator> v4 = {0, 1, 2};
	ASSERT_EQ(v4.GetSize(), 3);
	ASSERT_EQ(v4[0], 0);
	ASSERT_EQ(v4[1], 1);
	ASSERT_EQ(v4[2], 2);

	Vector<int, StdAllocator> v5 = v4;
	ASSERT_EQ(v5.GetSize(), v4.GetSize());
	for (int i = 0; i < v5.GetSize(); i++)
		ASSERT_EQ(v4[i], v5[i]);

	Vector<int, MemoryManagerAllocator> v6 = v4;
	ASSERT_EQ(v6.GetSize(), v4.GetSize());
	for (int i = 0; i < v6.GetSize(); i++)
		ASSERT_EQ(v4[i], v6[i]);
}

TEST(Vector, MoveConstructionTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1_1{0, 1, 2};
	Vector<test_vector_class, MemoryManagerAllocator> v1_2{0, 1, 2};
	ASSERT_EQ(v1_1.GetSize(), 3);
	ASSERT_EQ(v1_2.GetSize(), 3);
	Vector<test_vector_class, MemoryManagerAllocator> v2 = std::move(v1_1);
	ASSERT_EQ(v2.GetSize(), 3);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_TRUE(v2.GetObject(i).content == i);
		ASSERT_TRUE(v2.GetObject(i).mi == 1);
	}

	Vector<test_vector_class, StdAllocator> v3 = std::move(v1_2);
	ASSERT_EQ(v3.GetSize(), 3);
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_TRUE(v3.GetObject(i).content == i);
		ASSERT_TRUE(v3.GetObject(i).mi == 3);
	}

	Vector<int, StdAllocator> v4_1 = {0, 1, 2};
	ASSERT_EQ(v4_1.GetSize(), 3);
	Vector<int, StdAllocator> v4_2 = {0, 1, 2};
	ASSERT_EQ(v4_2.GetSize(), 3);

	Vector<int, StdAllocator> v5(std::move(v4_1));
	ASSERT_EQ(v5.GetSize(), 3);
	for (int i = 0; i < v5.GetSize(); i++)
		ASSERT_EQ(v5[i], i);

	Vector<int, MemoryManagerAllocator> v6(std::move(v4_2));
	ASSERT_EQ(v6.GetSize(), 3);
	for (int i = 0; i < v6.GetSize(); i++)
		ASSERT_EQ(v6[i], i);
}

TEST(Vector, CopyAssignmentTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1 = {0, 1, 2};
	ASSERT_EQ(v1.GetSize(), 3);
	ASSERT_EQ(v1.GetRealSize(), 6);
	Vector<test_vector_class, MemoryManagerAllocator> v1_1 = {0};
	ASSERT_EQ(v1_1.GetSize(), 1);
	ASSERT_EQ(v1_1.GetRealSize(), 2);
	Vector<test_vector_class, MemoryManagerAllocator> v1_2 = {1, 0};
	ASSERT_EQ(v1_2.GetSize(), 2);
	ASSERT_EQ(v1_2.GetRealSize(), 4);
	Vector<test_vector_class, MemoryManagerAllocator> v1_3 = {3, 2, 1, 0};
	ASSERT_EQ(v1_3.GetSize(), 4);
	ASSERT_EQ(v1_3.GetRealSize(), 8);
	v1_1 = v1;
	ASSERT_EQ(v1_1.GetSize(), v1.GetSize());
	ASSERT_EQ(v1_1.GetRealSize(), v1.GetRealSize());
	for (SizeType i = 0; i < v1_1.GetSize(); i++)
	{
		ASSERT_TRUE(v1_1.GetObject(i).content == v1.GetObject(i).content);
	}
	v1_2 = v1;
	ASSERT_EQ(v1_2.GetSize(), v1.GetSize());
	ASSERT_EQ(v1_2.GetRealSize(), 4);
	for (SizeType i = 0; i < v1_2.GetSize(); i++)
	{
		ASSERT_TRUE(v1_2.GetObject(i).content == v1.GetObject(i).content);
	}
	v1_3 = v1;
	ASSERT_EQ(v1_3.GetSize(), v1.GetSize());
	ASSERT_EQ(v1_3.GetRealSize(), 8);
	for (SizeType i = 0; i < v1_3.GetSize(); i++)
	{
		ASSERT_TRUE(v1_3.GetObject(i).content == v1.GetObject(i).content);
	}

	Vector<test_vector_class, StdAllocator> v2_1 = {0};
	ASSERT_EQ(v2_1.GetSize(), 1);
	ASSERT_EQ(v2_1.GetRealSize(), 2);
	Vector<test_vector_class, StdAllocator> v2_2 = {1, 0};
	ASSERT_EQ(v2_2.GetSize(), 2);
	ASSERT_EQ(v2_2.GetRealSize(), 4);
	Vector<test_vector_class, StdAllocator> v2_3 = {3, 2, 1, 0};
	ASSERT_EQ(v2_3.GetSize(), 4);
	ASSERT_EQ(v2_3.GetRealSize(), 8);
	v2_1 = v1;
	ASSERT_EQ(v2_1.GetSize(), v1.GetSize());
	ASSERT_EQ(v2_1.GetRealSize(), v1.GetRealSize());
	for (SizeType i = 0; i < v2_1.GetSize(); i++)
	{
		ASSERT_TRUE(v2_1.GetObject(i).content == v1.GetObject(i).content);
	}
	v2_2 = v1;
	ASSERT_EQ(v2_2.GetSize(), v1.GetSize());
	ASSERT_EQ(v2_2.GetRealSize(), 4);
	for (SizeType i = 0; i < v2_2.GetSize(); i++)
	{
		ASSERT_TRUE(v2_2.GetObject(i).content == v1.GetObject(i).content);
	}
	v2_3 = v1;
	ASSERT_EQ(v2_3.GetSize(), v1.GetSize());
	ASSERT_EQ(v2_3.GetRealSize(), 8);
	for (SizeType i = 0; i < v2_3.GetSize(); i++)
	{
		ASSERT_TRUE(v2_3.GetObject(i).content == v1.GetObject(i).content);
	}

	Vector<int, MemoryManagerAllocator> v3 = {0, 1, 2};
	ASSERT_EQ(v3.GetSize(), 3);
	ASSERT_EQ(v3.GetRealSize(), 6);
	Vector<int, MemoryManagerAllocator> v3_1 = {0};
	ASSERT_EQ(v3_1.GetSize(), 1);
	ASSERT_EQ(v3_1.GetRealSize(), 2);
	Vector<int, MemoryManagerAllocator> v3_2 = {1, 0};
	ASSERT_EQ(v3_2.GetSize(), 2);
	ASSERT_EQ(v3_2.GetRealSize(), 4);
	Vector<int, MemoryManagerAllocator> v3_3 = {3, 2, 1, 0};
	ASSERT_EQ(v3_3.GetSize(), 4);
	ASSERT_EQ(v3_3.GetRealSize(), 8);

	v3_1 = v3;
	ASSERT_EQ(v3_1.GetSize(), v3.GetSize());
	ASSERT_EQ(v3_1.GetRealSize(), v3.GetRealSize());
	for (SizeType i = 0; i < v3_1.GetSize(); i++)
	{
		ASSERT_TRUE(v3_1.GetObject(i) == v3.GetObject(i));
	}

	v3_2 = v3;
	ASSERT_EQ(v3_2.GetSize(), v3.GetSize());
	ASSERT_EQ(v3_2.GetRealSize(), 4);
	for (SizeType i = 0; i < v3_2.GetSize(); i++)
	{
		ASSERT_TRUE(v3_2.GetObject(i) == v3.GetObject(i));
	}

	v3_3 = v3;
	ASSERT_EQ(v3_3.GetSize(), v3.GetSize());
	ASSERT_EQ(v3_3.GetRealSize(), 8);
	for (SizeType i = 0; i < v3_3.GetSize(); i++)
	{
		ASSERT_TRUE(v3_3.GetObject(i) == v3.GetObject(i));
	}

	Vector<int, StdAllocator> v4_1 = {0};
	ASSERT_EQ(v4_1.GetSize(), 1);
	ASSERT_EQ(v4_1.GetRealSize(), 2);
	Vector<int, StdAllocator> v4_2 = {1, 0};
	ASSERT_EQ(v4_2.GetSize(), 2);
	ASSERT_EQ(v4_2.GetRealSize(), 4);
	Vector<int, StdAllocator> v4_3 = {3, 2, 1, 0};
	ASSERT_EQ(v4_3.GetSize(), 4);
	ASSERT_EQ(v4_3.GetRealSize(), 8);

	v4_1 = v3;
	ASSERT_EQ(v4_1.GetSize(), v3.GetSize());
	ASSERT_EQ(v4_1.GetRealSize(), v3.GetRealSize());
	for (SizeType i = 0; i < v4_1.GetSize(); i++)
	{
		ASSERT_TRUE(v4_1.GetObject(i) == v3.GetObject(i));
	}

	v4_2 = v3;
	ASSERT_EQ(v4_2.GetSize(), v3.GetSize());
	ASSERT_EQ(v4_2.GetRealSize(), 4);
	for (SizeType i = 0; i < v4_2.GetSize(); i++)
	{
		ASSERT_TRUE(v4_2.GetObject(i) == v3.GetObject(i));
	}

	v4_3 = v3;
	ASSERT_EQ(v4_3.GetSize(), v3.GetSize());
	ASSERT_EQ(v4_3.GetRealSize(), 8);
	for (SizeType i = 0; i < v4_3.GetSize(); i++)
	{
		ASSERT_TRUE(v4_3.GetObject(i) == v3.GetObject(i));
	}
}

TEST(Vector, MoveAssignmentTest)
{
	Vector<test_vector_class, MemoryManagerAllocator> v1 = {0};
	ASSERT_EQ(v1.GetSize(), 1);
	ASSERT_EQ(v1.GetRealSize(), 2);
	v1 = Vector<test_vector_class, MemoryManagerAllocator>({0, 1, 2, 3});
	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(v1.GetRealSize(), 8);
	for (SizeType i = 0; i < v1.GetSize(); i++)
	{
		ASSERT_EQ(v1[i].content, i);
		ASSERT_EQ(v1[i].mi, 1);
	}

	Vector<test_vector_class, MemoryManagerAllocator> v2 = {1, 0};
	ASSERT_EQ(v2.GetSize(), 2);
	ASSERT_EQ(v2.GetRealSize(), 4);
	v2 = Vector<test_vector_class, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v2.GetSize(), 3);
	ASSERT_EQ(v2.GetRealSize(), 4);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_EQ(v2[i].content, i);
		if (i >= 2)
			ASSERT_EQ(v2[i].mi, 3);
		else
			ASSERT_EQ(v2[i].mi, 4);
	}

	Vector<test_vector_class, MemoryManagerAllocator> v3 = {1};
	ASSERT_EQ(v3.GetSize(), 1);
	ASSERT_EQ(v3.GetRealSize(), 2);
	v3 = Vector<test_vector_class, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v3.GetSize(), 3);
	ASSERT_EQ(v3.GetRealSize(), 6);
	for (SizeType i = 0; i < v3.GetSize(); i++)
	{
		ASSERT_EQ(v3[i].content, i);
		ASSERT_EQ(v3[i].mi, 3);
	}

	Vector<test_vector_class, MemoryManagerAllocator> v4 = {3, 2, 1, 0};
	ASSERT_EQ(v4.GetSize(), 4);
	ASSERT_EQ(v4.GetRealSize(), 8);
	v4 = Vector<test_vector_class, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v4.GetSize(), 3);
	ASSERT_EQ(v4.GetRealSize(), 8);
	for (SizeType i = 0; i < v4.GetSize(); i++)
	{
		ASSERT_EQ(v4[i].content, i);
		ASSERT_EQ(v4[i].mi, 4);
	}

	Vector<int, MemoryManagerAllocator> v5 = {1};
	ASSERT_EQ(v5.GetSize(), 1);
	ASSERT_EQ(v5.GetRealSize(), 2);
	v5 = Vector<int, MemoryManagerAllocator>({0, 1, 2, 3});
	ASSERT_EQ(v5.GetSize(), 4);
	ASSERT_EQ(v5.GetRealSize(), 8);
	for (SizeType i = 0; i < v5.GetSize(); i++)
	{
		ASSERT_EQ(v5[i], i);
	}

	Vector<int, MemoryManagerAllocator> v6 = {1, 0};
	ASSERT_EQ(v6.GetSize(), 2);
	ASSERT_EQ(v6.GetRealSize(), 4);
	v6 = Vector<int, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v6.GetSize(), 3);
	ASSERT_EQ(v6.GetRealSize(), 4);
	for (SizeType i = 0; i < v6.GetSize(); i++)
	{
		ASSERT_EQ(v6[i], i);
	}

	Vector<int, MemoryManagerAllocator> v7 = {1};
	ASSERT_EQ(v7.GetSize(), 1);
	ASSERT_EQ(v7.GetRealSize(), 2);
	v7 = Vector<int, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v7.GetSize(), 3);
	ASSERT_EQ(v7.GetRealSize(), 6);
	for (SizeType i = 0; i < v7.GetSize(); i++)
	{
		ASSERT_EQ(v7[i], i);
	}

	Vector<int, MemoryManagerAllocator> v8 = {3, 2, 1, 0};
	ASSERT_EQ(v8.GetSize(), 4);
	ASSERT_EQ(v8.GetRealSize(), 8);
	v8 = Vector<int, StdAllocator>({0, 1, 2});
	ASSERT_EQ(v8.GetSize(), 3);
	ASSERT_EQ(v8.GetRealSize(), 8);
	for (SizeType i = 0; i < v8.GetSize(); i++)
	{
		ASSERT_EQ(v8[i], i);
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

	Vector<test_vector_class>* ptest2 = new Vector<test_vector_class>({1, 0});
	ASSERT_TRUE(ptest2->GetObject(0).content == 1);
	ASSERT_TRUE(ptest2->GetObject(1).content == 0);
	ASSERT_TRUE(ptest2->GetSize() == 2);
	ASSERT_TRUE(ptest2->GetRealSize() == 4);
	int des_cot = 0;
	for (SizeType i = 0; i < ptest2->GetSize(); i++)
	{
		(*ptest2)[i].destruction_hook = [&]() {
			des_cot += 1;
		};
	}
	ASSERT_EQ(des_cot, 0);
	ptest2->SetRealSize(2);
	ASSERT_TRUE(ptest2->GetSize() == 2);
	ASSERT_TRUE(ptest2->GetRealSize() == 2);
	ASSERT_TRUE(ptest2->GetObject(0).content == 1);
	ASSERT_TRUE(ptest2->GetObject(1).content == 0);
	delete ptest2;
	ASSERT_EQ(des_cot, 2);
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
	int des_cot = 0;
	for (SizeType i = 0; i < v.GetSize(); i++)
	{
		v[i].destruction_hook = [&]() {
			des_cot += 1;
		};
	}
	ASSERT_EQ(des_cot, 0);
	v.Clear();
	ASSERT_EQ(des_cot, 3);
	ASSERT_TRUE(v.GetSize() == 0);
	ASSERT_TRUE(v.GetRealSize() == 4);
	ASSERT_TRUE(v.GetData() != nullptr);

	Vector<int> v2 = {2, 1, 0};
	ASSERT_EQ(v2.GetSize(), 3);
	ASSERT_EQ(v2.GetRealSize(), 6);
	v2.Clear();
	ASSERT_EQ(v2.GetSize(), 0);
	ASSERT_EQ(v2.GetRealSize(), 4);
	ASSERT_TRUE(v2.GetData() != nullptr);
}

TEST(Vector, SetSizeTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	ASSERT_TRUE(v.GetSize() == 3);
	ASSERT_TRUE(v.GetRealSize() == 6);
	int des_cot = 0;
	v[2].destruction_hook = [&]() {
		des_cot += 1;
	};
	ASSERT_EQ(des_cot, 0);
	v.SetSize(2, test_vector_class());
	ASSERT_TRUE(v.GetSize() == 2);
	ASSERT_TRUE(v.GetRealSize() == 6);
	ASSERT_EQ(des_cot, 1);
	v.SetSize(6, test_vector_class(1));
	ASSERT_TRUE(v.GetSize() == 6);
	ASSERT_TRUE(v.GetRealSize() == 6);
	ASSERT_TRUE((v.GetBegin() + 5)->content == 1);
	v.SetSize(8, test_vector_class());
	ASSERT_TRUE(v.GetSize() == 8);
	ASSERT_TRUE(v.GetRealSize() == 16);
	ASSERT_TRUE(v.GetBegin()->mi == 3);
	ASSERT_TRUE((v.GetBegin() + 6)->mi == 1);

	Vector<int> v2 = {0, 1, 2};
	ASSERT_EQ(v2.GetSize(), 3);
	ASSERT_EQ(v2.GetRealSize(), 6);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_EQ(v2[i], i);
	}
	v2.SetSize(2, 0);
	ASSERT_EQ(v2.GetSize(), 2);
	ASSERT_EQ(v2.GetRealSize(), 6);
	for (SizeType i = 0; i < v2.GetSize(); i++)
	{
		ASSERT_EQ(v2[i], i);
	}
	v2.SetSize(6, 10);
	ASSERT_EQ(v2.GetSize(), 6);
	ASSERT_EQ(v2.GetRealSize(), 6);
	for (SizeType i = 0; i < 2; i++)
	{
		ASSERT_EQ(v2[i], i);
	}
	for (SizeType i = 2; i < v2.GetSize(); i++)
	{
		ASSERT_EQ(v2[i], 10);
	}
	v2.SetSize(8, 11);
	ASSERT_EQ(v2.GetSize(), 8);
	ASSERT_EQ(v2.GetRealSize(), 16);
	for (SizeType i = 0; i < 2; i++)
	{
		ASSERT_EQ(v2[i], i);
	}
	for (SizeType i = 2; i < 6; i++)
	{
		ASSERT_EQ(v2[i], 10);
	}
	for (SizeType i = 6; i < 8; i++)
	{
		ASSERT_EQ(v2[i], 11);
	}

	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto gen_func = [&](int i) {
		return [&, i]() {
			val_pool[i] += 1;
		};
	};
	Vector<test_vector_class>* pv = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; i++)
	{
		pv->SetSize(i + 1, test_vector_class(i, gen_func(i)));
		ASSERT_EQ(pv->GetSize(), i + 1);
		ASSERT_EQ((pv->GetEnd() - 1)->content, i);
	}

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ((*pv)[i].content, i);
	}

	pv->SetSize(0, test_vector_class(0));
	ASSERT_EQ(pv->GetSize(), 0);
	ASSERT_EQ(pv->GetBegin(), pv->GetEnd());

	delete pv;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
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

TEST(Vector, PushBackTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	ASSERT_TRUE(v.GetSize() == 3);
	test_vector_class val(3);
	auto& val_r = v.PushBack(val);
	ASSERT_TRUE(val_r.content == val.content && val_r.content == 3);
	ASSERT_TRUE(v.GetSize() == 4);
	ASSERT_TRUE(val_r.mi == 1);
	auto& val_r2 = v.PushBack(test_vector_class(4));
	ASSERT_TRUE(val_r2.content == 4);
	ASSERT_TRUE(v.GetSize() == 5);
	ASSERT_TRUE(val_r2.mi == 3);

	for (int i = 0; i < 50; i++)
	{
		val.content = i;
		auto& val_r3 = v.PushBack(val);
		ASSERT_TRUE(v.GetSize() == 6 + i);
		ASSERT_TRUE(val_r3.content == i);
		ASSERT_TRUE(val_r3.mi == 1);
	}

	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto gen_func = [&](int i) {
		return [&, i]() {
			val_pool[i] += 1;
		};
	};
	Vector<test_vector_class>* pv = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; i++)
	{
		test_vector_class buf(i, gen_func(i));
		ASSERT_EQ(pv->PushBack(buf).content, i);
	}
	ASSERT_EQ(pv->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		test_vector_class buf(i, gen_func(i));
		ASSERT_EQ(pv->PushBack(std::move(buf)).content, i);
	}
	ASSERT_EQ(pv->GetSize(), 2 * test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ((*pv)[i].content, i);
	}
	for (int i = 2 * test_size - 1; i >= test_size; --i)
	{
		ASSERT_EQ((*pv)[i].content, i - test_size);
	}

	delete pv;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Vector, EmplaceBackTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	ASSERT_TRUE(v.GetSize() == 3);
	auto& val_r = v.EmplaceBack(3);
	ASSERT_TRUE(val_r.content == 3);
	ASSERT_TRUE(v.GetSize() == 4);
	ASSERT_TRUE(val_r.mi == 0);

	for (int i = 0; i < 50; i++)
	{
		auto& val_r2 = v.EmplaceBack(i);
		ASSERT_TRUE(val_r2.content == i);
		ASSERT_TRUE(v.GetSize() == 5 + i);
		ASSERT_TRUE(val_r2.mi == 0);
	}

	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto gen_func = [&](int i) {
		return [&, i]() {
			val_pool[i] += 1;
		};
	};
	Vector<test_vector_class>* pv = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pv->EmplaceBack(i, gen_func(i)).content, i);
	}
	ASSERT_EQ(pv->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ((*pv)[i].content, i);
	}

	delete pv;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Vector, InsertTest)
{
	Vector<test_vector_class> v1 = {0, 1, 2};

	test_vector_class val = -1;
	auto i1 = v1.Insert(v1.GetBegin(), val);
	ASSERT_TRUE(v1.GetSize() == 4);
	ASSERT_TRUE(v1[0].content == -1);
	ASSERT_TRUE(v1[3].mi == 3);
	for (int i = 1; i < 3; i++)
		ASSERT_TRUE(v1[i].mi == 4);
	ASSERT_EQ(v1[0].mi, 2);
	ASSERT_TRUE(i1->content == -1);

	val = 3;
	auto i2 = v1.Insert(v1.GetEnd(), val);
	ASSERT_TRUE(v1.GetSize() == 5);
	ASSERT_EQ(v1[4].content, 3);
	ASSERT_TRUE(v1[3].mi == 3);
	for (int i = 1; i < 3; i++)
		ASSERT_TRUE(v1[i].mi == 4);
	ASSERT_TRUE(v1[0].mi == 2);
	ASSERT_TRUE(v1[4].mi == 1);
	ASSERT_TRUE(i2->content == 3);

	val = -2;
	auto i3 = v1.Insert(v1.GetConstBegin() + 1, val);
	ASSERT_TRUE(v1.GetSize() == 6);
	ASSERT_TRUE(v1[1].content == -2);
	ASSERT_TRUE(v1[0].content == -1);
	ASSERT_TRUE(v1[1].mi == 2);
	ASSERT_TRUE(v1[0].mi == 2);
	for (int i = 2; i < 5; i++)
		ASSERT_TRUE(v1[i].mi == 4);
	ASSERT_TRUE(v1[5].mi == 3);
	ASSERT_TRUE(i3->content == -2);

	val = 4;
	auto i4 = v1.Insert(v1.GetConstEnd() - 1, val);
	ASSERT_TRUE(v1.GetSize() == 7);
	ASSERT_TRUE(v1[6].content == 3);
	ASSERT_TRUE(v1[5].content == 4);
	ASSERT_TRUE(i4->content == 4);

	val = -3;
	auto i5 = v1.Insert(v1.GetBegin(), std::move(val));
	ASSERT_TRUE(i5->mi == 4);
	ASSERT_TRUE(i5->content == -3);

	Vector<test_vector_class> v2 = {0, 1};
	auto i6 = v2.Insert(v2.GetConstBegin(), 4, -1);
	ASSERT_EQ(i6->content, -1);
	ASSERT_EQ(v2.GetSize(), 6);
	for (int i = 0; i < 4; i++)
		ASSERT_EQ(v2[i].content, -1);
	ASSERT_EQ(v2[4].content, 0);
	ASSERT_EQ(v2[5].content, 1);

	auto i7 = v2.Insert(v2.GetConstBegin() + 1, 4, -2);
	ASSERT_EQ(i7->content, -2);
	ASSERT_EQ(v2.GetSize(), 10);
	ASSERT_EQ(v2[0].content, -1);
	for (int i = 1; i < 5; i++)
		ASSERT_EQ(v2[i].content, -2);
	for (int i = 5; i < 8; i++)
		ASSERT_EQ(v2[i].content, -1);
	ASSERT_EQ(v2[8].content, 0);
	ASSERT_EQ(v2[9].content, 1);

	auto i8 = v2.Insert(v2.GetEnd(), 8, 2);
	ASSERT_EQ(i8->content, 2);
	ASSERT_EQ(v2.GetSize(), 18);
	ASSERT_EQ(v2[0].content, -1);
	for (int i = 1; i < 5; i++)
		ASSERT_EQ(v2[i].content, -2);
	for (int i = 5; i < 8; i++)
		ASSERT_EQ(v2[i].content, -1);
	ASSERT_EQ(v2[8].content, 0);
	ASSERT_EQ(v2[9].content, 1);
	for (int i = 10; i < 18; i++)
		ASSERT_EQ(v2[i].content, 2);

	auto i9 = v2.Insert(v2.GetConstEnd(), 20, 3);
	ASSERT_EQ(i9->content, 3);
	ASSERT_EQ(v2.GetSize(), 38);
	ASSERT_EQ(v2[0].content, -1);
	for (int i = 1; i < 5; i++)
		ASSERT_EQ(v2[i].content, -2);
	for (int i = 5; i < 8; i++)
		ASSERT_EQ(v2[i].content, -1);
	ASSERT_EQ(v2[8].content, 0);
	ASSERT_EQ(v2[9].content, 1);
	for (int i = 10; i < 18; i++)
		ASSERT_EQ(v2[i].content, 2);
	for (int i = 18; i < 38; i++)
		ASSERT_EQ(v2[i].content, 3);

	Vector<test_vector_class> v3 = {0, 1};
	auto i10 = v3.Insert(v3.GetConstBegin() + 1, v2.GetBegin(), v2.GetBegin() + 8);
	ASSERT_EQ(i10->content, v2.GetBegin()->content);
	ASSERT_EQ(v3.GetSize(), 10);
	ASSERT_EQ(v3[0].content, 0);
	ASSERT_EQ(v3[9].content, 1);
	for (int i = 1; i < 9; i++)
		ASSERT_EQ(v3[i].content, v2[i - 1].content);

	auto i11 = v3.Insert(v3.GetEnd(), v1.GetBegin(), v1.GetEnd());
	ASSERT_EQ(i11->content, v1.GetBegin()->content);
	ASSERT_EQ(v3.GetSize(), 10 + v1.GetSize());
	ASSERT_EQ(v3[0].content, 0);
	ASSERT_EQ(v3[9].content, 1);
	for (int i = 1; i < 9; i++)
		ASSERT_EQ(v3[i].content, v2[i - 1].content);
	for (int i = 10; i < 10 + v1.GetSize(); i++)
		ASSERT_EQ(v3[i].content, v1[i - 10].content);

	Vector<test_vector_class> v4 = {0, 1};
	auto i12 = v4.Insert(v4.GetBegin(), {-3, -2, -1});
	ASSERT_EQ(i12->content, -3);
	ASSERT_EQ(v4.GetSize(), 5);
	for (int i = 0; i < 3; i++)
		ASSERT_EQ(v4[i].content, i - 3);
	ASSERT_EQ(v4[3].content, 0);
	ASSERT_EQ(v4[4].content, 1);

	auto i13 = v4.Insert(v4.GetConstEnd(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
	ASSERT_EQ(i13->content, 1);
	ASSERT_EQ(v4.GetSize(), 20);
	for (int i = 0; i < 3; i++)
		ASSERT_EQ(v4[i].content, i - 3);
	ASSERT_EQ(v4[3].content, 0);
	ASSERT_EQ(v4[4].content, 1);
	for (int i = 5; i < 20; i++)
		ASSERT_EQ(v4[i].content, i - 4);

	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto gen_func = [&](int i) {
		return [&, i]() {
			val_pool[i] += 1;
		};
	};
	Vector<test_vector_class>* pv = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; i++)
	{
		test_vector_class buf(i, gen_func(i));
		auto iter = pv->Insert(pv->GetReverseBegin(), buf);
	}
	ASSERT_EQ(pv->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		val_check_pool[(*pv)[i].content] += 1;
	}
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	for (int i = 0; i < test_size; i++)
	{
		test_vector_class buf(i, gen_func(i));
		auto iter = pv->Insert(pv->GetConstEnd() - 1, std::move(buf));
	}
	ASSERT_EQ(pv->GetSize(), 2 * test_size);

	for (int i = 0; i < pv->GetSize(); i++)
	{
		val_check_pool[(*pv)[i].content] += 1;
	}
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 3);
	}

	Vector<test_vector_class>* pv2 = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; ++i)
		pv2->PushBack(test_vector_class(i, gen_func(i)));

	ASSERT_EQ(pv2->GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ((*pv2)[i].content, i);
	}

	pv->Insert(pv->GetBegin() + 1, pv2->GetConstReverseBegin(), pv2->GetConstReverseEnd());

	for (int i = 0; i < pv->GetSize(); i++)
	{
		val_check_pool[(*pv)[i].content] += 1;
	}
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 6);
	}

	delete pv2;
	delete pv;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 5);
	}

	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));

	pv = new Vector<test_vector_class>();

	ASSERT_EQ(pv->GetSize(), 0);

	{	 //to make tbuf release
		test_vector_class tbuf(1, gen_func(1));
		pv->Insert(pv->GetBegin(), test_size, tbuf);
	}

	ASSERT_EQ(pv->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ((*pv)[i].content, 1);
	}

	delete pv;

	for (int i = 0; i < test_size; i++)
	{
		if (i != 1)
			ASSERT_EQ(val_pool[i], 0);
	}
	ASSERT_EQ(val_pool[1], test_size + 1);

	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));

	pv = new Vector<test_vector_class>();

	pv->Insert(pv->GetConstEnd(), {test_vector_class(0, gen_func(0)),
								   test_vector_class(1, gen_func(1)),
								   test_vector_class(2, gen_func(2)),
								   test_vector_class(3, gen_func(3)),
								   test_vector_class(4, gen_func(4))});

	ASSERT_EQ(pv->GetSize(), 5);
	for (int i = 0; i < 5; ++i)
	{
		ASSERT_EQ((*pv)[i].content, i);
	}

	delete pv;
	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(val_pool[i], 2);	  //initializer_list's element can not be moved
	}
	for (int i = 5; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 0);
	}
}

TEST(Vector, EmplaceTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	Vector<test_vector_class>* pv = new Vector<test_vector_class>();
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pv->Emplace(pv->GetConstReverseEnd(), i, [&, i]() {
			val_pool[i] += 1;
		});
		ASSERT_EQ(iter->content, i);
	}
	int cnt = 0;
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pv)[cnt++].content, i);
	}

	delete pv;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Vector, PopBackTest)
{
	Vector<test_vector_class> test = {0, 1, 2, 3};

	ASSERT_EQ(test.GetSize(), 4);
	ASSERT_EQ((&test[2] + 1)->mi, 1);
	for (int i = 0; i < test.GetSize(); i++)
	{
		ASSERT_EQ(test[i].mi, 1);
		ASSERT_EQ(test[i].content, i);
	}

	test.PopBack();
#ifdef SGE_DEBUG
	ASSERT_EQ((&test[2] + 1)->mi, -1);
#endif
	ASSERT_EQ(test.GetSize(), 3);
	for (int i = 0; i < test.GetSize(); i++)
	{
		ASSERT_EQ(test[i].mi, 1);
		ASSERT_EQ(test[i].content, i);
	}
}

TEST(Vector, RemoveTest)
{
	Vector<test_vector_class> test1 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest1 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test1.GetSize(), 6);
	ASSERT_EQ(itest1.GetSize(), 6);

	int des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test1[i].mi, 1);
		ASSERT_EQ(test1[i].content, i);
		test1[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest1[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter1 = test1.Remove(test1.GetBegin() + 1);
	auto iiter1 = itest1.Remove(itest1.GetBegin() + 1);

	ASSERT_EQ(des_cot, 1);

	ASSERT_EQ(test1.GetSize(), 5);
	ASSERT_EQ(test1[0].mi, 1);
	ASSERT_EQ(test1[0].content, 0);

	ASSERT_EQ(itest1.GetSize(), 5);
	ASSERT_EQ(itest1[0], 0);

	ASSERT_EQ(test1[1].mi, 3);
	ASSERT_EQ(test1[1].content, 2);
	ASSERT_EQ(itest1[1], 2);

	for (int i = 2; i < 5; i++)
	{
		ASSERT_EQ(test1[i].mi, 4);
		ASSERT_EQ(test1[i].content, i + 1);

		ASSERT_EQ(itest1[i], i + 1);
	}
	ASSERT_EQ(iter1->mi, 3);
	ASSERT_EQ(iter1->content, 2);

	ASSERT_EQ(*iiter1, 2);

	Vector<test_vector_class> test2 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest2 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test2.GetSize(), 6);
	ASSERT_EQ(itest2.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test2[i].mi, 1);
		ASSERT_EQ(test2[i].content, i);
		test2[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest2[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter2 = test2.Remove(test2.GetBegin());
	auto iiter2 = itest2.Remove(itest2.GetBegin());

	ASSERT_EQ(des_cot, 1);

	ASSERT_EQ(test2.GetSize(), 5);
	ASSERT_EQ(itest2.GetSize(), 5);

	ASSERT_EQ(test2[0].mi, 3);
	ASSERT_EQ(test2[0].content, 1);

	ASSERT_EQ(itest2[0], 1);

	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ(test2[i].mi, 4);
		ASSERT_EQ(test2[i].content, i + 1);

		ASSERT_EQ(itest2[i], i + 1);
	}
	ASSERT_EQ(iter2->mi, 3);
	ASSERT_EQ(iter2->content, 1);

	ASSERT_EQ(*iiter2, 1);

	Vector<test_vector_class> test3 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest3 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test3.GetSize(), 6);
	ASSERT_EQ(itest3.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test3[i].mi, 1);
		ASSERT_EQ(test3[i].content, i);
		test3[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest3[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter3 = test3.Remove(test3.GetEnd() - 1);
	auto iiter3 = itest3.Remove(itest3.GetEnd() - 1);

	ASSERT_EQ(des_cot, 1);

	ASSERT_EQ(test3.GetSize(), 5);
	ASSERT_EQ(itest3.GetSize(), 5);

	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test3[i].mi, 1);
		ASSERT_EQ(test3[i].content, i);

		ASSERT_EQ(itest3[i], i);
	}
	ASSERT_EQ(iter3, test3.GetEnd());
	ASSERT_EQ(iiter3, itest3.GetEnd());

	Vector<test_vector_class> test4 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest4 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test4.GetSize(), 6);
	ASSERT_EQ(itest4.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test4[i].mi, 1);
		ASSERT_EQ(test4[i].content, i);
		test4[i].destruction_hook = [&]() {
			des_cot += 1;
		};
	}

	ASSERT_EQ(des_cot, 0);

	auto iter4 = test4.Remove(test4.GetConstBegin() + 1);
	auto iiter4 = itest4.Remove(itest4.GetConstBegin() + 1);

	ASSERT_EQ(des_cot, 1);

	ASSERT_EQ(test4.GetSize(), 5);
	ASSERT_EQ(itest4.GetSize(), 5);

	ASSERT_EQ(test4[0].mi, 1);
	ASSERT_EQ(test4[0].content, 0);
	ASSERT_EQ(itest4[0], 0);

	ASSERT_EQ(test4[1].mi, 3);
	ASSERT_EQ(test4[1].content, 2);

	ASSERT_EQ(itest4[1], 2);

	for (int i = 2; i < 5; i++)
	{
		ASSERT_EQ(test4[i].mi, 4);
		ASSERT_EQ(test4[i].content, i + 1);

		ASSERT_EQ(itest4[i], i + 1);
	}
	ASSERT_EQ(iter4->mi, 3);
	ASSERT_EQ(iter4->content, 2);
	ASSERT_EQ(*iiter4, 2);

	Vector<test_vector_class> test5 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest5 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test5.GetSize(), 6);
	ASSERT_EQ(itest5.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test5[i].mi, 1);
		ASSERT_EQ(test5[i].content, i);
		test5[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest5[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter5 = test5.Remove(test5.GetConstBegin());
	auto iiter5 = itest5.Remove(itest5.GetConstBegin());

	ASSERT_EQ(des_cot, 1);

	ASSERT_EQ(test5.GetSize(), 5);
	ASSERT_EQ(itest5.GetSize(), 5);

	ASSERT_EQ(test5[0].mi, 3);
	ASSERT_EQ(test5[0].content, 1);

	ASSERT_EQ(itest5[0], 1);

	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ(test5[i].mi, 4);
		ASSERT_EQ(test5[i].content, i + 1);

		ASSERT_EQ(itest5[i], i + 1);
	}
	ASSERT_EQ(iter5->mi, 3);
	ASSERT_EQ(iter5->content, 1);

	ASSERT_EQ(*iiter5, 1);

	Vector<test_vector_class> test6 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest6 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test6.GetSize(), 6);
	ASSERT_EQ(itest6.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test6[i].mi, 1);
		ASSERT_EQ(test6[i].content, i);
		test6[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest6[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter6 = test6.Remove(test6.GetConstEnd() - 1);
	auto iiter6 = itest6.Remove(itest6.GetConstEnd() - 1);

	ASSERT_EQ(test6.GetSize(), 5);
	ASSERT_EQ(itest6.GetSize(), 5);

	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test6[i].mi, 1);
		ASSERT_EQ(test6[i].content, i);

		ASSERT_EQ(itest6[i], i);
	}

	ASSERT_EQ(iter6, test6.GetConstEnd());
	ASSERT_EQ(iiter6, itest6.GetConstEnd());

	Vector<test_vector_class> test7 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest7 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test7.GetSize(), 6);
	ASSERT_EQ(itest7.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test7[i].mi, 1);
		ASSERT_EQ(test7[i].content, i);
		test7[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest7[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter7 = test7.Remove(test7.GetBegin() + 1, test7.GetEnd());
	auto iiter7 = itest7.Remove(itest7.GetBegin() + 1, itest7.GetEnd());

	ASSERT_EQ(des_cot, 5);

	ASSERT_EQ(test7.GetSize(), 1);
	ASSERT_EQ(itest7.GetSize(), 1);

	ASSERT_EQ(test7[0].mi, 1);
	ASSERT_EQ(test7[0].content, 0);

	ASSERT_EQ(itest7[0], 0);

	ASSERT_EQ(iter7, test7.GetEnd());
	ASSERT_EQ(iiter7, itest7.GetEnd());

#ifdef SGE_DEBUG
	for (int i = 2; i < 6; i++)
	{
		ASSERT_EQ((&test7[0] + i)->content, i);
		ASSERT_EQ((&test7[0] + i)->mi, -1);
	}
#endif
	Vector<test_vector_class> test8 = {0, 1, 2, 3, 4, 5};
	Vector<int> itest8 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test8.GetSize(), 6);
	ASSERT_EQ(itest8.GetSize(), 6);

	des_cot = 0;

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test8[i].mi, 1);
		ASSERT_EQ(test8[i].content, i);
		test8[i].destruction_hook = [&]() {
			des_cot += 1;
		};

		ASSERT_EQ(itest8[i], i);
	}

	ASSERT_EQ(des_cot, 0);

	auto iter8 = test8.Remove(test8.GetConstBegin(), test8.GetConstEnd() - 1);
	auto iiter8 = itest8.Remove(itest8.GetConstBegin(), itest8.GetConstEnd() - 1);

	ASSERT_EQ(des_cot, 5);

	ASSERT_EQ(test8.GetSize(), 1);
	ASSERT_EQ(itest8.GetSize(), 1);

	ASSERT_EQ(test8[0].mi, 3);
	ASSERT_EQ(test8[0].content, 5);

	ASSERT_EQ(itest8[0], 5);

	ASSERT_EQ(iter8, test8.GetConstBegin());
	ASSERT_EQ(iiter8, itest8.GetConstBegin());
#ifdef SGE_DEBUG
	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ((&test8[0] + i)->content, i);
		ASSERT_EQ((&test8[0] + i)->mi, -1);
	}
#endif
}

TEST(Vector, RemoveReverseTest)
{
	Vector<test_vector_class> v1 = {0, 1, 2, 3, 4, 5, 6};
	Vector<int> iv1 = {0, 1, 2, 3, 4, 5, 6};

	ASSERT_EQ(v1.GetSize(), 7);
	ASSERT_EQ(iv1.GetSize(), 7);

	int des_cot = 0;

	for (int i = 0; i < 7; i++)
	{
		ASSERT_EQ(v1[i].content, i);
		ASSERT_EQ(v1[i].mi, 1);
		ASSERT_EQ(iv1[i], i);

		v1[i].destruction_hook = [&]() {
			des_cot += 1;
		};
	}

	auto iter1 = v1.Remove(v1.GetConstReverseBegin() + 1);
	auto iiter1 = iv1.Remove(iv1.GetConstReverseBegin() + 1);

	ASSERT_EQ(iter1->mi, 1);
	ASSERT_EQ(iter1->content, 4);
	ASSERT_EQ(*iiter1, 4);

	ASSERT_EQ(v1.GetSize(), 6);
	ASSERT_EQ(iv1.GetSize(), 6);
	ASSERT_EQ(v1[5].mi, 3);
	ASSERT_EQ(v1[5].content, 6);
	ASSERT_EQ(iv1[5], 6);

	ASSERT_EQ(des_cot, 1);

	auto iter2 = v1.Remove(v1.GetReverseBegin() + 3, v1.GetReverseEnd() - 1);
	auto iiter2 = iv1.Remove(iv1.GetReverseBegin() + 3, iv1.GetReverseEnd() - 1);

	ASSERT_EQ(iter2->mi, 1);
	ASSERT_EQ(iter2->content, 0);
	ASSERT_EQ(*iiter2, 0);

	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(iv1.GetSize(), 4);
	ASSERT_EQ(v1[1].mi, 3);
	ASSERT_EQ(v1[1].content, 3);
	ASSERT_EQ(v1[2].mi, 3);
	ASSERT_EQ(v1[2].content, 4);
	ASSERT_EQ(v1[3].mi, 4);
	ASSERT_EQ(v1[3].content, 6);
	ASSERT_EQ(iv1[1], 3);
	ASSERT_EQ(iv1[2], 4);
	ASSERT_EQ(iv1[3], 6);

	ASSERT_EQ(des_cot, 3);
}

TEST(Vector, FindTest)
{
	Vector<int> test1 = {0, 1, 2, 3, 4, 5, 6};
	ASSERT_EQ(*(test1.Find(3)), 3);
	ASSERT_EQ(test1.Find(2) - test1.GetBegin(), 2);
	ASSERT_EQ(test1.Find(7), test1.GetEnd());

	const Vector<int> test2 = {0, 1, 2, 3, 4, 5, 6};
	ASSERT_EQ(*(test2.Find(3)), 3);
	ASSERT_EQ(test2.Find(2) - test2.GetConstBegin(), 2);
	ASSERT_EQ(test2.Find(7), test2.GetConstEnd());
}

TEST(Vector, FindByFunctionTest)
{
	Vector<int> test1 = {0, 1, 2, 3, 4, 5, 6};
	auto iter1 = test1.FindByFunction(Function<bool(const int&)>([](const int& i) { return i == 3; }));
	ASSERT_EQ(*iter1, 3);

	const Vector<int> test2 = {0, 1, 2, 3, 4, 5, 6};
	auto iter2 = test2.FindByFunction(Function<bool(const int&)>([](const int& i) { return i == 3; }));
	ASSERT_EQ(*iter2, 3);
}

TEST(Vector, FindAllTest)
{
	Vector<int> test1 = {0, 1, 2, 0, 1, 2};
	int cot1 = 0;
	test1.FindAll(0, Function<void(int&)>([&](int& i) {
					  ASSERT_EQ(i, 0);
					  i = 3;
					  cot1 += 1;
				  }));

	ASSERT_EQ(cot1, 2);
	ASSERT_EQ(test1[0], 3);
	ASSERT_EQ(test1[3], 3);

	const Vector<int> test2 = {0, 1, 2, 0, 1, 2};
	int cot2 = 0;
	test2.FindAll(0, Function<void(const int&)>([&](const int& i) {
					  cot2 += 1;
					  ASSERT_EQ(i, 0);
				  }));
	ASSERT_EQ(cot2, 2);
}

TEST(Vector, FindAllByFunctionTest)
{
	Vector<int> test1 = {0, 1, 2, 3, 4, 5, 6};
	int cot1 = 0;
	test1.FindAllByFunction(Function<bool(const int& i)>([](const int& i) {
								return i % 2 == 0;
							}),
							Function<void(int&)>([&](int& i) {
								ASSERT_EQ(i % 2, 0);
								cot1 += 1;
								i = 10;
							}));
	ASSERT_EQ(cot1, 4);
	for (int i = 0; i < test1.GetSize(); i++)
	{
		if (i % 2 == 0)
		{
			ASSERT_EQ(test1[i], 10);
		}
	}

	const Vector<int> test2 = {0, 1, 2, 3, 4, 5, 6};
	int cot2 = 0;
	test2.FindAllByFunction(Function<bool(const int& i)>([](const int& i) {
								return i % 2 == 0;
							}),
							Function<void(const int&)>([&](const int& i) {
								ASSERT_EQ(i % 2, 0);
								cot2 += 1;
							}));
	ASSERT_EQ(cot2, 4);
}

TEST(Vector, TrivialInsertTest)
{

	Vector<int> v1{0, 2};
	ASSERT_EQ(v1.GetSize(), 2);
	ASSERT_TRUE((IsTrivial<int>));

	int v = -1;

	v1.Insert(v1.GetEnd() - 1, v);
	ASSERT_EQ(v1.GetSize(), 3);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], -1);
	ASSERT_EQ(v1[2], 2);

	v = 1;

	v1.Insert(v1.GetBegin() + 1, v);
	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 2);

	v = 3;

	v1.Insert(v1.GetReverseBegin() + 1, v);
	ASSERT_EQ(v1.GetSize(), 5);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 3);
	ASSERT_EQ(v1[4], 2);

	v = 4;

	v1.Insert(v1.GetReverseBegin() + 2, v);
	ASSERT_EQ(v1.GetSize(), 6);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 4);
	ASSERT_EQ(v1[4], 3);
	ASSERT_EQ(v1[5], 2);

	Vector<int> v2{0, 2};
	ASSERT_EQ(v2.GetSize(), 2);
	ASSERT_TRUE((IsTrivial<int>));

	v2.Insert(v2.GetEnd() - 1, -1);
	ASSERT_EQ(v2.GetSize(), 3);
	ASSERT_EQ(v2[0], 0);
	ASSERT_EQ(v2[1], -1);
	ASSERT_EQ(v2[2], 2);

	v2.Insert(v2.GetBegin() + 1, 1);
	ASSERT_EQ(v2.GetSize(), 4);
	ASSERT_EQ(v2[0], 0);
	ASSERT_EQ(v2[1], 1);
	ASSERT_EQ(v2[2], -1);
	ASSERT_EQ(v2[3], 2);

	v2.Insert(v2.GetReverseBegin() + 1, 3);
	ASSERT_EQ(v2.GetSize(), 5);
	ASSERT_EQ(v2[0], 0);
	ASSERT_EQ(v2[1], 1);
	ASSERT_EQ(v2[2], -1);
	ASSERT_EQ(v2[3], 3);
	ASSERT_EQ(v2[4], 2);

	v2.Insert(v2.GetReverseBegin() + 2, 4);
	ASSERT_EQ(v2.GetSize(), 6);
	ASSERT_EQ(v2[0], 0);
	ASSERT_EQ(v2[1], 1);
	ASSERT_EQ(v2[2], -1);
	ASSERT_EQ(v2[3], 4);
	ASSERT_EQ(v2[4], 3);
	ASSERT_EQ(v2[5], 2);

	Vector<int> v3({0, 4});
	ASSERT_EQ(v3.GetSize(), 2);

	v3.Insert(v3.GetBegin() + 1, 3, 1);

	ASSERT_EQ(v3.GetSize(), 5);
	ASSERT_EQ(v3[0], 0);
	ASSERT_EQ(v3[1], 1);
	ASSERT_EQ(v3[2], 1);
	ASSERT_EQ(v3[3], 1);
	ASSERT_EQ(v3[4], 4);

	v3.Insert(v3.GetReverseBegin() + 2, 2, 2);
	ASSERT_EQ(v3.GetSize(), 7);
	ASSERT_EQ(v3[0], 0);
	ASSERT_EQ(v3[1], 1);
	ASSERT_EQ(v3[2], 1);
	ASSERT_EQ(v3[3], 2);
	ASSERT_EQ(v3[4], 2);
	ASSERT_EQ(v3[5], 1);
	ASSERT_EQ(v3[6], 4);

	Vector<int> v4({0, 4});
	Vector<int> vdat({1, 2, 3});
	ASSERT_EQ(v4.GetSize(), 2);
	ASSERT_EQ(vdat.GetSize(), 3);

	v4.Insert(v4.GetBegin() + 1, vdat.GetReverseBegin(), vdat.GetReverseEnd());

	ASSERT_EQ(v4.GetSize(), 5);
	ASSERT_EQ(v4[0], 0);
	ASSERT_EQ(v4[1], 3);
	ASSERT_EQ(v4[2], 2);
	ASSERT_EQ(v4[3], 1);
	ASSERT_EQ(v4[4], 4);

	v4.Insert(v4.GetReverseBegin() + 2, vdat.GetBegin(), vdat.GetEnd());

	ASSERT_EQ(v4.GetSize(), 8);
	ASSERT_EQ(v4[0], 0);
	ASSERT_EQ(v4[1], 3);
	ASSERT_EQ(v4[2], 2);
	ASSERT_EQ(v4[3], 3);
	ASSERT_EQ(v4[4], 2);
	ASSERT_EQ(v4[5], 1);
	ASSERT_EQ(v4[6], 1);
	ASSERT_EQ(v4[7], 4);

	Vector<int> v5({0, 4});
	ASSERT_EQ(v5.GetSize(), 2);

	v5.Insert(v5.GetBegin() + 1, {1, 2, 3});

	ASSERT_EQ(v5.GetSize(), 5);
	ASSERT_EQ(v5[0], 0);
	ASSERT_EQ(v5[1], 1);
	ASSERT_EQ(v5[2], 2);
	ASSERT_EQ(v5[3], 3);
	ASSERT_EQ(v5[4], 4);

	v5.Insert(v5.GetReverseBegin() + 2, {1, 2});

	ASSERT_EQ(v5.GetSize(), 7);
	ASSERT_EQ(v5[0], 0);
	ASSERT_EQ(v5[1], 1);
	ASSERT_EQ(v5[2], 2);
	ASSERT_EQ(v5[3], 2);
	ASSERT_EQ(v5[4], 1);
	ASSERT_EQ(v5[5], 3);
	ASSERT_EQ(v5[6], 4);
}

TEST(Vector, TrivialEmplaceTest)
{
	Vector<int> v1{0, 2};
	ASSERT_EQ(v1.GetSize(), 2);
	ASSERT_TRUE((IsTrivial<int>));

	v1.Emplace(v1.GetEnd() - 1, -1);
	ASSERT_EQ(v1.GetSize(), 3);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], -1);
	ASSERT_EQ(v1[2], 2);

	v1.Emplace(v1.GetBegin() + 1, 1);
	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 2);

	v1.Emplace(v1.GetReverseBegin() + 1, 3);
	ASSERT_EQ(v1.GetSize(), 5);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 3);
	ASSERT_EQ(v1[4], 2);

	v1.Emplace(v1.GetReverseBegin() + 2, 4);
	ASSERT_EQ(v1.GetSize(), 6);
	ASSERT_EQ(v1[0], 0);
	ASSERT_EQ(v1[1], 1);
	ASSERT_EQ(v1[2], -1);
	ASSERT_EQ(v1[3], 4);
	ASSERT_EQ(v1[4], 3);
	ASSERT_EQ(v1[5], 2);
}

TEST(Vector, EqualTest)
{
	const Vector<int, MemoryManagerAllocator> v1({1, 2, 3});
	const Vector<int, MemoryManagerAllocator> v2({1, 2});
	const Vector<int, StdAllocator> v2_({1, 2});
	const Vector<int, MemoryManagerAllocator> v3({1, 4, 3});
	const Vector<int, StdAllocator> v3_({1, 4, 3});
	const Vector<int, MemoryManagerAllocator> v4({1, 2, 3});
	const Vector<int, StdAllocator> v4_({1, 2, 3});

	ASSERT_FALSE(v1 == v2);
	ASSERT_FALSE(v1 == v2_);
	ASSERT_FALSE(v1 == v3);
	ASSERT_FALSE(v1 == v3_);
	ASSERT_TRUE(v1 == v4);
	ASSERT_TRUE(v1 == v4_);
}

TEST(Vector, NotEqualTest)
{
	const Vector<int, MemoryManagerAllocator> v1({1, 2, 3});
	const Vector<int, MemoryManagerAllocator> v2({1, 2});
	const Vector<int, StdAllocator> v2_({1, 2});
	const Vector<int, MemoryManagerAllocator> v3({1, 4, 3});
	const Vector<int, StdAllocator> v3_({1, 4, 3});
	const Vector<int, MemoryManagerAllocator> v4({1, 2, 3});
	const Vector<int, StdAllocator> v4_({1, 2, 3});

	ASSERT_TRUE(v1 != v2);
	ASSERT_TRUE(v1 != v2_);
	ASSERT_TRUE(v1 != v3);
	ASSERT_TRUE(v1 != v3_);
	ASSERT_FALSE(v1 != v4);
	ASSERT_FALSE(v1 != v4_);
}

TEST(Vector, SwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));

	Vector<test_vector_class>* pv1 = new Vector<test_vector_class>();

	for (int i = 0; i < test_size; ++i)
		pv1->EmplaceBack(i, [i, &val_pool]() { val_pool[i] += 1; });

	ASSERT_EQ(pv1->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv1)[i].content, i);

	Vector<test_vector_class>* pv2 = new Vector<test_vector_class>();

	for (int i = test_size; i < 2 * test_size; ++i)
		pv2->EmplaceBack(i, [i, &val_pool]() { val_pool[i] += 1; });

	ASSERT_EQ(pv2->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv2)[i].content, i + test_size);

	Vector<test_vector_class>* pv3 = new Vector<test_vector_class>(std::move(*pv1));

	ASSERT_EQ(pv3->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv3)[i].content, i);

	*pv1 = std::move(*pv2);

	ASSERT_EQ(pv1->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv1)[i].content, i + test_size);

	*pv2 = std::move(*pv3);

	ASSERT_EQ(pv2->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv2)[i].content, i);

	delete pv1;
	delete pv2;
	delete pv3;

	for (int i = 0; i < 2 * test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Vector, AnotherAllocatorSwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));

	Vector<test_vector_class, MemoryManagerAllocator>* pv1 = new Vector<test_vector_class, MemoryManagerAllocator>();

	for (int i = 0; i < test_size; ++i)
		pv1->EmplaceBack(i, [i, &val_pool]() { val_pool[i] += 1; });

	ASSERT_EQ(pv1->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv1)[i].content, i);

	Vector<test_vector_class, StdAllocator>* pv2 = new Vector<test_vector_class, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
		pv2->EmplaceBack(i, [i, &val_pool]() { val_pool[i] += 1; });

	ASSERT_EQ(pv2->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv2)[i].content, i + test_size);

	Vector<test_vector_class, MemoryManagerAllocator>* pv3 = new Vector<test_vector_class, MemoryManagerAllocator>(std::move(*pv1));

	ASSERT_EQ(pv3->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv3)[i].content, i);

	*pv1 = std::move(*pv2);

	ASSERT_EQ(pv1->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv1)[i].content, i + test_size);

	*pv2 = std::move(*pv3);

	ASSERT_EQ(pv2->GetSize(), test_size);
	for (int i = 0; i < test_size; ++i)
		ASSERT_EQ((*pv2)[i].content, i);

	delete pv1;
	delete pv2;
	delete pv3;

	for (int i = 0; i < 2 * test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Vector, ConstructByEmptyListTest)
{
	Vector<int> v({});
	ASSERT_EQ(v.GetSize(), 0);

	v.Insert(v.GetConstEnd(), {1, 2, 3});
	ASSERT_EQ(v.GetSize(), 3);
	ASSERT_EQ(v[0], 1);
	ASSERT_EQ(v[1], 2);
	ASSERT_EQ(v[2], 3);
}

TEST(Vector, InsertEmptyListTest)
{
	Vector<int> v({1, 2, 3});
	ASSERT_EQ(v.GetSize(), 3);
	ASSERT_EQ(v[0], 1);
	ASSERT_EQ(v[1], 2);
	ASSERT_EQ(v[2], 3);

	v.Insert(v.GetConstBegin() + 1, {});

	ASSERT_EQ(v.GetSize(), 3);
	ASSERT_EQ(v[0], 1);
	ASSERT_EQ(v[1], 2);
	ASSERT_EQ(v[2], 3);
}

TEST(VectorIterator, IsVectorIteratorTest)
{
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::Iterator>::Value));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::ReverseIterator>::Value));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::ConstReverseIterator>::Value));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<int>::Value == false));
	ASSERT_FALSE((Vector<float>::IsVectorIterator<Vector<int>::Iterator>::Value));
	ASSERT_FALSE((Vector<float>::IsVectorIterator<Vector<int>::ConstIterator>::Value));
	ASSERT_FALSE((Vector<float>::IsVectorIterator<Vector<int>::ReverseIterator>::Value));
	ASSERT_FALSE((Vector<float>::IsVectorIterator<Vector<int>::ConstReverseIterator>::Value));
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

TEST(VectorIterator, GetConstBeginTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetConstBegin();
	ASSERT_TRUE(*iter1 == 0);
}

TEST(VectorIterator, GetConstEndTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetConstEnd();
	ASSERT_TRUE(*(iter1 - 1) == 2);
}

TEST(VectorIterator, GetReverseBeginTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetReverseBegin();
	ASSERT_EQ(*iter1, 2);
}

TEST(VectorIterator, GetReverseEndTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetReverseEnd();
	ASSERT_EQ(*(iter1 - 1), 0);
}

TEST(VectorIterator, GetConstReverseBeginTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetConstReverseBegin();
	ASSERT_EQ(*iter1, 2);
}

TEST(VectorIterator, GetConstReverseEndTest)
{
	Vector<int> test1 = {0, 1, 2};
	auto iter1 = test1.GetConstReverseEnd();
	ASSERT_EQ(*(iter1 - 1), 0);
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
	for (auto i = v.GetConstBegin(); i != v.GetConstBegin() + v.GetSize(); i += 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetConstBegin()));
	}
	for (auto i = v.GetConstBegin(); i != v.GetConstBegin() + v.GetSize(); i = i + 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetConstBegin()));
	}
	for (auto i = v.GetConstBegin() + v.GetSize() - 1; (i == v.GetConstBegin() - 1) == false; i -= 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetConstBegin()));
	}
	for (auto i = v.GetConstBegin() + v.GetSize() - 1; (i == v.GetConstBegin() - 1) == false; i = i - 1)
	{
		ASSERT_TRUE(*i == v.GetObject(i - v.GetConstBegin()));
	}
	for (auto i = v.GetReverseBegin(); i != v.GetReverseBegin() + v.GetSize(); i += 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetReverseEnd() - i - 1));
	}
	for (auto i = v.GetReverseBegin(); i != v.GetReverseEnd(); i = i + 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetReverseEnd() - i - 1));
	}
	for (auto i = v.GetReverseEnd() - 1; i != v.GetReverseBegin() - 1; i -= 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetReverseEnd() - i - 1));
	}
	for (auto i = v.GetReverseEnd() - 1; i != v.GetReverseBegin() - 1; i = i - 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetReverseEnd() - i - 1));
	}
	for (auto i = v.GetConstReverseBegin(); i != v.GetConstReverseBegin() + v.GetSize(); i += 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetConstReverseEnd() - i - 1));
	}
	for (auto i = v.GetConstReverseBegin(); i != v.GetConstReverseEnd(); i = i + 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetConstReverseEnd() - i - 1));
	}
	for (auto i = v.GetConstReverseEnd() - 1; i != v.GetConstReverseBegin() - 1; i -= 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetConstReverseEnd() - i - 1));
	}
	for (auto i = v.GetConstReverseEnd() - 1; i != v.GetConstReverseBegin() - 1; i = i - 1)
	{
		ASSERT_EQ(*i, v.GetObject(v.GetConstReverseEnd() - i - 1));
	}
}

TEST(VectorIterator, GetTest)
{
	Vector<test_vector_class> v = {0, 1, 2};
	auto iter = v.GetBegin();
	ASSERT_TRUE((*iter).content == 0);
	ASSERT_TRUE(iter->content == 0);
	auto citer = v.GetConstBegin();
	ASSERT_TRUE((*citer).content == 0);
	ASSERT_TRUE(citer->content == 0);
	auto riter = v.GetReverseBegin();
	ASSERT_EQ((*riter).content, 2);
	ASSERT_EQ(riter->content, 2);
	auto criter = v.GetConstReverseBegin();
	ASSERT_EQ((*criter).content, 2);
	ASSERT_EQ(criter->content, 2);
}

TEST(VectorIterator, OutOfRangeErrorTest)
{
	Vector<int> test = {0, 1, 2};
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin(), test.GetData(), test.GetData() + test.GetSize()) == false));
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin() - 1, test.GetData(), test.GetData() + test.GetSize()) == true));
	ASSERT_TRUE((Vector<int>::ConstIterator::OutOfRangeError::Judge(test.GetConstBegin(), test.GetData(), test.GetData() + test.GetSize()) == false));
	ASSERT_TRUE((Vector<int>::ConstIterator::OutOfRangeError::Judge(test.GetConstBegin() - 1, test.GetData(), test.GetData() + test.GetSize()) == true));
	ASSERT_FALSE((Vector<int>::ReverseIterator::OutOfRangeError::Judge(test.GetReverseBegin(), test.GetData() - 1, test.GetData() + test.GetSize() - 1)));
	ASSERT_TRUE((Vector<int>::ReverseIterator::OutOfRangeError::Judge(test.GetReverseBegin() - 1, test.GetData() - 1, test.GetData() + test.GetSize() - 1)));
	ASSERT_FALSE((Vector<int>::ConstReverseIterator::OutOfRangeError::Judge(test.GetConstReverseBegin(), test.GetData() - 1, test.GetData() + test.GetSize() - 1)));
	ASSERT_TRUE((Vector<int>::ConstReverseIterator::OutOfRangeError::Judge(test.GetConstReverseBegin() - 1, test.GetData() - 1, test.GetData() + test.GetSize() - 1)));
}

TEST(VectorIterator, ReverseIteratorTest)
{
	Vector<int> v1 = {0, 1, 2};

	ASSERT_EQ(v1.GetReverseEnd() - Vector<int>::ReverseIterator::GetBegin(v1), 3);

	int i1 = 3;
	ASSERT_EQ(v1.GetSize(), 3);
	auto iter1 = v1.Insert(v1.GetReverseBegin(), i1);
	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(v1[3], 3);
	ASSERT_EQ(v1[2], 2);
	ASSERT_EQ(*iter1, 3);

	i1 = -1;
	auto iter2 = v1.Insert(v1.GetReverseEnd(), i1);
	ASSERT_EQ(v1.GetSize(), 5);
	ASSERT_EQ(v1[0], -1);
	ASSERT_EQ(v1[1], 0);
	ASSERT_EQ(*iter2, -1);

	Vector<test_vector_class> v2 = {0, 1, 2};

	test_vector_class i2 = 3;

	ASSERT_EQ(v2.GetSize(), 3);
	auto iter3 = v2.Insert(v2.GetReverseBegin(), std::move(i2));
	ASSERT_EQ(v2.GetSize(), 4);
	ASSERT_EQ(v2[3].content, 3);
	ASSERT_EQ(v2[3].mi, 3);
	ASSERT_EQ(v2[2].content, 2);
	ASSERT_EQ(iter3->content, 3);
	ASSERT_EQ(iter3->mi, 3);

	i2 = -1;
	auto iter4 = v2.Insert(v2.GetReverseEnd(), std::move(i2));
	ASSERT_EQ(v2.GetSize(), 5);
	ASSERT_EQ(v2[0].content, -1);
	ASSERT_EQ(v2[0].mi, 4);
	ASSERT_EQ(v2[1].content, 0);
	ASSERT_EQ(iter4->content, -1);
	ASSERT_EQ(iter4->mi, 4);

	Vector<int> v3 = {0, 1, 2, 3, 4};

	ASSERT_EQ(v3.GetSize(), 5);
	auto iter5 = v3.Insert(v3.GetReverseBegin(), 3, 10);
	ASSERT_EQ(v3.GetSize(), 8);
	for (SizeType i = 5; i < 8; i++)
		ASSERT_EQ(v3[i], 10);
	for (auto i = iter5; i != iter5 + 3; i += 1)
		ASSERT_EQ(*i, 10);

	auto iter6 = v3.Insert(v3.GetReverseEnd(), 3, -1);
	ASSERT_EQ(v3.GetSize(), 11);
	for (SizeType i = 0; i < 3; i++)
		ASSERT_EQ(v3[i], -1);
	for (auto i = iter6; i != iter6 + 3; i += 1)
		ASSERT_EQ(*i, -1);

	auto iter7 = v3.Insert(v3.GetReverseBegin() + 1, 50, 5);
	ASSERT_EQ(v3.GetSize(), 61);
	ASSERT_EQ(v3[60], 10);
	for (SizeType i = 10; i < 60; i++)
		ASSERT_EQ(v3[i], 5);
	ASSERT_EQ(v3[9], 10);
	ASSERT_EQ(v3[8], 10);
	for (auto i = iter7; i != iter7 + 50; i += 1)
		ASSERT_EQ(*i, 5);

	Vector<int> v4 = {0, 1, 2, 3, 4};
	Vector<int> d1 = {7, 6, 5};

	ASSERT_EQ(v4.GetSize(), 5);
	auto iter8 = v4.Insert(v4.GetReverseBegin(), d1.GetConstBegin(), d1.GetConstEnd());
	ASSERT_EQ(v4.GetSize(), 8);
	ASSERT_EQ(*iter8, 7);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v4[i], i);
	for (auto i = iter8; i != v4.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (i - v4.GetReverseBegin()));

	Vector<int> d2 = {-3, -2, -1};
	auto iter9 = v4.Insert(v4.GetReverseEnd(), d2.GetReverseBegin(), d2.GetReverseEnd());
	ASSERT_EQ(v4.GetSize(), 11);
	ASSERT_EQ(*iter9, -1);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v4[i], i - 3);
	for (auto i = iter9; i != v4.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, -4 + (int)(v4.GetReverseEnd() - i));

	Vector<int> v5 = {0, 1, 2, 3, 7};
	Vector<int> d3 = {6, 5, 4};
	auto iter10 = v5.Insert(v5.GetReverseBegin() + 1, d3.GetBegin(), d3.GetEnd());
	ASSERT_EQ(v5.GetSize(), 8);
	ASSERT_EQ(*iter10, 6);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v5[i], i);
	for (auto i = iter10; i != v5.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (int)(i - v5.GetReverseBegin()));

	Vector<int> v6 = {0, 1, 2, 3, 4};

	ASSERT_EQ(v6.GetSize(), 5);
	auto iter11 = v6.Insert(v6.GetReverseBegin(), {7, 6, 5});
	ASSERT_EQ(v6.GetSize(), 8);
	ASSERT_EQ(*iter11, 7);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v6[i], i);
	for (auto i = iter11; i != v6.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (i - v6.GetReverseBegin()));

	auto iter12 = v6.Insert(v6.GetReverseEnd(), {-1, -2, -3});
	ASSERT_EQ(v6.GetSize(), 11);
	ASSERT_EQ(*iter12, -1);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v6[i], i - 3);
	for (auto i = iter12; i != v6.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, -4 + (int)(v6.GetReverseEnd() - i));

	Vector<int> v7 = {0, 1, 10};
	ASSERT_EQ(v7.GetSize(), 3);
	auto iter13 = v7.Insert(v7.GetReverseBegin() + 1, {9, 8, 7, 6, 5, 4, 3, 2});
	ASSERT_EQ(v7.GetSize(), 11);
	ASSERT_EQ(*iter13, 9);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v7[i], i);
	for (auto i = iter13; i != v7.GetReverseEnd(); i += 1)
		ASSERT_EQ(*i, 10 - (i - v7.GetReverseBegin()));

	Vector<int> v8 = {0, 1, 2, 3, 4};
	ASSERT_EQ(v8.GetSize(), 5);
	v8.Remove(v8.GetReverseBegin());
	ASSERT_EQ(v8.GetSize(), 4);
	for (SizeType i = 0; i < 4; i++)
	{
		ASSERT_EQ(v8[i], i);
	}

	v8.Remove(v8.GetReverseEnd() - 1);
	ASSERT_EQ(v8.GetSize(), 3);
	for (SizeType i = 0; i < 3; i++)
	{
		ASSERT_EQ(v8[i], i + 1);
	}

	v8.Remove(v8.GetReverseBegin() + 1);
	ASSERT_EQ(v8.GetSize(), 2);
	ASSERT_EQ(v8[0], 1);
	ASSERT_EQ(v8[1], 3);

	Vector<int> v9 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	ASSERT_EQ(v9.GetSize(), 10);
	v9.Remove(v9.GetReverseBegin() + 2, v9.GetReverseEnd() - 2);
	ASSERT_EQ(v9.GetSize(), 4);
	ASSERT_EQ(v9[0], 0);
	ASSERT_EQ(v9[1], 1);
	ASSERT_EQ(v9[2], 8);
	ASSERT_EQ(v9[3], 9);
}

TEST(VectorIterator, ConstReverseIteratorTest)
{
	Vector<int> v1 = {0, 1, 2};

	ASSERT_EQ(v1.GetConstReverseEnd() - Vector<int>::ConstReverseIterator::GetBegin(v1), 3);

	int i1 = 3;
	ASSERT_EQ(v1.GetSize(), 3);
	auto iter1 = v1.Insert(v1.GetConstReverseBegin(), i1);
	ASSERT_EQ(v1.GetSize(), 4);
	ASSERT_EQ(v1[3], 3);
	ASSERT_EQ(v1[2], 2);
	ASSERT_EQ(*iter1, 3);

	i1 = -1;
	auto iter2 = v1.Insert(v1.GetConstReverseEnd(), i1);
	ASSERT_EQ(v1.GetSize(), 5);
	ASSERT_EQ(v1[0], -1);
	ASSERT_EQ(v1[1], 0);
	ASSERT_EQ(*iter2, -1);

	Vector<test_vector_class> v2 = {0, 1, 2};

	test_vector_class i2 = 3;

	ASSERT_EQ(v2.GetSize(), 3);
	auto iter3 = v2.Insert(v2.GetConstReverseBegin(), std::move(i2));
	ASSERT_EQ(v2.GetSize(), 4);
	ASSERT_EQ(v2[3].content, 3);
	ASSERT_EQ(v2[3].mi, 3);
	ASSERT_EQ(v2[2].content, 2);
	ASSERT_EQ(iter3->content, 3);
	ASSERT_EQ(iter3->mi, 3);

	i2 = -1;
	auto iter4 = v2.Insert(v2.GetConstReverseEnd(), std::move(i2));
	ASSERT_EQ(v2.GetSize(), 5);
	ASSERT_EQ(v2[0].content, -1);
	ASSERT_EQ(v2[0].mi, 4);
	ASSERT_EQ(v2[1].content, 0);
	ASSERT_EQ(iter4->content, -1);
	ASSERT_EQ(iter4->mi, 4);

	Vector<int> v3 = {0, 1, 2, 3, 4};

	ASSERT_EQ(v3.GetSize(), 5);
	auto iter5 = v3.Insert(v3.GetConstReverseBegin(), 3, 10);
	ASSERT_EQ(v3.GetSize(), 8);
	for (SizeType i = 5; i < 8; i++)
		ASSERT_EQ(v3[i], 10);
	for (auto i = iter5; i != iter5 + 3; i += 1)
		ASSERT_EQ(*i, 10);

	auto iter6 = v3.Insert(v3.GetConstReverseEnd(), 3, -1);
	ASSERT_EQ(v3.GetSize(), 11);
	for (SizeType i = 0; i < 3; i++)
		ASSERT_EQ(v3[i], -1);
	for (auto i = iter6; i != iter6 + 3; i += 1)
		ASSERT_EQ(*i, -1);

	auto iter7 = v3.Insert(v3.GetConstReverseBegin() + 1, 50, 5);
	ASSERT_EQ(v3.GetSize(), 61);
	ASSERT_EQ(v3[60], 10);
	for (SizeType i = 10; i < 60; i++)
		ASSERT_EQ(v3[i], 5);
	ASSERT_EQ(v3[9], 10);
	ASSERT_EQ(v3[8], 10);
	for (auto i = iter7; i != iter7 + 50; i += 1)
		ASSERT_EQ(*i, 5);

	Vector<int> v4 = {0, 1, 2, 3, 4};
	Vector<int> d1 = {7, 6, 5};

	ASSERT_EQ(v4.GetSize(), 5);
	auto iter8 = v4.Insert(v4.GetConstReverseBegin(), d1.GetConstBegin(), d1.GetConstEnd());
	ASSERT_EQ(v4.GetSize(), 8);
	ASSERT_EQ(*iter8, 7);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v4[i], i);
	for (auto i = iter8; i != v4.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (i - v4.GetConstReverseBegin()));

	Vector<int> d2 = {-3, -2, -1};
	auto iter9 = v4.Insert(v4.GetConstReverseEnd(), d2.GetConstReverseBegin(), d2.GetConstReverseEnd());
	ASSERT_EQ(v4.GetSize(), 11);
	ASSERT_EQ(*iter9, -1);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v4[i], i - 3);
	for (auto i = iter9; i != v4.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, -4 + (int)(v4.GetConstReverseEnd() - i));

	Vector<int> v5 = {0, 1, 2, 3, 7};
	Vector<int> d3 = {6, 5, 4};
	auto iter10 = v5.Insert(v5.GetConstReverseBegin() + 1, d3.GetBegin(), d3.GetEnd());
	ASSERT_EQ(v5.GetSize(), 8);
	ASSERT_EQ(*iter10, 6);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v5[i], i);
	for (auto i = iter10; i != v5.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (int)(i - v5.GetConstReverseBegin()));

	Vector<int> v6 = {0, 1, 2, 3, 4};

	ASSERT_EQ(v6.GetSize(), 5);
	auto iter11 = v6.Insert(v6.GetConstReverseBegin(), {7, 6, 5});
	ASSERT_EQ(v6.GetSize(), 8);
	ASSERT_EQ(*iter11, 7);
	for (SizeType i = 0; i < 8; i++)
		ASSERT_EQ(v6[i], i);
	for (auto i = iter11; i != v6.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, 7 - (i - v6.GetConstReverseBegin()));

	auto iter12 = v6.Insert(v6.GetConstReverseEnd(), {-1, -2, -3});
	ASSERT_EQ(v6.GetSize(), 11);
	ASSERT_EQ(*iter12, -1);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v6[i], i - 3);
	for (auto i = iter12; i != v6.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, -4 + (int)(v6.GetConstReverseEnd() - i));

	Vector<int> v7 = {0, 1, 10};
	ASSERT_EQ(v7.GetSize(), 3);
	auto iter13 = v7.Insert(v7.GetConstReverseBegin() + 1, {9, 8, 7, 6, 5, 4, 3, 2});
	ASSERT_EQ(v7.GetSize(), 11);
	ASSERT_EQ(*iter13, 9);
	for (SizeType i = 0; i < 11; i++)
		ASSERT_EQ(v7[i], i);
	for (auto i = iter13; i != v7.GetConstReverseEnd(); i += 1)
		ASSERT_EQ(*i, 10 - (i - v7.GetConstReverseBegin()));

	Vector<int> v8 = {0, 1, 2, 3, 4};
	ASSERT_EQ(v8.GetSize(), 5);
	auto iter14 = v8.Remove(v8.GetConstReverseBegin());
	ASSERT_EQ(*iter14, 3);
	ASSERT_EQ(v8.GetSize(), 4);
	for (SizeType i = 0; i < 4; i++)
	{
		ASSERT_EQ(v8[i], i);
	}

	auto iter15 = v8.Remove(v8.GetConstReverseEnd() - 1);
	ASSERT_EQ(iter15, v8.GetConstReverseEnd());
	ASSERT_EQ(v8.GetSize(), 3);
	for (SizeType i = 0; i < 3; i++)
	{
		ASSERT_EQ(v8[i], i + 1);
	}

	auto iter16 = v8.Remove(v8.GetConstReverseBegin() + 1);
	ASSERT_EQ(*iter16, 1);
	ASSERT_EQ(v8.GetSize(), 2);
	ASSERT_EQ(v8[0], 1);
	ASSERT_EQ(v8[1], 3);

	Vector<int> v9 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	ASSERT_EQ(v9.GetSize(), 10);
	auto iter17 = v9.Remove(v9.GetConstReverseBegin() + 2, v9.GetConstReverseEnd() - 2);
	ASSERT_EQ(*iter17, 1);
	ASSERT_EQ(v9.GetSize(), 4);
	ASSERT_EQ(v9[0], 0);
	ASSERT_EQ(v9[1], 1);
	ASSERT_EQ(v9[2], 8);
	ASSERT_EQ(v9[3], 9);
}

TEST(VectorIterator, SelfIncrementTest)
{
	Vector<int> v = {0, 1, 2, 3, 4};
	auto iter1 = v.GetBegin();
	auto iter2 = v.GetConstBegin();

	for (; iter1 != v.GetEnd() && iter2 != v.GetConstEnd(); iter1++, ++iter2)
		ASSERT_EQ(*iter1, *iter2);

	ASSERT_EQ(iter1, v.GetEnd());
	ASSERT_EQ(iter2, v.GetConstEnd());

	auto riter1 = v.GetReverseBegin();
	auto riter2 = v.GetConstReverseBegin();
	for (; riter1 != v.GetReverseEnd() && riter2 != v.GetConstReverseEnd(); riter1++, ++riter2)
		ASSERT_EQ(*riter1, *riter2);

	ASSERT_EQ(riter1, v.GetReverseEnd());
	ASSERT_EQ(riter2, v.GetConstReverseEnd());
}

TEST(VectorIterator, SelfDecrementTest)
{
	Vector<int> v = {0, 1, 2, 3, 4};
	auto iter1 = v.GetEnd() - 1;
	auto iter2 = v.GetConstEnd() - 1;

	for (; iter1 != v.GetBegin() - 1 && iter2 != v.GetConstBegin() - 1; iter1--, --iter2)
		ASSERT_EQ(*iter1, *iter2);

	ASSERT_EQ(iter1, v.GetBegin() - 1);
	ASSERT_EQ(iter2, v.GetConstBegin() - 1);

	auto riter1 = v.GetReverseEnd() - 1;
	auto riter2 = v.GetConstReverseEnd() - 1;
	for (; riter1 != v.GetReverseBegin() - 1 && riter2 != v.GetConstReverseBegin() - 1; riter1--, --riter2)
		ASSERT_EQ(*riter1, *riter2);

	ASSERT_EQ(riter1, v.GetReverseBegin() - 1);
	ASSERT_EQ(riter2, v.GetConstReverseBegin() - 1);
}

TEST(VectorIterator, ConvertTest)
{
	Vector<int> v1 = {0, 1, 2, 3, 4, 5};

	Vector<int>::Iterator i1_1 = v1.GetReverseBegin();
	ASSERT_EQ(*i1_1, 5);

	Vector<int>::ConstIterator i1_2 = v1.GetBegin();
	ASSERT_EQ(*i1_2, 0);
	Vector<int>::ConstIterator i1_3 = v1.GetReverseBegin();
	ASSERT_EQ(*i1_3, 5);
	Vector<int>::ConstIterator i1_4 = v1.GetConstReverseBegin();
	ASSERT_EQ(*i1_4, 5);

	Vector<int>::ReverseIterator i1_5 = v1.GetBegin();
	ASSERT_EQ(*i1_5, 0);

	Vector<int>::ConstReverseIterator i1_6 = v1.GetReverseBegin();
	ASSERT_EQ(*i1_6, 5);
	Vector<int>::ConstReverseIterator i1_7 = v1.GetBegin();
	ASSERT_EQ(*i1_7, 0);
	Vector<int>::ConstReverseIterator i1_8 = v1.GetConstBegin();
	ASSERT_EQ(*i1_8, 0);
}