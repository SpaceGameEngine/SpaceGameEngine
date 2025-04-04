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
#include "gtest/gtest.h"
#include "Container/Map.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_map_object
{
	std::function<void(test_map_object&)> rel_func;
	test_map_object()
		: val(0), rel_func([](test_map_object&) {})
	{
	}
	test_map_object(int v)
		: val(v), rel_func([](test_map_object&) {})
	{
	}
	test_map_object(int v, const std::function<void(test_map_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_map_object(const test_map_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_map_object(test_map_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_map_object&) {};
	}
	~test_map_object()
	{
		rel_func(*this);
	}
	int val;

	test_map_object& operator=(const test_map_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_map_object& operator=(test_map_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_map_object&) {};
		return *this;
	}

	bool operator<(const test_map_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_map_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_map_object& o) const
	{
		return val != o.val;
	}
};

TEST(Map, InitializerListConstructionTest)
{
	const int test_size = 10;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};

	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>(
		{Pair<const test_map_object, test_map_object>(test_map_object(0, key_rel_func), test_map_object(0, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(1, key_rel_func), test_map_object(1, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(2, key_rel_func), test_map_object(2, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(3, key_rel_func), test_map_object(3, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(4, key_rel_func), test_map_object(4, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(5, key_rel_func), test_map_object(5, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(6, key_rel_func), test_map_object(6, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(7, key_rel_func), test_map_object(7, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(8, key_rel_func), test_map_object(8, val_rel_func)),
		 Pair<const test_map_object, test_map_object>(test_map_object(9, key_rel_func), test_map_object(9, val_rel_func))});

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}
	delete pm;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Map, ClearTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	pm->Clear();

	ASSERT_EQ(pm->GetSize(), 0);
	ASSERT_EQ(pm->GetBegin().GetData(), pm->GetEnd().GetData());
	ASSERT_EQ(pm->GetConstBegin().GetData(), pm->GetConstEnd().GetData());
	ASSERT_EQ(pm->GetReverseBegin().GetData(), pm->GetReverseEnd().GetData());
	ASSERT_EQ(pm->GetConstReverseBegin().GetData(), pm->GetConstReverseEnd().GetData());

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}

	delete pm;
}

TEST(Map, InsertTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(test_size - 1 - i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_FALSE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Map, UpsertTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Upsert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Upsert(test_map_object(i, key_rel_func), test_map_object(test_size - 1 - i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, test_size - 1 - i);
		ASSERT_FALSE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, InsertListTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 10;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};

	// test repeat insert
	pm->Insert(test_map_object(0, key_rel_func), test_map_object(-1));
	ASSERT_EQ(pm->GetSize(), 1);
	ASSERT_EQ((*pm)[test_map_object(0)].val, -1);

	// have 11 elements, test repeat insert
	pm->Insert({Pair<const test_map_object, test_map_object>(test_map_object(0, key_rel_func), test_map_object(0, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(1, key_rel_func), test_map_object(0)),
				Pair<const test_map_object, test_map_object>(test_map_object(1), test_map_object(1, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(2, key_rel_func), test_map_object(2, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(3, key_rel_func), test_map_object(3, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(4, key_rel_func), test_map_object(4, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(5, key_rel_func), test_map_object(5, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(6, key_rel_func), test_map_object(6, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(7, key_rel_func), test_map_object(7, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(8, key_rel_func), test_map_object(8, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(9, key_rel_func), test_map_object(9, val_rel_func))});

	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 2; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}
	ASSERT_EQ((*pm)[test_map_object(0)].val, -1);
	ASSERT_EQ((*pm)[test_map_object(1)].val, 0);
	delete pm;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], i > 1 ? 2 : 1);
	}
}

TEST(Map, UpsertListTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 10;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};

	// test repeat insert
	pm->Upsert(test_map_object(0, key_rel_func), test_map_object(-1));
	ASSERT_EQ(pm->GetSize(), 1);
	ASSERT_EQ((*pm)[test_map_object(0)].val, -1);

	// have 11 elements, test repeat insert
	pm->Upsert({Pair<const test_map_object, test_map_object>(test_map_object(0, key_rel_func), test_map_object(0, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(1, key_rel_func), test_map_object(0)),
				Pair<const test_map_object, test_map_object>(test_map_object(1), test_map_object(1, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(2, key_rel_func), test_map_object(2, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(3, key_rel_func), test_map_object(3, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(4, key_rel_func), test_map_object(4, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(5, key_rel_func), test_map_object(5, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(6, key_rel_func), test_map_object(6, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(7, key_rel_func), test_map_object(7, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(8, key_rel_func), test_map_object(8, val_rel_func)),
				Pair<const test_map_object, test_map_object>(test_map_object(9, key_rel_func), test_map_object(9, val_rel_func))});

	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}
	delete pm;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Map, RemoveTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	std::vector<Map<test_map_object, test_map_object>::Iterator> iter_buf;
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
		if (i >= 4 && i < 8)
			iter_buf.push_back(re.m_First);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	ASSERT_EQ(iter_buf.size(), 4);

	auto iter1 = pm->Remove(Map<test_map_object, test_map_object>::Iterator(iter_buf[0]));
	ASSERT_TRUE((std::is_same_v<decltype(iter1), Map<test_map_object, test_map_object>::Iterator>));
	ASSERT_EQ(iter1->m_First.val, 5);
	ASSERT_EQ(iter1->m_Second.val, 5);
	ASSERT_EQ(pm->GetSize(), test_size - 1);

	auto iter2 = pm->Remove(Map<test_map_object, test_map_object>::ConstIterator(iter_buf[1]));
	ASSERT_TRUE((std::is_same_v<decltype(iter2), Map<test_map_object, test_map_object>::ConstIterator>));
	ASSERT_EQ(iter2->m_First.val, 6);
	ASSERT_EQ(iter2->m_Second.val, 6);
	ASSERT_EQ(pm->GetSize(), test_size - 2);

	auto iter3 = pm->Remove(Map<test_map_object, test_map_object>::ReverseIterator(iter_buf[2]));
	ASSERT_TRUE((std::is_same_v<decltype(iter3), Map<test_map_object, test_map_object>::ReverseIterator>));
	ASSERT_EQ(iter3->m_First.val, 3);
	ASSERT_EQ(iter3->m_Second.val, 3);
	ASSERT_EQ(pm->GetSize(), test_size - 3);

	auto iter4 = pm->Remove(Map<test_map_object, test_map_object>::ConstReverseIterator(iter_buf[3]));
	ASSERT_TRUE((std::is_same_v<decltype(iter4), Map<test_map_object, test_map_object>::ConstReverseIterator>));
	ASSERT_EQ(iter4->m_First.val, 3);
	ASSERT_EQ(iter4->m_Second.val, 3);
	ASSERT_EQ(pm->GetSize(), test_size - 4);

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, RemoveByKeyTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(pm->RemoveByKey(test_map_object(i)));
	}

	ASSERT_EQ(pm->GetSize(), 0);

	delete pm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, FindTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pm->Find(test_map_object(i));
		ASSERT_TRUE((std::is_same_v<decltype(iter), Map<test_map_object, test_map_object>::Iterator>));
		ASSERT_EQ(iter->m_First.val, i);
		ASSERT_EQ(iter->m_Second.val, i);
	}
	ASSERT_EQ(pm->Find(test_map_object(test_size)), pm->GetEnd());
	ASSERT_EQ(pm->GetSize(), test_size);

	const Map<test_map_object, test_map_object>* pcm = pm;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pcm->Find(test_map_object(i));
		ASSERT_TRUE((std::is_same_v<decltype(iter), Map<test_map_object, test_map_object>::ConstIterator>));
		ASSERT_EQ(iter->m_First.val, i);
		ASSERT_EQ(iter->m_Second.val, i);
	}
	ASSERT_EQ(pcm->Find(test_map_object(test_size)), pm->GetEnd());
	ASSERT_EQ(pcm->GetSize(), test_size);

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, GetTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->m_First.val, i);
		ASSERT_EQ(re.m_First->m_Second.val, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm->Get(test_map_object(i)).val, i);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	const Map<test_map_object, test_map_object>* pcm = pm;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pcm->Get(test_map_object(i)).val, i);
	}
	ASSERT_EQ(pcm->GetSize(), test_size);

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, OperatorTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();

	for (int i = 0; i < test_size; i++)
	{
		(*pm)[test_map_object(i, key_rel_func)] = test_map_object(i, val_rel_func);
	}

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pm->Find(test_map_object(i));
		ASSERT_EQ(iter->m_First.val, i);
		ASSERT_EQ(iter->m_Second.val, i);
	}

	delete pm;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, ForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Map<test_map_object, test_map_object> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(test_map_object(i), test_map_object(1));
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	int last_key = 0;
	m1.ForEach([&last_key](Pair<const test_map_object, test_map_object>& val) {
		ASSERT_EQ(val.m_First.val, last_key);
		ASSERT_EQ(val.m_Second.val, 1);
		val.m_Second.val = val.m_First.val;
		++last_key;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m1.Find(test_map_object(i))->m_Second.val, i);
	}

	const Map<test_map_object, test_map_object> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(test_map_object(i))->m_Second.val, i);
	}

	last_key = 0;
	m2.ForEach([&](const Pair<const test_map_object, test_map_object>& val) {
		ASSERT_EQ(val.m_First.val, last_key);
		ASSERT_EQ(val.m_First.val, val.m_Second.val);
		test_cnt[val.m_First.val] += 1;
		++last_key;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(test_map_object(i))->m_Second.val, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(Map, ReverseForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Map<test_map_object, test_map_object> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(test_map_object(i), test_map_object(1));
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	int last_key = test_size - 1;
	m1.ReverseForEach([&last_key](Pair<const test_map_object, test_map_object>& val) {
		ASSERT_EQ(val.m_First.val, last_key);
		ASSERT_EQ(val.m_Second.val, 1);
		val.m_Second.val = val.m_First.val;
		--last_key;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m1.Find(test_map_object(i))->m_Second.val, i);
	}

	const Map<test_map_object, test_map_object> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(test_map_object(i))->m_Second.val, i);
	}

	last_key = test_size - 1;
	m2.ReverseForEach([&](const Pair<const test_map_object, test_map_object>& val) {
		ASSERT_EQ(val.m_First.val, last_key);
		ASSERT_EQ(val.m_First.val, val.m_Second.val);
		test_cnt[val.m_First.val] += 1;
		--last_key;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(test_map_object(i))->m_Second.val, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(Map, CopyConstructionTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm2 = new Map<test_map_object, test_map_object>(*pm);

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Map, MoveConstructionTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm2 = new Map<test_map_object, test_map_object>(std::move(*pm));

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, CopyAssignmentTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm2 = new Map<test_map_object, test_map_object>();

	ASSERT_EQ(pm2->GetSize(), 0);

	*pm2 = *pm;

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Map, MoveAssignmentTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm2 = new Map<test_map_object, test_map_object>();

	ASSERT_EQ(pm2->GetSize(), 0);

	*pm2 = std::move(*pm);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, AnotherAllocatorCopyConstructionTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>(*pm);

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm3 = new Map<test_map_object, test_map_object>(*pm2);

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm3)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	delete pm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Map, AnotherAllocatorMoveConstructionTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>(std::move(*pm));

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm3 = new Map<test_map_object, test_map_object>(std::move(*pm2));

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm3)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	delete pm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, AnotherAllocatorCopyAssignmentTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>();

	ASSERT_EQ(pm2->GetSize(), 0);

	*pm2 = *pm;

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm3 = new Map<test_map_object, test_map_object>();

	ASSERT_EQ(pm3->GetSize(), 0);

	*pm3 = *pm2;

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm3)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	delete pm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Map, AnotherAllocatorMoveAssignmentTest)
{
	Map<test_map_object, test_map_object>* pm = new Map<test_map_object, test_map_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pm->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>();

	ASSERT_EQ(pm2->GetSize(), 0);

	*pm2 = std::move(*pm);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm2)[test_map_object(i)].val, i);
	}

	Map<test_map_object, test_map_object>* pm3 = new Map<test_map_object, test_map_object>();

	ASSERT_EQ(pm3->GetSize(), 0);

	*pm3 = std::move(*pm2);

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm3)[test_map_object(i)].val, i);
	}

	delete pm;
	delete pm2;
	delete pm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, EqualTest)
{
	const Map<int, int, Less<int>, MemoryManagerAllocator> m1({{1, 10},
															   {2, 20},
															   {3, 30}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m2({{1, 10},
															   {2, 20}});
	const Map<int, int, Less<int>, StdAllocator> m2_({{1, 10},
													  {2, 20}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m3({{1, 10},
															   {2, 21},
															   {3, 30}});
	const Map<int, int, Less<int>, StdAllocator> m3_({{1, 10},
													  {2, 21},
													  {3, 30}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m4({{1, 10},
															   {2, 20},
															   {3, 30}});
	const Map<int, int, Less<int>, StdAllocator> m4_({{1, 10},
													  {2, 20},
													  {3, 30}});

	ASSERT_FALSE(m1 == m2);
	ASSERT_FALSE(m1 == m2_);
	ASSERT_FALSE(m1 == m3);
	ASSERT_FALSE(m1 == m3_);
	ASSERT_TRUE(m1 == m4);
	ASSERT_TRUE(m1 == m4_);
}

TEST(Map, NotEqualTest)
{
	const Map<int, int, Less<int>, MemoryManagerAllocator> m1({{1, 10},
															   {2, 20},
															   {3, 30}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m2({{1, 10},
															   {2, 20}});
	const Map<int, int, Less<int>, StdAllocator> m2_({{1, 10},
													  {2, 20}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m3({{1, 10},
															   {2, 21},
															   {3, 30}});
	const Map<int, int, Less<int>, StdAllocator> m3_({{1, 10},
													  {2, 21},
													  {3, 30}});
	const Map<int, int, Less<int>, MemoryManagerAllocator> m4({{1, 10},
															   {2, 20},
															   {3, 30}});
	const Map<int, int, Less<int>, StdAllocator> m4_({{1, 10},
													  {2, 20},
													  {3, 30}});

	ASSERT_TRUE(m1 != m2);
	ASSERT_TRUE(m1 != m2_);
	ASSERT_TRUE(m1 != m3);
	ASSERT_TRUE(m1 != m3_);
	ASSERT_FALSE(m1 != m4);
	ASSERT_FALSE(m1 != m4_);
}

TEST(Map, SwapTest)
{
	const int test_size = 1000;
	int key_pool[2 * test_size];
	int val_pool[2 * test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object>* pm1 = new Map<test_map_object, test_map_object>();
	Map<test_map_object, test_map_object>* pm2 = new Map<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_map_object(i))->m_Second.val, i);
	}

	for (int i = test_size; i < 2 * test_size; i++)
	{
		pm2->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm2->Find(test_map_object(i))->m_Second.val, i);
	}

	Map<test_map_object, test_map_object>* pm3 = new Map<test_map_object, test_map_object>(std::move(*pm1));

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm3->Find(test_map_object(i))->m_Second.val, i);
	}

	*pm1 = std::move(*pm2);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm1->Find(test_map_object(i))->m_Second.val, i);
	}

	*pm2 = std::move(*pm3);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_map_object(i))->m_Second.val, i);
	}

	delete pm1;
	delete pm2;
	delete pm3;

	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, AnotherAllocatorSwapTest)
{
	const int test_size = 1000;
	int key_pool[2 * test_size];
	int val_pool[2 * test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	Map<test_map_object, test_map_object, Less<test_map_object>, MemoryManagerAllocator>* pm1 = new Map<test_map_object, test_map_object, Less<test_map_object>, MemoryManagerAllocator>();
	Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new Map<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_map_object(i))->m_Second.val, i);
	}

	for (int i = test_size; i < 2 * test_size; i++)
	{
		pm2->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm2->Find(test_map_object(i))->m_Second.val, i);
	}

	Map<test_map_object, test_map_object, Less<test_map_object>, MemoryManagerAllocator>* pm3 = new Map<test_map_object, test_map_object, Less<test_map_object>, MemoryManagerAllocator>(std::move(*pm1));

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm3->Find(test_map_object(i))->m_Second.val, i);
	}

	*pm1 = std::move(*pm2);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm1->Find(test_map_object(i))->m_Second.val, i);
	}

	*pm2 = std::move(*pm3);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_map_object(i))->m_Second.val, i);
	}

	delete pm1;
	delete pm2;
	delete pm3;

	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Map, ConstructByEmptyListTest)
{
	Map<int, int> m({});
	ASSERT_EQ(m.GetSize(), 0);

	m.Insert(1, 2);
	ASSERT_EQ(m.GetSize(), 1);
	ASSERT_EQ(m[1], 2);
}

TEST(Map, InsertEmptyListTest)
{
	Map<int, int> m({{1, 2}});
	ASSERT_EQ(m.GetSize(), 1);
	ASSERT_EQ(m[1], 2);

	m.Insert(std::initializer_list<Pair<const int, int>>{});
	ASSERT_EQ(m.GetSize(), 1);
	ASSERT_EQ(m[1], 2);
}

TEST(MapIterator, GetBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::Iterator>));
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::Iterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetConstBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstIterator>));
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetConstEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetReverseBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ReverseIterator>));
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetReverseEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ReverseIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetConstReverseBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstReverseIterator>));
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetConstReverseEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstReverseIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, IteratorTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0},
						{4, 4.0},
						{5, 5.0},
						{6, 6.0},
						{7, 7.0},
						{8, 8.0},
						{9, 9.0},
						{10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 1;
	for (auto i = m.GetBegin(); i != m.GetEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt++;
	}

	auto iter = m.GetBegin();
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
	--iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		--iter;
	}
	++iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, 0);
		++iter;
	}
}

TEST(MapIterator, ConstIteratorTest)
{
	const Map<int, double> m({{1, 1.0},
							  {2, 2.0},
							  {3, 3.0},
							  {4, 4.0},
							  {5, 5.0},
							  {6, 6.0},
							  {7, 7.0},
							  {8, 8.0},
							  {9, 9.0},
							  {10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 1;
	for (auto i = m.GetConstBegin(); i != m.GetConstEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt++;
	}

	auto iter = m.GetConstBegin();
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
	--iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		--iter;
	}
	++iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, (double)i);
		++iter;
	}
}

TEST(MapIterator, ReverseIteratorTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0},
						{4, 4.0},
						{5, 5.0},
						{6, 6.0},
						{7, 7.0},
						{8, 8.0},
						{9, 9.0},
						{10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 10;
	for (auto i = m.GetReverseBegin(); i != m.GetReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt--;
	}

	auto iter = m.GetReverseBegin();
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
	--iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		--iter;
	}
	++iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, 0);
		++iter;
	}
}

TEST(MapIterator, ConstReverseIteratorTest)
{
	const Map<int, double> m({{1, 1.0},
							  {2, 2.0},
							  {3, 3.0},
							  {4, 4.0},
							  {5, 5.0},
							  {6, 6.0},
							  {7, 7.0},
							  {8, 8.0},
							  {9, 9.0},
							  {10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 10;
	for (auto i = m.GetConstReverseBegin(); i != m.GetConstReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt--;
	}

	auto iter = m.GetConstReverseBegin();
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
	--iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		--iter;
	}
	++iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, (double)i);
		++iter;
	}
}

TEST(MapIterator, PlusTest)
{
	Map<int, int> m = {
		{0, 0},
		{1, 1},
		{2, 2},
		{3, 3},
		{4, 4}};
	auto iter1_1 = m.GetBegin() + 1;
	ASSERT_EQ(iter1_1->m_First, 1);
	ASSERT_EQ(iter1_1->m_Second, 1);
	auto iter1_2 = m.GetBegin() + 2;
	ASSERT_EQ(iter1_2->m_First, 2);
	ASSERT_EQ(iter1_2->m_Second, 2);

	auto iter3_1 = m.GetReverseBegin() + 1;
	ASSERT_EQ(iter3_1->m_First, 3);
	ASSERT_EQ(iter3_1->m_Second, 3);
	auto iter3_2 = m.GetReverseBegin() + 2;
	ASSERT_EQ(iter3_2->m_First, 2);
	ASSERT_EQ(iter3_2->m_Second, 2);

	const Map<int, int>& cm = m;
	auto iter2_1 = cm.GetConstBegin() + 1;
	ASSERT_EQ(iter2_1->m_First, 1);
	ASSERT_EQ(iter2_1->m_Second, 1);
	auto iter2_2 = cm.GetConstBegin() + 2;
	ASSERT_EQ(iter2_2->m_First, 2);
	ASSERT_EQ(iter2_2->m_Second, 2);

	auto iter4_1 = cm.GetConstReverseBegin() + 1;
	ASSERT_EQ(iter4_1->m_First, 3);
	ASSERT_EQ(iter4_1->m_Second, 3);
	auto iter4_2 = cm.GetConstReverseBegin() + 2;
	ASSERT_EQ(iter4_2->m_First, 2);
	ASSERT_EQ(iter4_2->m_Second, 2);
}

TEST(MapIterator, SubtractTest)
{
	Map<int, int> m = {
		{0, 0},
		{1, 1},
		{2, 2},
		{3, 3},
		{4, 4}};
	auto iter1_1 = m.GetEnd() - 1;
	ASSERT_EQ(iter1_1->m_First, 4);
	ASSERT_EQ(iter1_1->m_Second, 4);
	auto iter1_2 = m.GetEnd() - 2;
	ASSERT_EQ(iter1_2->m_First, 3);
	ASSERT_EQ(iter1_2->m_Second, 3);

	auto iter3_1 = m.GetReverseEnd() - 1;
	ASSERT_EQ(iter3_1->m_First, 0);
	ASSERT_EQ(iter3_1->m_Second, 0);
	auto iter3_2 = m.GetReverseEnd() - 2;
	ASSERT_EQ(iter3_2->m_First, 1);
	ASSERT_EQ(iter3_2->m_Second, 1);

	const Map<int, int>& cm = m;
	auto iter2_1 = cm.GetConstEnd() - 1;
	ASSERT_EQ(iter2_1->m_First, 4);
	ASSERT_EQ(iter2_1->m_Second, 4);
	auto iter2_2 = cm.GetConstEnd() - 2;
	ASSERT_EQ(iter2_2->m_First, 3);
	ASSERT_EQ(iter2_2->m_Second, 3);

	auto iter4_1 = cm.GetConstReverseEnd() - 1;
	ASSERT_EQ(iter4_1->m_First, 0);
	ASSERT_EQ(iter4_1->m_Second, 0);
	auto iter4_2 = cm.GetConstReverseEnd() - 2;
	ASSERT_EQ(iter4_2->m_First, 1);
	ASSERT_EQ(iter4_2->m_Second, 1);
}

TEST(MapIterator, SelfIncreaseTest)
{
	Map<int, int> m = {
		{0, 0},
		{1, 1},
		{2, 2},
		{3, 3},
		{4, 4}};
	auto iter1 = m.GetBegin();
	ASSERT_EQ(iter1->m_First, 0);
	ASSERT_EQ(iter1->m_Second, 0);
	iter1 += 2;
	ASSERT_EQ(iter1->m_First, 2);
	ASSERT_EQ(iter1->m_Second, 2);

	auto iter3 = m.GetReverseBegin();
	ASSERT_EQ(iter3->m_First, 4);
	ASSERT_EQ(iter3->m_Second, 4);
	iter3 += 2;
	ASSERT_EQ(iter3->m_First, 2);
	ASSERT_EQ(iter3->m_Second, 2);

	const Map<int, int>& cm = m;
	auto iter2 = cm.GetConstBegin();
	ASSERT_EQ(iter2->m_First, 0);
	ASSERT_EQ(iter2->m_Second, 0);
	iter2 += 2;
	ASSERT_EQ(iter2->m_First, 2);
	ASSERT_EQ(iter2->m_Second, 2);

	auto iter4 = cm.GetConstReverseBegin();
	ASSERT_EQ(iter4->m_First, 4);
	ASSERT_EQ(iter4->m_Second, 4);
	iter4 += 2;
	ASSERT_EQ(iter4->m_First, 2);
	ASSERT_EQ(iter4->m_Second, 2);
}

TEST(MapIterator, SelfDecreaseTest)
{
	Map<int, int> m = {
		{0, 0},
		{1, 1},
		{2, 2},
		{3, 3},
		{4, 4}};
	auto iter1 = m.GetEnd();
	iter1 -= 2;
	ASSERT_EQ(iter1->m_First, 3);
	ASSERT_EQ(iter1->m_Second, 3);

	auto iter3 = m.GetReverseEnd();
	iter3 -= 2;
	ASSERT_EQ(iter3->m_First, 1);
	ASSERT_EQ(iter3->m_Second, 1);

	const Map<int, int>& cm = m;
	auto iter2 = cm.GetConstEnd();
	iter2 -= 2;
	ASSERT_EQ(iter2->m_First, 3);
	ASSERT_EQ(iter2->m_Second, 3);

	auto iter4 = cm.GetConstReverseEnd();
	iter4 -= 2;
	ASSERT_EQ(iter4->m_First, 1);
	ASSERT_EQ(iter4->m_Second, 1);
}

TEST(MapIterator, DistanceTest)
{
	Map<int, int> m = {
		{0, 0},
		{1, 1},
		{2, 2},
		{3, 3},
		{4, 4}};
	ASSERT_EQ(m.GetSize(), 5);
	ASSERT_EQ(m.GetEnd() - m.GetBegin(), 5);
	ASSERT_EQ(m.GetReverseEnd() - m.GetReverseBegin(), 5);

	const Map<int, int>& cm = m;
	ASSERT_EQ(cm.GetSize(), 5);
	ASSERT_EQ(cm.GetConstEnd() - cm.GetConstBegin(), 5);
	ASSERT_EQ(cm.GetConstReverseEnd() - cm.GetConstReverseBegin(), 5);
}