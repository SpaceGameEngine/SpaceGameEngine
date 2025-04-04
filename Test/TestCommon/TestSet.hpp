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
#include "Container/Set.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_set_object
{
	std::function<void(test_set_object&)> rel_func;
	test_set_object()
		: key(0), rel_func([](test_set_object&) {})
	{
	}
	test_set_object(int k, int v = 0)
		: key(k), value(v), rel_func([](test_set_object&) {})
	{
	}
	test_set_object(int k, const std::function<void(test_set_object&)>& func)
		: key(k), value(0), rel_func(func)
	{
	}
	test_set_object(int k, int v, const std::function<void(test_set_object&)>& func)
		: key(k), value(v), rel_func(func)
	{
	}
	test_set_object(const test_set_object& o) noexcept
		: key(o.key), value(o.value), rel_func(o.rel_func)
	{
	}
	test_set_object(test_set_object&& o) noexcept
		: key(o.key), value(o.value), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_set_object&) {};
	}
	~test_set_object()
	{
		rel_func(*this);
	}
	int key;
	int value;

	test_set_object& operator=(const test_set_object& o)
	{
		key = o.key;
		value = o.value;
		rel_func = o.rel_func;
		return *this;
	}

	test_set_object& operator=(test_set_object&& o)
	{
		key = o.key;
		value = o.value;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_set_object&) {};
		return *this;
	}

	bool operator<(const test_set_object& o) const
	{
		return key < o.key;
	}

	// only compare key here
	bool operator==(const test_set_object& o) const
	{
		return key == o.key && value == o.value;
	}

	// only compare key here
	bool operator!=(const test_set_object& o) const
	{
		return key != o.key || value != o.value;
	}
};

template<>
struct SpaceGameEngine::Equal<test_set_object>
{
	inline static constexpr bool Compare(const test_set_object& lhs, const test_set_object& rhs)
	{
		return lhs.key == rhs.key;
	}
};

bool operator==(const test_set_object& o, int val)
{
	return o.value == val;
}

bool operator==(int val, const test_set_object& o)
{
	return o.value == val;
}

TEST(Set, InitializerListConstructionTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};

	Set<test_set_object>* ps = new Set<test_set_object>(
		{test_set_object(0, val_rel_func),
		 test_set_object(1, val_rel_func),
		 test_set_object(2, val_rel_func),
		 test_set_object(3, val_rel_func),
		 test_set_object(4, val_rel_func),
		 test_set_object(5, val_rel_func),
		 test_set_object(6, val_rel_func),
		 test_set_object(7, val_rel_func),
		 test_set_object(8, val_rel_func),
		 test_set_object(9, val_rel_func)});

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}
	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Set, ClearTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	Set<test_set_object>* ps = new Set<test_set_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	ps->Clear();

	ASSERT_EQ(ps->GetSize(), 0);
	ASSERT_EQ(ps->GetBegin().GetData(), ps->GetEnd().GetData());
	ASSERT_EQ(ps->GetConstBegin().GetData(), ps->GetConstEnd().GetData());
	ASSERT_EQ(ps->GetReverseBegin().GetData(), ps->GetReverseEnd().GetData());
	ASSERT_EQ(ps->GetConstReverseBegin().GetData(), ps->GetConstReverseEnd().GetData());

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	delete ps;
}

TEST(Set, InsertTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
		auto iter = ps->Insert(test_set_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Set, UpsertTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Upsert(test_set_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
		auto iter = ps->Upsert(test_set_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, test_size - 1 - i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, InsertListTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	ps->Insert({test_set_object(0, -1)});
	ASSERT_EQ(ps->GetSize(), 1);
	ASSERT_EQ(ps->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	ps->Insert({test_set_object(0, 0, val_rel_func),
				test_set_object(1, -1),
				test_set_object(1, 1, val_rel_func),
				test_set_object(2, 2, val_rel_func),
				test_set_object(3, 3, val_rel_func),
				test_set_object(4, 4, val_rel_func),
				test_set_object(5, 5, val_rel_func),
				test_set_object(6, 6, val_rel_func),
				test_set_object(7, 7, val_rel_func),
				test_set_object(8, 8, val_rel_func),
				test_set_object(9, 9, val_rel_func)});

	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 2; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}
	ASSERT_EQ((*ps)[test_set_object(0)].value, -1);
	ASSERT_EQ((*ps)[test_set_object(1)].value, -1);
	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], i > 1 ? 2 : 1);
	}
}

TEST(Set, UpsertListTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	ps->Upsert({test_set_object(0, -1)});
	ASSERT_EQ(ps->GetSize(), 1);
	ASSERT_EQ(ps->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	ps->Upsert({test_set_object(0, 0, val_rel_func),
				test_set_object(1, -1),
				test_set_object(1, 1, val_rel_func),
				test_set_object(2, 2, val_rel_func),
				test_set_object(3, 3, val_rel_func),
				test_set_object(4, 4, val_rel_func),
				test_set_object(5, 5, val_rel_func),
				test_set_object(6, 6, val_rel_func),
				test_set_object(7, 7, val_rel_func),
				test_set_object(8, 8, val_rel_func),
				test_set_object(9, 9, val_rel_func)});

	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Set, RemoveTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	int rm_cnt = 0;
	auto iter = ps->GetBegin();
	while (iter != ps->GetEnd())
	{
		iter = ps->Remove(iter);
		rm_cnt += 1;
	}
	ASSERT_EQ(rm_cnt, test_size);
	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, RemoveByValueTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(ps->RemoveByValue(test_set_object(i)));
	}

	ASSERT_EQ(ps->GetSize(), 0);

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, FindTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = ps->Find(test_set_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto niter = ps->Find(test_set_object(test_size));
	ASSERT_EQ(niter, ps->GetEnd());

	ASSERT_EQ(ps->GetSize(), test_size);

	const Set<test_set_object>* pcs = ps;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pcs->Find(test_set_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto cniter = pcs->Find(test_set_object(test_size));
	ASSERT_EQ(cniter, pcs->GetConstEnd());

	ASSERT_EQ(pcs->GetSize(), test_size);

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, GetTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(ps->Get(test_set_object(i)).key, i);
	}

	ASSERT_EQ(ps->GetSize(), test_size);

	const Set<test_set_object>* pcs = ps;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pcs->Get(test_set_object(i)).key, i);
	}

	ASSERT_EQ(pcs->GetSize(), test_size);

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, OperatorTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		(*ps)[test_set_object(i)] = test_set_object(i, i, val_rel_func);
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
		ASSERT_EQ((*ps)[test_set_object(i)].value, i);
		(*ps)[test_set_object(i)].value += 1;
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
		ASSERT_EQ((*ps)[test_set_object(i)].value, i + 1);
		(*ps)[test_set_object(i)].value -= 1;
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	delete ps;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, ForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Set<test_set_object> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		s1.Insert(test_set_object(i, 1));
	}
	ASSERT_EQ(s1.GetSize(), test_size);

	int last_key = 0;
	s1.ForEach([&last_key](test_set_object& val) {
		ASSERT_EQ(val.key, last_key);
		ASSERT_EQ(val.value, 1);
		val.value = val.key;
		++last_key;
	});

	ASSERT_EQ(s1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s1.Find(test_set_object(i))->value, i);
	}

	const Set<test_set_object> s2(s1);
	ASSERT_EQ(s2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s2.Find(i)->value, i);
	}

	last_key = 0;
	s2.ForEach([&](const test_set_object& val) {
		ASSERT_EQ(val.key, last_key);
		ASSERT_EQ(val.key, val.value);
		test_cnt[val.key] += 1;
		++last_key;
	});

	ASSERT_EQ(s2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s2.Find(i)->value, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(Set, ReverseForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Set<test_set_object> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		s1.Insert(test_set_object(i, 1));
	}
	ASSERT_EQ(s1.GetSize(), test_size);

	int last_key = test_size - 1;
	s1.ReverseForEach([&last_key](test_set_object& val) {
		ASSERT_EQ(val.key, last_key);
		ASSERT_EQ(val.value, 1);
		val.value = val.key;
		--last_key;
	});

	ASSERT_EQ(s1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s1.Find(test_set_object(i))->value, i);
	}

	const Set<test_set_object> s2(s1);
	ASSERT_EQ(s2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s2.Find(i)->value, i);
	}

	last_key = test_size - 1;
	s2.ReverseForEach([&](const test_set_object& val) {
		ASSERT_EQ(val.key, last_key);
		ASSERT_EQ(val.key, val.value);
		test_cnt[val.key] += 1;
		--last_key;
	});

	ASSERT_EQ(s2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(s2.Find(i)->value, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(Set, CopyConstructionTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps2 = new Set<test_set_object>(*ps);

	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Set, MoveConstructionTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps2 = new Set<test_set_object>(std::move(*ps));

	ASSERT_EQ(ps2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, CopyAssignmentTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps2 = new Set<test_set_object>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = *ps;

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(Set, MoveAssignmentTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps2 = new Set<test_set_object>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = std::move(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, AnotherAllocatorCopyConstructionTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object, Less<test_set_object>, StdAllocator>* ps2 = new Set<test_set_object, Less<test_set_object>, StdAllocator>(*ps);

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps3 = new Set<test_set_object>(*ps2);

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Set, AnotherAllocatorMoveConstructionTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}
	Set<test_set_object, Less<test_set_object>, StdAllocator>* ps2 = new Set<test_set_object, Less<test_set_object>, StdAllocator>(std::move(*ps));
	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}
	Set<test_set_object>* ps3 = new Set<test_set_object>(std::move(*ps2));
	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}
	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, AnotherAllocatorCopyAssignmentTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object, Less<test_set_object>, StdAllocator>* ps2 = new Set<test_set_object, Less<test_set_object>, StdAllocator>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = *ps;

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps3 = new Set<test_set_object>();

	ASSERT_EQ(ps3->GetSize(), 0);

	*ps3 = *ps2;

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(Set, AnotherAllocatorMoveAssignmentTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object, Less<test_set_object>, StdAllocator>* ps2 = new Set<test_set_object, Less<test_set_object>, StdAllocator>();

	ASSERT_EQ(ps2->GetSize(), 0);

	*ps2 = std::move(*ps);

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps3 = new Set<test_set_object>();

	ASSERT_EQ(ps3->GetSize(), 0);

	*ps3 = std::move(*ps2);

	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, EqualTest)
{
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set1({test_set_object(1, 10),
																					test_set_object(2, 20),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set2({test_set_object(1, 10),
																					test_set_object(2, 20)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set2_({test_set_object(1, 10),
																		   test_set_object(2, 20)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set3({test_set_object(1, 10),
																					test_set_object(2, 21),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set3_({test_set_object(1, 10),
																		   test_set_object(2, 21),
																		   test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set4({test_set_object(1, 10),
																					test_set_object(2, 20),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set4_({test_set_object(1, 10),
																		   test_set_object(2, 20),
																		   test_set_object(3, 30)});

	ASSERT_FALSE(set1 == set2);
	ASSERT_FALSE(set1 == set2_);
	ASSERT_FALSE(set1 == set3);
	ASSERT_FALSE(set1 == set3_);
	ASSERT_TRUE(set1 == set4);
	ASSERT_TRUE(set1 == set4_);
}

TEST(Set, NotEqualTest)
{
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set1({test_set_object(1, 10),
																					test_set_object(2, 20),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set2({test_set_object(1, 10),
																					test_set_object(2, 20)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set2_({test_set_object(1, 10),
																		   test_set_object(2, 20)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set3({test_set_object(1, 10),
																					test_set_object(2, 21),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set3_({test_set_object(1, 10),
																		   test_set_object(2, 21),
																		   test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, MemoryManagerAllocator> set4({test_set_object(1, 10),
																					test_set_object(2, 20),
																					test_set_object(3, 30)});
	const Set<test_set_object, Less<test_set_object>, StdAllocator> set4_({test_set_object(1, 10),
																		   test_set_object(2, 20),
																		   test_set_object(3, 30)});

	ASSERT_TRUE(set1 != set2);
	ASSERT_TRUE(set1 != set2_);
	ASSERT_TRUE(set1 != set3);
	ASSERT_TRUE(set1 != set3_);
	ASSERT_FALSE(set1 != set4);
	ASSERT_FALSE(set1 != set4_);
}

TEST(Set, SwapTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps2 = new Set<test_set_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = ps2->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps3 = new Set<test_set_object>(std::move(*ps));

	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}

	*ps = std::move(*ps2);

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	*ps2 = std::move(*ps3);

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, AnotherAllocatorSwapTest)
{
	Set<test_set_object>* ps = new Set<test_set_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = ps->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	Set<test_set_object, Less<test_set_object>, StdAllocator>* ps2 = new Set<test_set_object, Less<test_set_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = ps2->Insert(test_set_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	Set<test_set_object>* ps3 = new Set<test_set_object>(std::move(*ps));

	ASSERT_EQ(ps3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps3)[test_set_object(i)].key, i);
	}

	*ps = std::move(*ps2);

	ASSERT_EQ(ps->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*ps)[test_set_object(i)].key, i);
	}

	*ps2 = std::move(*ps3);

	ASSERT_EQ(ps2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*ps2)[test_set_object(i)].key, i);
	}

	delete ps;
	delete ps2;
	delete ps3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(Set, ConstructByEmptyListTest)
{
	Set<int> s(std::initializer_list<int>{});
	ASSERT_EQ(s.GetSize(), 0);

	s.Insert(2);
	ASSERT_EQ(s.GetSize(), 1);
	ASSERT_EQ(*s.GetBegin(), 2);
}

TEST(Set, InsertEmptyListTest)
{
	Set<int> s({2});
	ASSERT_EQ(s.GetSize(), 1);
	ASSERT_EQ(*s.GetBegin(), 2);

	s.Insert(std::initializer_list<int>{});
	ASSERT_EQ(s.GetSize(), 1);
	ASSERT_EQ(*s.GetBegin(), 2);
}

TEST(SetIterator, GetBeginTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::Iterator>));
	ASSERT_EQ(*iter, 1);
}

TEST(SetIterator, GetEndTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::Iterator>));
	--iter;
	ASSERT_EQ(*iter, 3);
}

TEST(SetIterator, GetConstBeginTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ConstIterator>));
	ASSERT_EQ(*iter, 1);
}

TEST(SetIterator, GetConstEndTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ConstIterator>));
	--iter;
	ASSERT_EQ(*iter, 3);
}

TEST(SetIterator, GetReverseBeginTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ReverseIterator>));
	ASSERT_EQ(*iter, 3);
}

TEST(SetIterator, GetReverseEndTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ReverseIterator>));
	--iter;
	ASSERT_EQ(*iter, 1);
}

TEST(SetIterator, GetConstReverseBeginTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ConstReverseIterator>));
	ASSERT_EQ(*iter, 3);
}

TEST(SetIterator, GetConstReverseEndTest)
{
	Set<int> s({1, 2, 3});
	ASSERT_EQ(s.GetSize(), 3);
	auto iter = s.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Set<int>::ConstReverseIterator>));
	--iter;
	ASSERT_EQ(*iter, 1);
}

TEST(SetIterator, IteratorTest)
{
	Set<Pair<const int, int>, KeyComparer<Less<int>>> s({MakePair(1, 1.0),
														 MakePair(2, 2.0),
														 MakePair(3, 3.0),
														 MakePair(4, 4.0),
														 MakePair(5, 5.0),
														 MakePair(6, 6.0),
														 MakePair(7, 7.0),
														 MakePair(8, 8.0),
														 MakePair(9, 9.0),
														 MakePair(10, 10.0)});
	ASSERT_EQ(s.GetSize(), 10);

	int cnt = 1;
	for (auto i = s.GetBegin(); i != s.GetEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt++;
	}

	auto iter = s.GetBegin();
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

TEST(SetIterator, ConstIteratorTest)
{
	const Set<Pair<const int, int>, KeyComparer<Less<int>>> s({MakePair(1, 1.0),
															   MakePair(2, 2.0),
															   MakePair(3, 3.0),
															   MakePair(4, 4.0),
															   MakePair(5, 5.0),
															   MakePair(6, 6.0),
															   MakePair(7, 7.0),
															   MakePair(8, 8.0),
															   MakePair(9, 9.0),
															   MakePair(10, 10.0)});
	ASSERT_EQ(s.GetSize(), 10);

	int cnt = 1;
	for (auto i = s.GetConstBegin(); i != s.GetConstEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt++;
	}

	auto iter = s.GetConstBegin();
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

TEST(SetIterator, ReverseIteratorTest)
{
	Set<Pair<const int, int>, KeyComparer<Less<int>>> s({MakePair(1, 1.0),
														 MakePair(2, 2.0),
														 MakePair(3, 3.0),
														 MakePair(4, 4.0),
														 MakePair(5, 5.0),
														 MakePair(6, 6.0),
														 MakePair(7, 7.0),
														 MakePair(8, 8.0),
														 MakePair(9, 9.0),
														 MakePair(10, 10.0)});
	ASSERT_EQ(s.GetSize(), 10);

	int cnt = 10;
	for (auto i = s.GetReverseBegin(); i != s.GetReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt--;
	}

	auto iter = s.GetReverseBegin();
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

TEST(SetIterator, ConstReverseIteratorTest)
{
	const Set<Pair<const int, int>, KeyComparer<Less<int>>> s({MakePair(1, 1.0),
															   MakePair(2, 2.0),
															   MakePair(3, 3.0),
															   MakePair(4, 4.0),
															   MakePair(5, 5.0),
															   MakePair(6, 6.0),
															   MakePair(7, 7.0),
															   MakePair(8, 8.0),
															   MakePair(9, 9.0),
															   MakePair(10, 10.0)});
	ASSERT_EQ(s.GetSize(), 10);

	int cnt = 10;
	for (auto i = s.GetConstReverseBegin(); i != s.GetConstReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt--;
	}

	auto iter = s.GetConstReverseBegin();
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

TEST(SetIterator, PlusTest)
{
	Set<int> s({0, 1, 2, 3, 4});
	auto iter1_1 = s.GetBegin() + 1;
	ASSERT_EQ(*iter1_1, 1);
	auto iter1_2 = s.GetBegin() + 2;
	ASSERT_EQ(*iter1_2, 2);

	auto iter3_1 = s.GetReverseBegin() + 1;
	ASSERT_EQ(*iter3_1, 3);
	auto iter3_2 = s.GetReverseBegin() + 2;
	ASSERT_EQ(*iter3_2, 2);

	const Set<int>& cs = s;
	auto iter2_1 = cs.GetConstBegin() + 1;
	ASSERT_EQ(*iter2_1, 1);
	auto iter2_2 = cs.GetConstBegin() + 2;
	ASSERT_EQ(*iter2_2, 2);

	auto iter4_1 = cs.GetConstReverseBegin() + 1;
	ASSERT_EQ(*iter4_1, 3);
	auto iter4_2 = cs.GetConstReverseBegin() + 2;
	ASSERT_EQ(*iter4_2, 2);
}

TEST(SetIterator, SubtractTest)
{
	Set<int> s({0, 1, 2, 3, 4});
	auto iter1_1 = s.GetEnd() - 1;
	ASSERT_EQ(*iter1_1, 4);
	auto iter1_2 = s.GetEnd() - 2;
	ASSERT_EQ(*iter1_2, 3);

	auto iter3_1 = s.GetReverseEnd() - 1;
	ASSERT_EQ(*iter3_1, 0);
	auto iter3_2 = s.GetReverseEnd() - 2;
	ASSERT_EQ(*iter3_2, 1);

	const Set<int>& cs = s;
	auto iter2_1 = cs.GetConstEnd() - 1;
	ASSERT_EQ(*iter2_1, 4);
	auto iter2_2 = cs.GetConstEnd() - 2;
	ASSERT_EQ(*iter2_2, 3);

	auto iter4_1 = cs.GetConstReverseEnd() - 1;
	ASSERT_EQ(*iter4_1, 0);
	auto iter4_2 = cs.GetConstReverseEnd() - 2;
	ASSERT_EQ(*iter4_2, 1);
}

TEST(SetIterator, SelfIncreaseTest)
{
	Set<int> s({0, 1, 2, 3, 4});
	auto iter1 = s.GetBegin();
	ASSERT_EQ(*iter1, 0);
	iter1 += 2;
	ASSERT_EQ(*iter1, 2);

	auto iter3 = s.GetReverseBegin();
	ASSERT_EQ(*iter3, 4);
	iter3 += 2;
	ASSERT_EQ(*iter3, 2);

	const Set<int>& cs = s;
	auto iter2 = cs.GetConstBegin();
	ASSERT_EQ(*iter2, 0);
	iter2 += 2;
	ASSERT_EQ(*iter2, 2);

	auto iter4 = cs.GetConstReverseBegin();
	ASSERT_EQ(*iter4, 4);
	iter4 += 2;
	ASSERT_EQ(*iter4, 2);
}

TEST(SetIterator, SelfDecreaseTest)
{
	Set<int> s({0, 1, 2, 3, 4});
	auto iter1 = s.GetEnd();
	iter1 -= 2;
	ASSERT_EQ(*iter1, 3);

	auto iter3 = s.GetReverseEnd();
	iter3 -= 2;
	ASSERT_EQ(*iter3, 1);

	const Set<int>& cs = s;
	auto iter2 = cs.GetConstEnd();
	iter2 -= 2;
	ASSERT_EQ(*iter2, 3);

	auto iter4 = cs.GetConstReverseEnd();
	iter4 -= 2;
	ASSERT_EQ(*iter4, 1);
}

TEST(SetIterator, DistanceTest)
{
	Set<int> s({0, 1, 2, 3, 4});
	ASSERT_EQ(s.GetSize(), 5);
	ASSERT_EQ(s.GetEnd() - s.GetBegin(), 5);
	ASSERT_EQ(s.GetReverseEnd() - s.GetReverseBegin(), 5);

	const Set<int>& cs = s;
	ASSERT_EQ(cs.GetSize(), 5);
	ASSERT_EQ(cs.GetConstEnd() - cs.GetConstBegin(), 5);
	ASSERT_EQ(cs.GetConstReverseEnd() - cs.GetConstReverseBegin(), 5);
}
