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
#include "Container/Stack.hpp"

using namespace SpaceGameEngine;

struct test_stack_object
{
	std::function<void(test_stack_object&)> rel_func;
	test_stack_object()
		: val(0), rel_func([](test_stack_object&) {})
	{
	}
	test_stack_object(int v)
		: val(v), rel_func([](test_stack_object&) {})
	{
	}
	test_stack_object(int v, const std::function<void(test_stack_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_stack_object(const test_stack_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_stack_object(test_stack_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_stack_object&) {};
	}
	~test_stack_object()
	{
		rel_func(*this);
	}
	int val;

	test_stack_object& operator=(const test_stack_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_stack_object& operator=(test_stack_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_stack_object&) {};
		return *this;
	}

	bool operator<(const test_stack_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_stack_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_stack_object& o) const
	{
		return val != o.val;
	}
};

TEST(Stack, InstanceTest)
{
	Stack<int> s1;
	Stack<int, StdAllocator> s2({1, 2, 3});
	const Stack<int, DefaultAllocator, Vector> s3(10, 1);

	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(s3.GetSize(), 10);

	List<int, StdAllocator>& l2 = s2.GetImplement();
	ASSERT_EQ(l2.GetSize(), 3);
	ASSERT_EQ(*l2.GetBegin(), 1);
	ASSERT_EQ(*(l2.GetBegin() + 1), 2);
	ASSERT_EQ(*(l2.GetBegin() + 2), 3);

	const Vector<int>& v3 = s3.GetImplement();
	ASSERT_EQ(v3.GetSize(), 10);
	for (int i = 0; i < 10; ++i)
		ASSERT_EQ(v3[i], 1);
}

TEST(Stack, PushAndPopTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object>* ps = new Stack<test_stack_object>();
	const Stack<test_stack_object>* pcs = ps;

	for (int i = 0; i < test_size; ++i)
	{
		test_stack_object tbuf(i, val_rel_func);
		ps->Push(tbuf);
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(pcs->GetTop().val, i);
		ps->Pop();
		ASSERT_EQ(ps->GetSize(), i);
	}
	ASSERT_EQ(ps->GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
	{
		test_stack_object tbuf(i, val_rel_func);
		ps->Push(std::move(tbuf));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	delete ps;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Stack, CopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object>* ps = new Stack<test_stack_object>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object>* ps2 = new Stack<test_stack_object>(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps->GetTop().val, i);
		ps->Pop();
		ASSERT_EQ(ps->GetSize(), i);
	}
	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Stack, MoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object>* ps = new Stack<test_stack_object>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object>* ps2 = new Stack<test_stack_object>(std::move(*ps));

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Stack, CopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object>* ps = new Stack<test_stack_object>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object>* ps2 = new Stack<test_stack_object>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = *ps;

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps->GetTop().val, i);
		ps->Pop();
		ASSERT_EQ(ps->GetSize(), i);
	}
	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Stack, MoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object>* ps = new Stack<test_stack_object>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object>* ps2 = new Stack<test_stack_object>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = std::move(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Stack, AnotherAllocatorCopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object, StdAllocator>* ps = new Stack<test_stack_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object, MemoryManagerAllocator>* ps2 = new Stack<test_stack_object, MemoryManagerAllocator>(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps->GetTop().val, i);
		ps->Pop();
		ASSERT_EQ(ps->GetSize(), i);
	}
	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Stack, AnotherAllocatorMoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object, StdAllocator>* ps = new Stack<test_stack_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object, MemoryManagerAllocator>* ps2 = new Stack<test_stack_object, MemoryManagerAllocator>(std::move(*ps));

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Stack, AnotherAllocatorCopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object, StdAllocator>* ps = new Stack<test_stack_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object, MemoryManagerAllocator>* ps2 = new Stack<test_stack_object, MemoryManagerAllocator>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = *ps;

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps->GetTop().val, i);
		ps->Pop();
		ASSERT_EQ(ps->GetSize(), i);
	}
	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Stack, AnotherAllocatorMoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_stack_object& o) {
		val_pool[o.val] += 1;
	};

	Stack<test_stack_object, StdAllocator>* ps = new Stack<test_stack_object, StdAllocator>();
	for (int i = 0; i < test_size; ++i)
	{
		ps->Push(test_stack_object(i, val_rel_func));
		ASSERT_EQ(ps->GetTop().val, i);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	Stack<test_stack_object, MemoryManagerAllocator>* ps2 = new Stack<test_stack_object, MemoryManagerAllocator>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = std::move(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; --i)
	{
		ASSERT_EQ(ps2->GetTop().val, i);
		ps2->Pop();
		ASSERT_EQ(ps2->GetSize(), i);
	}
	ASSERT_EQ(ps2->GetSize(), 0);

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}
