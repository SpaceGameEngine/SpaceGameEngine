﻿/*
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
#include "Container/List.hpp"

using namespace SpaceGameEngine;

struct test_list_object
{
	std::function<void(test_list_object&)> rel_func;
	test_list_object()
		: val(0), rel_func([](test_list_object&) {})
	{
	}
	test_list_object(int v)
		: val(v), rel_func([](test_list_object&) {})
	{
	}
	test_list_object(int v, const std::function<void(test_list_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_list_object(const test_list_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_list_object(test_list_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_list_object&) {};
	}
	~test_list_object()
	{
		rel_func(*this);
	}
	int val;

	test_list_object& operator=(const test_list_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_list_object& operator=(test_list_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_list_object&) {};
		return *this;
	}

	bool operator<(const test_list_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_list_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_list_object& o) const
	{
		return val != o.val;
	}
};

template<typename Allocator>
inline bool CheckListConnection(const List<test_list_object, Allocator>& l)
{
	int* pval = new int[l.GetSize()];
	memset(pval, 0, sizeof(int) * l.GetSize());

	int cnt = 0;
	for (auto i = l.GetConstBegin(); i != l.GetConstEnd(); ++i, ++cnt)
	{
		pval[cnt] = i->val;
	}

	if (cnt != l.GetSize())
		return false;

	cnt -= 1;

	for (auto i = l.GetConstReverseBegin(); i != l.GetConstReverseEnd(); ++i, --cnt)
	{
		if (pval[cnt] != i->val)
			return false;
	}

	cnt += 1;

	if (cnt != 0)
		return false;

	delete[] pval;
	return true;
}

TEST(List, InstanceTest)
{
	List<int> l;
	ASSERT_TRUE((std::is_same_v<decltype(l)::ValueType, int>));
	ASSERT_TRUE((std::is_same_v<decltype(l)::AllocatorType, DefaultAllocator>));
	ASSERT_EQ(l.GetSize(), 0);
}

TEST(List, ClearTest)
{
	List<int> l;	//todo : add more content
	ASSERT_EQ(l.GetSize(), 0);
	l.Clear();
	ASSERT_EQ(l.GetSize(), 0);
}

TEST(List, PushBackTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		ASSERT_EQ(pl->PushBack(buf).val, i);
		auto iter = pl->GetEnd() - 1;
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		ASSERT_EQ(pl->PushBack(std::move(buf)).val, i);
		auto iter = pl->GetEnd() - 1;
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), 2 * test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(List, EmplaceBackTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
		auto iter = pl->GetEnd() - 1;
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, PushFrontTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		ASSERT_EQ(pl->PushFront(buf).val, i);
		auto iter = pl->GetBegin();
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		ASSERT_EQ(pl->PushFront(std::move(buf)).val, i);
		auto iter = pl->GetBegin();
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), 2 * test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(List, EmplaceFrontTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceFront(i, val_rel_func).val, i);
		auto iter = pl->GetBegin();
		ASSERT_EQ(iter->val, i);
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, InsertTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		auto iter = pl->Insert(pl->GetEnd(), buf);
		ASSERT_EQ(iter->val, i);
		ASSERT_EQ(iter, pl->GetReverseBegin());
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		auto iter = pl->Insert(pl->GetConstReverseEnd(), std::move(buf));
		ASSERT_EQ(iter->val, i);
		ASSERT_EQ(iter, pl->GetConstBegin());
	}
	ASSERT_EQ(pl->GetSize(), test_size * 2);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}

	int check_arr[7];
	memset(check_arr, 0, sizeof(check_arr));
	List<int> l;
	for (int i = 0; i < 5; i++)
		l.Insert(l.GetConstEnd(), i);

	auto iter1 = l.Insert(l.GetBegin() + 1, 5);
	ASSERT_EQ(*iter1, 5);

	for (auto i = l.GetBegin(); i != l.GetEnd(); ++i)
		check_arr[*i] += 1;

	for (int i = 0; i < 6; i++)
		ASSERT_EQ(check_arr[i], 1);

	auto iter2 = l.Insert(l.GetConstReverseBegin() + 1, 6);
	ASSERT_EQ(*iter2, 6);

	for (auto i = l.GetConstReverseBegin(); i != l.GetConstReverseEnd(); ++i)
		check_arr[*i] += 1;

	for (int i = 0; i < 6; i++)
		ASSERT_EQ(check_arr[i], 2);

	ASSERT_EQ(check_arr[6], 1);
}

TEST(List, EmplaceTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pl->Emplace(pl->GetEnd(), i, val_rel_func);
		ASSERT_EQ(iter->val, i);
		ASSERT_EQ(iter, pl->GetReverseBegin());
	}
	ASSERT_EQ(pl->GetSize(), test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	int check_arr[7];
	memset(check_arr, 0, sizeof(check_arr));
	List<int> l;
	for (int i = 0; i < 5; i++)
		l.Emplace(l.GetConstEnd(), i);

	auto iter1 = l.Emplace(l.GetBegin() + 1, 5);
	ASSERT_EQ(*iter1, 5);

	for (auto i = l.GetBegin(); i != l.GetEnd(); ++i)
		check_arr[*i] += 1;

	for (int i = 0; i < 6; i++)
		ASSERT_EQ(check_arr[i], 1);

	auto iter2 = l.Emplace(l.GetConstReverseBegin() + 1, 6);
	ASSERT_EQ(*iter2, 6);

	for (auto i = l.GetConstReverseBegin(); i != l.GetConstReverseEnd(); ++i)
		check_arr[*i] += 1;

	for (int i = 0; i < 6; i++)
		ASSERT_EQ(check_arr[i], 2);

	ASSERT_EQ(check_arr[6], 1);
}

TEST(List, InsertSizeTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		test_list_object buf(i, val_rel_func);
		auto iter = pl->Insert(pl->GetConstBegin(), 1, buf);
		ASSERT_EQ(iter->val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetReverseBegin(); i != pl->GetReverseEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}

	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	{
		test_list_object tbuf(1, val_rel_func);
		auto iter = pl->Insert(pl->GetBegin(), test_size, tbuf);
		ASSERT_EQ(pl->GetSize(), test_size);
		ASSERT_EQ(iter->val, 1);
		ASSERT_EQ(iter, pl->GetBegin());
		for (auto i = iter; i != pl->GetEnd(); ++i)
		{
			ASSERT_EQ(i->val, 1);
		}
	}

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	ASSERT_EQ(val_pool[1], test_size + 1);

	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	pl->PushBack(test_list_object(-2));
	pl->PushFront(test_list_object(-1));
	ASSERT_EQ(pl->GetSize(), 2);
	ASSERT_EQ(pl->GetBegin()->val, -1);
	ASSERT_EQ((pl->GetEnd() - 1)->val, -2);

	{
		test_list_object tbuf(2, val_rel_func);
		auto iter = pl->Insert(pl->GetReverseBegin() + 1, test_size, tbuf);
		ASSERT_EQ(pl->GetSize(), test_size + 2);
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::ReverseIterator>));
		ASSERT_EQ(iter->val, 2);
		ASSERT_EQ(iter, pl->GetReverseBegin() + 1);
		for (auto i = iter; i != pl->GetReverseEnd() - 1; ++i)
		{
			ASSERT_EQ(i->val, 2);
		}
	}

	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		if (cnt == 0)
			ASSERT_EQ(i->val, -1);
		else if (cnt == test_size + 1)
			ASSERT_EQ(i->val, -2);
		else
			ASSERT_EQ(i->val, 2);
	}

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	ASSERT_EQ(val_pool[2], test_size + 1);
}

TEST(ListIterator, OutOfRangeErrorTest)
{
	//todo : make list more content, add more test
	List<int> l;
	ASSERT_TRUE(List<int>::Iterator::OutOfRangeError::Judge(l.GetEnd()));
	ASSERT_TRUE(List<int>::ConstIterator::OutOfRangeError::Judge(l.GetConstEnd()));
	ASSERT_TRUE(List<int>::ReverseIterator::OutOfRangeError::Judge(l.GetReverseEnd()));
	ASSERT_TRUE(List<int>::ConstReverseIterator::OutOfRangeError::Judge(l.GetConstReverseEnd()));
}

TEST(ListIterator, GetTest)
{
	//todo : make list more content
	List<int> l;

	auto iter1 = l.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<int>::Iterator>));
	ASSERT_EQ(iter1, l.GetEnd());

	auto iter2 = l.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<int>::Iterator>));
	ASSERT_EQ(iter2, l.GetEnd());

	auto riter1 = l.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(riter1), List<int>::ReverseIterator>));
	ASSERT_EQ(riter1, l.GetReverseEnd());

	auto riter2 = l.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(riter2), List<int>::ReverseIterator>));
	ASSERT_EQ(riter2, l.GetReverseEnd());
}

TEST(ListIterator, GetConstTest)
{
	//todo : make list more content
	const List<int> l;

	auto iter1 = l.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<int>::ConstIterator>));
	ASSERT_EQ(iter1, l.GetConstEnd());

	auto iter2 = l.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<int>::ConstIterator>));
	ASSERT_EQ(iter2, l.GetConstEnd());

	auto riter1 = l.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(riter1), List<int>::ConstReverseIterator>));
	ASSERT_EQ(riter1, l.GetConstReverseEnd());

	auto riter2 = l.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(riter2), List<int>::ConstReverseIterator>));
	ASSERT_EQ(riter2, l.GetConstReverseEnd());
}