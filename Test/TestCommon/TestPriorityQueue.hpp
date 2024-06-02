/*
Copyright 2024 creatorlxd

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
#include <random>
#include "gtest/gtest.h"
#include "Container/PriorityQueue.hpp"

using namespace SpaceGameEngine;

struct test_priority_queue_object
{
	std::function<void(test_priority_queue_object&)> rel_func;
	test_priority_queue_object()
		: val(0), rel_func([](test_priority_queue_object&) {})
	{
	}
	test_priority_queue_object(int v)
		: val(v), rel_func([](test_priority_queue_object&) {})
	{
	}
	test_priority_queue_object(int v, const std::function<void(test_priority_queue_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_priority_queue_object(const test_priority_queue_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_priority_queue_object(test_priority_queue_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_priority_queue_object&) {};
	}
	~test_priority_queue_object()
	{
		rel_func(*this);
	}
	int val;

	test_priority_queue_object& operator=(const test_priority_queue_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_priority_queue_object& operator=(test_priority_queue_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_priority_queue_object&) {};
		return *this;
	}

	bool operator<(const test_priority_queue_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_priority_queue_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_priority_queue_object& o) const
	{
		return val != o.val;
	}
};

TEST(PriorityQueue, InstanceTest)
{
	PriorityQueue<int> pq1;
	PriorityQueue<int, Less<int>, StdAllocator> pq2({3, 2, 1, 8, 5, 7, -2, 3});
	const PriorityQueue<int, Greater<int>, DefaultAllocator> pq3(10, 1);
	Vector<int> vd1({3, 2, 1, 8, 5, 7, -2, 3});
	PriorityQueue<int> pq4(vd1.GetConstBegin(), vd1.GetConstEnd());
	PriorityQueue<int> pq5(vd1);
	PriorityQueue<int> pq6(std::move(vd1));
	Vector<int, StdAllocator> vd2({3, 2, 1, 8, 5, 7, -2, 3});
	PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq7(vd2);
	PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq8(std::move(vd2));

	ASSERT_EQ(pq1.GetSize(), 0);
	ASSERT_EQ(pq2.GetSize(), 8);
	ASSERT_EQ(pq3.GetSize(), 10);

	Vector<int, StdAllocator>& v2 = pq2.GetImplement();
	ASSERT_EQ(v2.GetSize(), 8);
	ASSERT_EQ(*v2.GetBegin(), -2);
	ASSERT_EQ(*(v2.GetBegin() + 1), 3);
	ASSERT_EQ(*(v2.GetBegin() + 2), 1);
	ASSERT_EQ(*(v2.GetBegin() + 3), 3);
	ASSERT_EQ(*(v2.GetBegin() + 4), 5);
	ASSERT_EQ(*(v2.GetBegin() + 5), 7);
	ASSERT_EQ(*(v2.GetBegin() + 6), 2);
	ASSERT_EQ(*(v2.GetBegin() + 7), 8);

	const Vector<int>& v3 = pq3.GetImplement();
	ASSERT_EQ(v3.GetSize(), 10);
	for (int i = 0; i < 10; ++i)
		ASSERT_EQ(v3[i], 1);

	Vector<int>& v4 = pq4.GetImplement();
	ASSERT_EQ(v4.GetSize(), 8);
	ASSERT_EQ(*v4.GetBegin(), -2);
	ASSERT_EQ(*(v4.GetBegin() + 1), 3);
	ASSERT_EQ(*(v4.GetBegin() + 2), 1);
	ASSERT_EQ(*(v4.GetBegin() + 3), 3);
	ASSERT_EQ(*(v4.GetBegin() + 4), 5);
	ASSERT_EQ(*(v4.GetBegin() + 5), 7);
	ASSERT_EQ(*(v4.GetBegin() + 6), 2);
	ASSERT_EQ(*(v4.GetBegin() + 7), 8);

	Vector<int>& v5 = pq5.GetImplement();
	ASSERT_EQ(v5.GetSize(), 8);
	ASSERT_EQ(*v5.GetBegin(), -2);
	ASSERT_EQ(*(v5.GetBegin() + 1), 3);
	ASSERT_EQ(*(v5.GetBegin() + 2), 1);
	ASSERT_EQ(*(v5.GetBegin() + 3), 3);
	ASSERT_EQ(*(v5.GetBegin() + 4), 5);
	ASSERT_EQ(*(v5.GetBegin() + 5), 7);
	ASSERT_EQ(*(v5.GetBegin() + 6), 2);
	ASSERT_EQ(*(v5.GetBegin() + 7), 8);

	Vector<int>& v6 = pq6.GetImplement();
	ASSERT_EQ(v6.GetSize(), 8);
	ASSERT_EQ(*v6.GetBegin(), -2);
	ASSERT_EQ(*(v6.GetBegin() + 1), 3);
	ASSERT_EQ(*(v6.GetBegin() + 2), 1);
	ASSERT_EQ(*(v6.GetBegin() + 3), 3);
	ASSERT_EQ(*(v6.GetBegin() + 4), 5);
	ASSERT_EQ(*(v6.GetBegin() + 5), 7);
	ASSERT_EQ(*(v6.GetBegin() + 6), 2);
	ASSERT_EQ(*(v6.GetBegin() + 7), 8);

	Vector<int, MemoryManagerAllocator>& v7 = pq7.GetImplement();
	ASSERT_EQ(v7.GetSize(), 8);
	ASSERT_EQ(*v7.GetBegin(), -2);
	ASSERT_EQ(*(v7.GetBegin() + 1), 3);
	ASSERT_EQ(*(v7.GetBegin() + 2), 1);
	ASSERT_EQ(*(v7.GetBegin() + 3), 3);
	ASSERT_EQ(*(v7.GetBegin() + 4), 5);
	ASSERT_EQ(*(v7.GetBegin() + 5), 7);
	ASSERT_EQ(*(v7.GetBegin() + 6), 2);
	ASSERT_EQ(*(v7.GetBegin() + 7), 8);

	Vector<int, MemoryManagerAllocator>& v8 = pq8.GetImplement();
	ASSERT_EQ(v8.GetSize(), 8);
	ASSERT_EQ(*v8.GetBegin(), -2);
	ASSERT_EQ(*(v8.GetBegin() + 1), 3);
	ASSERT_EQ(*(v8.GetBegin() + 2), 1);
	ASSERT_EQ(*(v8.GetBegin() + 3), 3);
	ASSERT_EQ(*(v8.GetBegin() + 4), 5);
	ASSERT_EQ(*(v8.GetBegin() + 5), 7);
	ASSERT_EQ(*(v8.GetBegin() + 6), 2);
	ASSERT_EQ(*(v8.GetBegin() + 7), 8);
}

TEST(PriorityQueue, PushAndPopTest)
{
	const int test_size = 1000;
	const int test_up_bound = 100;
	int val_cnt[test_up_bound];
	int val_pool[test_size];
	memset(val_cnt, 0, sizeof(val_cnt));
	memset(val_pool, 0, sizeof(val_pool));

	std::random_device rd;

	for (int i = 0; i < test_size; ++i)
		val_pool[i] = rd() % test_up_bound;

	PriorityQueue<int, Less<int>> pq1;
	ASSERT_EQ(pq1.GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
		pq1.Push(val_pool[i]);
	ASSERT_EQ(pq1.GetSize(), test_size);

	int buf = pq1.GetTop();
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_LE(buf, pq1.GetTop());
		val_cnt[pq1.GetTop()] += 1;
		pq1.Pop();
		ASSERT_EQ(pq1.GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq1.GetSize(), 0);

	PriorityQueue<int, Greater<int>> pq2;
	ASSERT_EQ(pq2.GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
		pq2.Push(val_pool[i]);
	ASSERT_EQ(pq2.GetSize(), test_size);

	buf = pq2.GetTop();
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_GE(buf, pq2.GetTop());
		val_cnt[pq2.GetTop()] -= 1;
		pq2.Pop();
		ASSERT_EQ(pq2.GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2.GetSize(), 0);

	for (int i = 0; i < test_up_bound; ++i)
		ASSERT_EQ(val_cnt[i], 0);
}

TEST(PriorityQueue, CopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object>* ppq = new PriorityQueue<test_priority_queue_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object>* ppq2 = new PriorityQueue<test_priority_queue_object>(*ppq);

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq->GetTop().val, i);
		ppq->Pop();
		ASSERT_EQ(ppq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(PriorityQueue, MoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object>* ppq = new PriorityQueue<test_priority_queue_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object>* ppq2 = new PriorityQueue<test_priority_queue_object>(std::move(*ppq));

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(PriorityQueue, CopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object>* ppq = new PriorityQueue<test_priority_queue_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object>* ppq2 = new PriorityQueue<test_priority_queue_object>();
	ASSERT_EQ(ppq2->GetSize(), 0);

	*ppq2 = *ppq;

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq->GetTop().val, i);
		ppq->Pop();
		ASSERT_EQ(ppq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(PriorityQueue, MoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object>* ppq = new PriorityQueue<test_priority_queue_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object>* ppq2 = new PriorityQueue<test_priority_queue_object>();
	ASSERT_EQ(ppq2->GetSize(), 0);

	*ppq2 = std::move(*ppq);

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(PriorityQueue, AnotherAllocatorCopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>* ppq = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq2 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>(*ppq);

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq->GetTop().val, i);
		ppq->Pop();
		ASSERT_EQ(ppq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(PriorityQueue, AnotherAllocatorMoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>* ppq = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq2 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>(std::move(*ppq));

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(PriorityQueue, AnotherAllocatorCopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>* ppq = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq2 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>();
	ASSERT_EQ(ppq2->GetSize(), 0);

	*ppq2 = *ppq;

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq->GetTop().val, i);
		ppq->Pop();
		ASSERT_EQ(ppq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(PriorityQueue, AnotherAllocatorMoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>* ppq = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq2 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>();
	ASSERT_EQ(ppq2->GetSize(), 0);

	*ppq2 = std::move(*ppq);

	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(ppq2->GetTop().val, i);
		ppq2->Pop();
		ASSERT_EQ(ppq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(ppq2->GetSize(), 0);

	delete ppq;
	delete ppq2;

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(PriorityQueue, EqualTest)
{
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq1({1, 2, 3});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq2({1, 2});
	const PriorityQueue<int, Less<int>, StdAllocator> pq2_({1, 2});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq3({1, 4, 3});
	const PriorityQueue<int, Less<int>, StdAllocator> pq3_({1, 4, 3});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq4({1, 2, 3});
	const PriorityQueue<int, Less<int>, StdAllocator> pq4_({1, 2, 3});

	ASSERT_FALSE(pq1 == pq2);
	ASSERT_FALSE(pq1 == pq2_);
	ASSERT_FALSE(pq1 == pq3);
	ASSERT_FALSE(pq1 == pq3_);
	ASSERT_TRUE(pq1 == pq4);
	ASSERT_TRUE(pq1 == pq4_);
}

TEST(PriorityQueue, NotEqualTest)
{
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq1({1, 2, 3});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq2({1, 2});
	const PriorityQueue<int, Less<int>, StdAllocator> pq2_({1, 2});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq3({1, 4, 3});
	const PriorityQueue<int, Less<int>, StdAllocator> pq3_({1, 4, 3});
	const PriorityQueue<int, Less<int>, MemoryManagerAllocator> pq4({1, 2, 3});
	const PriorityQueue<int, Less<int>, StdAllocator> pq4_({1, 2, 3});

	ASSERT_TRUE(pq1 != pq2);
	ASSERT_TRUE(pq1 != pq2_);
	ASSERT_TRUE(pq1 != pq3);
	ASSERT_TRUE(pq1 != pq3_);
	ASSERT_FALSE(pq1 != pq4);
	ASSERT_FALSE(pq1 != pq4_);
}

TEST(PriorityQueue, SwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object>* ppq = new PriorityQueue<test_priority_queue_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	int cnt = 0;
	for (auto iter = ppq->GetImplement().GetConstBegin(); iter != ppq->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	PriorityQueue<test_priority_queue_object>* ppq2 = new PriorityQueue<test_priority_queue_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		ppq2->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq2->GetTop().val, test_size);
	}
	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (auto iter = ppq2->GetImplement().GetConstBegin(); iter != ppq2->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, 2 * test_size);

	PriorityQueue<test_priority_queue_object>* ppq3 = new PriorityQueue<test_priority_queue_object>(std::move(*ppq));

	cnt = 0;
	for (auto iter = ppq3->GetImplement().GetConstBegin(); iter != ppq3->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	*ppq = std::move(*ppq2);

	for (auto iter = ppq->GetImplement().GetConstBegin(); iter != ppq->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, 2 * test_size);

	*ppq2 = std::move(*ppq3);

	cnt = 0;
	for (auto iter = ppq2->GetImplement().GetConstBegin(); iter != ppq2->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	delete ppq;
	delete ppq2;
	delete ppq3;

	for (int i = 0; i < 2 * test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(PriorityQueue, AnotherAllocatorSwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_priority_queue_object& o) {
		val_pool[o.val] += 1;
	};

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>();

	for (int i = 0; i < test_size; ++i)
	{
		ppq->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq->GetTop().val, 0);
	}
	ASSERT_EQ(ppq->GetSize(), test_size);

	int cnt = 0;
	for (auto iter = ppq->GetImplement().GetConstBegin(); iter != ppq->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>* ppq2 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		ppq2->Push(test_priority_queue_object(i, val_rel_func));
		ASSERT_EQ(ppq2->GetTop().val, test_size);
	}
	ASSERT_EQ(ppq2->GetSize(), test_size);

	for (auto iter = ppq2->GetImplement().GetConstBegin(); iter != ppq2->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, 2 * test_size);

	PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>* ppq3 = new PriorityQueue<test_priority_queue_object, Less<test_priority_queue_object>, MemoryManagerAllocator>(std::move(*ppq));

	cnt = 0;
	for (auto iter = ppq3->GetImplement().GetConstBegin(); iter != ppq3->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	*ppq = std::move(*ppq2);

	for (auto iter = ppq->GetImplement().GetConstBegin(); iter != ppq->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, 2 * test_size);

	*ppq2 = std::move(*ppq3);

	cnt = 0;
	for (auto iter = ppq2->GetImplement().GetConstBegin(); iter != ppq2->GetImplement().GetConstEnd(); ++iter)
	{
		ASSERT_EQ(*iter, cnt);
		++cnt;
	}
	ASSERT_EQ(cnt, test_size);

	delete ppq;
	delete ppq2;
	delete ppq3;

	for (int i = 0; i < 2 * test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}