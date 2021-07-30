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
#include "Container/Queue.hpp"

using namespace SpaceGameEngine;

struct test_queue_object
{
	std::function<void(test_queue_object&)> rel_func;
	test_queue_object()
		: val(0), rel_func([](test_queue_object&) {})
	{
	}
	test_queue_object(int v)
		: val(v), rel_func([](test_queue_object&) {})
	{
	}
	test_queue_object(int v, const std::function<void(test_queue_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_queue_object(const test_queue_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_queue_object(test_queue_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_queue_object&) {};
	}
	~test_queue_object()
	{
		rel_func(*this);
	}
	int val;

	test_queue_object& operator=(const test_queue_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_queue_object& operator=(test_queue_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_queue_object&) {};
		return *this;
	}

	bool operator<(const test_queue_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_queue_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_queue_object& o) const
	{
		return val != o.val;
	}
};

TEST(Queue, InstanceTest)
{
	Queue<int> q1;
	Queue<int, StdAllocator> q2({1, 2, 3});
	const Queue<int, DefaultAllocator> q3(10, 1);

	ASSERT_EQ(q1.GetSize(), 0);
	ASSERT_EQ(q2.GetSize(), 3);
	ASSERT_EQ(q3.GetSize(), 10);

	List<int, StdAllocator>& l2 = q2.GetImplement();
	ASSERT_EQ(l2.GetSize(), 3);
	ASSERT_EQ(*l2.GetBegin(), 1);
	ASSERT_EQ(*(l2.GetBegin() + 1), 2);
	ASSERT_EQ(*(l2.GetBegin() + 2), 3);

	const List<int>& v3 = q3.GetImplement();
	ASSERT_EQ(v3.GetSize(), 10);
	for (auto i = v3.GetConstBegin(); i != v3.GetConstEnd(); ++i)
		ASSERT_EQ(*i, 1);
}

TEST(Queue, PushAndPopTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object>* pq = new Queue<test_queue_object>();
	const Queue<test_queue_object>* pcq = pq;

	for (int i = 0; i < test_size; ++i)
	{
		test_queue_object tbuf(i, val_rel_func);
		pq->Push(tbuf);
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pcq->GetFront().val, i);
		pq->Pop();
		ASSERT_EQ(pq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		test_queue_object tbuf(i, val_rel_func);
		pq->Push(std::move(tbuf));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	delete pq;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Queue, CopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object>* pq = new Queue<test_queue_object>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object>* pq2 = new Queue<test_queue_object>(*pq);

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq->GetFront().val, i);
		pq->Pop();
		ASSERT_EQ(pq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Queue, MoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object>* pq = new Queue<test_queue_object>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object>* pq2 = new Queue<test_queue_object>(std::move(*pq));

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Queue, CopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object>* pq = new Queue<test_queue_object>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object>* pq2 = new Queue<test_queue_object>();

	ASSERT_EQ(pq2->GetSize(), 0);

	*pq2 = *pq;

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq->GetFront().val, i);
		pq->Pop();
		ASSERT_EQ(pq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Queue, MoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object>* pq = new Queue<test_queue_object>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object>* pq2 = new Queue<test_queue_object>();

	ASSERT_EQ(pq2->GetSize(), 0);

	*pq2 = std::move(*pq);

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Queue, AnotherAllocatorCopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object, StdAllocator>* pq = new Queue<test_queue_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object, MemoryManagerAllocator>* pq2 = new Queue<test_queue_object, MemoryManagerAllocator>(*pq);

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq->GetFront().val, i);
		pq->Pop();
		ASSERT_EQ(pq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Queue, AnotherAllocatorMoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object, StdAllocator>* pq = new Queue<test_queue_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object, MemoryManagerAllocator>* pq2 = new Queue<test_queue_object, MemoryManagerAllocator>(std::move(*pq));

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Queue, AnotherAllocatorCopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object, StdAllocator>* pq = new Queue<test_queue_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object, MemoryManagerAllocator>* pq2 = new Queue<test_queue_object, MemoryManagerAllocator>();

	ASSERT_EQ(pq2->GetSize(), 0);

	*pq2 = *pq;

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq->GetFront().val, i);
		pq->Pop();
		ASSERT_EQ(pq->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Queue, AnotherAllocatorMoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_queue_object& o) {
		val_pool[o.val] += 1;
	};

	Queue<test_queue_object, StdAllocator>* pq = new Queue<test_queue_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		pq->Push(test_queue_object(i, val_rel_func));
		ASSERT_EQ(pq->GetFront().val, 0);
	}
	ASSERT_EQ(pq->GetSize(), test_size);

	Queue<test_queue_object, MemoryManagerAllocator>* pq2 = new Queue<test_queue_object, MemoryManagerAllocator>();

	ASSERT_EQ(pq2->GetSize(), 0);

	*pq2 = std::move(*pq);

	ASSERT_EQ(pq2->GetSize(), test_size);

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pq2->GetFront().val, i);
		pq2->Pop();
		ASSERT_EQ(pq2->GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2->GetSize(), 0);

	delete pq;
	delete pq2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}
