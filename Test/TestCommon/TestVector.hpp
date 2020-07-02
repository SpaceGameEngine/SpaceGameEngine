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
	ASSERT_EQ((&test[2] + 1)->mi, -1);
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

	ASSERT_EQ(test1.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test1[i].mi, 1);
		ASSERT_EQ(test1[i].content, i);
	}

	auto iter1 = test1.Remove(test1.GetBegin() + 1);

	ASSERT_EQ(test1.GetSize(), 5);
	ASSERT_EQ(test1[0].mi, 1);
	ASSERT_EQ(test1[0].content, 0);
	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ(test1[i].mi, 3);
		ASSERT_EQ(test1[i].content, i + 1);
	}
	ASSERT_EQ(iter1->mi, 3);
	ASSERT_EQ(iter1->content, 2);

	Vector<test_vector_class> test2 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test2.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test2[i].mi, 1);
		ASSERT_EQ(test2[i].content, i);
	}

	auto iter2 = test2.Remove(test2.GetBegin());

	ASSERT_EQ(test2.GetSize(), 5);
	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test2[i].mi, 3);
		ASSERT_EQ(test2[i].content, i + 1);
	}
	ASSERT_EQ(iter2->mi, 3);
	ASSERT_EQ(iter2->content, 1);

	Vector<test_vector_class> test3 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test3.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test3[i].mi, 1);
		ASSERT_EQ(test3[i].content, i);
	}

	auto iter3 = test3.Remove(test3.GetEnd() - 1);

	ASSERT_EQ(test3.GetSize(), 5);
	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test3[i].mi, 1);
		ASSERT_EQ(test3[i].content, i);
	}
	ASSERT_EQ(iter3, test3.GetEnd());

	Vector<test_vector_class> test4 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test4.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test4[i].mi, 1);
		ASSERT_EQ(test4[i].content, i);
	}

	auto iter4 = test4.Remove(test4.GetConstBegin() + 1);

	ASSERT_EQ(test4.GetSize(), 5);
	ASSERT_EQ(test4[0].mi, 1);
	ASSERT_EQ(test4[0].content, 0);
	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ(test4[i].mi, 3);
		ASSERT_EQ(test4[i].content, i + 1);
	}
	ASSERT_EQ(iter4->mi, 3);
	ASSERT_EQ(iter4->content, 2);

	Vector<test_vector_class> test5 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test5.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test5[i].mi, 1);
		ASSERT_EQ(test5[i].content, i);
	}

	auto iter5 = test5.Remove(test5.GetConstBegin());

	ASSERT_EQ(test5.GetSize(), 5);
	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test5[i].mi, 3);
		ASSERT_EQ(test5[i].content, i + 1);
	}
	ASSERT_EQ(iter5->mi, 3);
	ASSERT_EQ(iter5->content, 1);

	Vector<test_vector_class> test6 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test6.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test6[i].mi, 1);
		ASSERT_EQ(test6[i].content, i);
	}

	auto iter6 = test6.Remove(test6.GetConstEnd() - 1);

	ASSERT_EQ(test6.GetSize(), 5);
	for (int i = 0; i < 5; i++)
	{
		ASSERT_EQ(test6[i].mi, 1);
		ASSERT_EQ(test6[i].content, i);
	}
	ASSERT_EQ(iter6, test6.GetConstEnd());

	Vector<test_vector_class> test7 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test7.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test7[i].mi, 1);
		ASSERT_EQ(test7[i].content, i);
	}

	auto iter7 = test7.Remove(test7.GetBegin() + 1, test7.GetEnd());

	ASSERT_EQ(test7.GetSize(), 1);
	ASSERT_EQ(test7[0].mi, 1);
	ASSERT_EQ(test7[0].content, 0);
	for (int i = 2; i < 6; i++)
	{
		ASSERT_EQ((&test7[0] + i)->content, i);
		ASSERT_EQ((&test7[0] + i)->mi, -1);
	}

	Vector<test_vector_class> test8 = {0, 1, 2, 3, 4, 5};

	ASSERT_EQ(test8.GetSize(), 6);
	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(test8[i].mi, 1);
		ASSERT_EQ(test8[i].content, i);
	}

	auto iter8 = test8.Remove(test8.GetConstBegin(), test8.GetConstEnd() - 1);

	ASSERT_EQ(test8.GetSize(), 1);
	ASSERT_EQ(test8[0].mi, 3);
	ASSERT_EQ(test8[0].content, 5);
	for (int i = 1; i < 5; i++)
	{
		ASSERT_EQ((&test8[0] + i)->content, i);
		ASSERT_EQ((&test8[0] + i)->mi, -1);
	}
}

TEST(VectorIterator, IsVectorIteratorTest)
{
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::Iterator>::Result == true));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<Vector<int>::ConstIterator>::Result == true));
	ASSERT_TRUE((Vector<int>::IsVectorIterator<int>::Result == false));
	ASSERT_TRUE((Vector<float>::IsVectorIterator<Vector<int>::Iterator>::Result == false));
	ASSERT_TRUE((Vector<float>::IsVectorIterator<Vector<int>::ConstIterator>::Result == false));
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
}

TEST(VectorIterator, OutOfRangeErrorTest)
{
	Vector<int> test = {0, 1, 2};
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin(), test.GetData(), test.GetData() + test.GetSize()) == false));
	ASSERT_TRUE((Vector<int>::Iterator::OutOfRangeError::Judge(test.GetBegin() - 1, test.GetData(), test.GetData() + test.GetSize()) == true));
	ASSERT_TRUE((Vector<int>::ConstIterator::OutOfRangeError::Judge(test.GetConstBegin(), test.GetData(), test.GetData() + test.GetSize()) == false));
	ASSERT_TRUE((Vector<int>::ConstIterator::OutOfRangeError::Judge(test.GetConstBegin() - 1, test.GetData(), test.GetData() + test.GetSize()) == true));
}