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

TEST(List, SizeConstructorTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	{
		test_list_object tbuf(1, val_rel_func);
		List<test_list_object>* pl = new List<test_list_object>(10, tbuf);

		ASSERT_EQ(pl->GetSize(), 10);

		for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i)
		{
			ASSERT_EQ(i->val, 1);
		}

		ASSERT_TRUE(CheckListConnection(*pl));

		delete pl;
	}
	ASSERT_EQ(val_pool[1], 11);
	for (int i = 0; i < test_size; i++)
	{
		if (i != 1)
			ASSERT_EQ(val_pool[i], 0);
	}
}

TEST(List, RangeConstructorTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};

	List<test_list_object>* pld = new List<test_list_object>();

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pld->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pld->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pld->GetConstBegin(); i != pld->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);
	ASSERT_TRUE(CheckListConnection(*pld));

	List<test_list_object>* pl = new List<test_list_object>(pld->GetBegin(), pld->GetEnd());

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetBegin(); i != pl->GetEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);
	ASSERT_TRUE(CheckListConnection(*pl));

	List<test_list_object>* pl2 = new List<test_list_object>(pld->GetConstReverseBegin(), pld->GetConstReverseEnd());

	ASSERT_EQ(pl2->GetSize(), test_size);
	--cnt;
	for (auto i = pl2->GetConstBegin(); i != pl2->GetConstEnd(); ++i, --cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	++cnt;
	ASSERT_EQ(cnt, 0);
	ASSERT_TRUE(CheckListConnection(*pl2));

	delete pld;
	delete pl;
	delete pl2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(List, ListConstructorTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};

	List<test_list_object>* pl = new List<test_list_object>({test_list_object(0, val_rel_func),
															 test_list_object(1, val_rel_func),
															 test_list_object(2, val_rel_func),
															 test_list_object(3, val_rel_func),
															 test_list_object(4, val_rel_func),
															 test_list_object(5, val_rel_func),
															 test_list_object(6, val_rel_func),
															 test_list_object(7, val_rel_func),
															 test_list_object(8, val_rel_func),
															 test_list_object(9, val_rel_func)});

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetBegin(); i != pl->GetEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);
	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
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

TEST(List, InsertRangeTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();
	List<test_list_object>* pld = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pld->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pld->GetSize(), test_size);

	auto iter1 = pl->Insert(pl->GetEnd(), pld->GetConstBegin(), pld->GetConstEnd());
	ASSERT_EQ(pl->GetSize(), test_size);
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<test_list_object>::Iterator>));
	ASSERT_EQ(iter1, pl->GetBegin());
	int cnt = 0;
	for (; iter1 != pl->GetEnd(); ++iter1, ++cnt)
	{
		ASSERT_EQ(iter1->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	auto iter2 = pl->Insert(pl->GetReverseEnd() - 1, pld->GetBegin(), pld->GetEnd());
	ASSERT_EQ(pl->GetSize(), 2 * test_size);
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<test_list_object>::ReverseIterator>));
	ASSERT_EQ(iter2.GetData(), (pl->GetBegin() + test_size).GetData());
	cnt = 0;
	for (; iter2 != pl->GetReverseEnd() - 1; ++iter2, ++cnt)
	{
		ASSERT_EQ(iter2->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	--cnt;
	for (auto i = pl->GetConstBegin() + 1; i != pl->GetConstBegin() + test_size + 1; ++i, --cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	++cnt;
	ASSERT_EQ(cnt, 0);

	auto iter3 = pl->Insert(pl->GetConstBegin(), pld->GetConstReverseBegin(), pld->GetConstReverseEnd());
	ASSERT_EQ(pl->GetSize(), 3 * test_size);
	ASSERT_TRUE((std::is_same_v<decltype(iter3), List<test_list_object>::ConstIterator>));
	ASSERT_EQ(iter3, pl->GetConstBegin());

	cnt = test_size - 1;
	for (; iter3 != pl->GetConstBegin() + test_size; ++iter3, --cnt)
	{
		ASSERT_EQ(iter3->val, cnt);
	}
	++cnt;
	ASSERT_EQ(cnt, 0);

	auto iter4 = pl->Insert(pl->GetConstReverseBegin(), pld->GetReverseBegin(), pld->GetReverseEnd());
	ASSERT_EQ(pl->GetSize(), 4 * test_size);
	ASSERT_TRUE((std::is_same_v<decltype(iter4), List<test_list_object>::ConstReverseIterator>));
	ASSERT_EQ(iter4.GetData(), (pl->GetEnd() - 1).GetData());
	cnt = test_size - 1;
	for (; iter4 != pl->GetReverseBegin() + test_size; ++iter4, --cnt)
	{
		ASSERT_EQ(iter4->val, cnt);
	}
	++cnt;
	ASSERT_EQ(cnt, 0);

	cnt = 0;
	for (auto i = pl->GetConstBegin() + 3 * test_size; i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pld;
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 5);
	}
}

TEST(List, InsertListTest)
{
	const int test_size = 5;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	List<test_list_object>* pl = new List<test_list_object>();

	auto iter1 = pl->Insert(pl->GetConstEnd(), {test_list_object(0, val_rel_func),
												test_list_object(1, val_rel_func),
												test_list_object(2, val_rel_func)});

	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<test_list_object>::ConstIterator>));
	ASSERT_EQ(iter1->val, 0);

	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, 3);

	auto iter2 = pl->Insert(pl->GetReverseBegin(), {test_list_object(4, val_rel_func),
													test_list_object(3, val_rel_func)});
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<test_list_object>::ReverseIterator>));
	ASSERT_EQ(iter2->val, 4);

	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, 5);

	ASSERT_TRUE(CheckListConnection(*pl));

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(List, PopBackTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}

	ASSERT_TRUE(CheckListConnection(*pl));

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pl->GetConstReverseBegin()->val, test_size - i - 1);
		pl->PopBack();
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
			ASSERT_TRUE(CheckListConnection(*pl));
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, PopFrontTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}

	ASSERT_TRUE(CheckListConnection(*pl));

	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pl->GetBegin()->val, i);
		pl->PopFront();
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
			ASSERT_TRUE(CheckListConnection(*pl));
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, RemoveTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	//--------------------------------------------------------
	List<test_list_object>* pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove front
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pl->GetBegin()->val, i);
		auto iter = pl->Remove(pl->GetBegin());
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::Iterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
		{
			ASSERT_TRUE(CheckListConnection(*pl));
			ASSERT_EQ(iter->val, i + 1);
		}
		else
			ASSERT_EQ(iter, pl->GetEnd());
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove end
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ((pl->GetConstEnd() - 1)->val, test_size - 1 - i);
		auto iter = pl->Remove(pl->GetConstEnd() - 1);
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::ConstIterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
		{
			ASSERT_TRUE(CheckListConnection(*pl));
		}
		ASSERT_EQ(iter, pl->GetConstEnd());
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove rbegin
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(pl->GetReverseBegin()->val, test_size - 1 - i);
		auto iter = pl->Remove(pl->GetReverseBegin());
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::ReverseIterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
		{
			ASSERT_TRUE(CheckListConnection(*pl));
			ASSERT_EQ(iter->val, test_size - 2 - i);
		}
		else
			ASSERT_EQ(iter, pl->GetReverseEnd());
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove rend
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_EQ((pl->GetReverseEnd() - 1)->val, i);
		auto iter = pl->Remove(pl->GetReverseEnd() - 1);
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::ReverseIterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		if (i != test_size - 1)
		{
			ASSERT_TRUE(CheckListConnection(*pl));
		}
		ASSERT_EQ(iter, pl->GetReverseEnd());
	}

	ASSERT_EQ(pl->GetSize(), 0);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove mid
	for (int i = 0; i < test_size - 2; ++i)
	{
		ASSERT_EQ((pl->GetBegin() + 1)->val, i + 1);
		auto iter = pl->Remove(pl->GetBegin() + 1);
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::Iterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		ASSERT_TRUE(CheckListConnection(*pl));
		ASSERT_EQ(iter->val, i + 2);
		ASSERT_EQ(iter, pl->GetBegin() + 1);
	}

	ASSERT_EQ(pl->GetSize(), 2);
	ASSERT_EQ(pl->GetBegin()->val, 0);
	ASSERT_EQ(pl->GetReverseBegin()->val, test_size - 1);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove rmid
	for (int i = 0; i < test_size - 2; ++i)
	{
		ASSERT_EQ((pl->GetReverseBegin() + 1)->val, test_size - 2 - i);
		auto iter = pl->Remove(pl->GetReverseBegin() + 1);
		ASSERT_TRUE((std::is_same_v<decltype(iter), List<test_list_object>::ReverseIterator>));
		ASSERT_EQ(pl->GetSize(), test_size - i - 1);
		ASSERT_TRUE(CheckListConnection(*pl));
		ASSERT_EQ(iter->val, test_size - 3 - i);
		ASSERT_EQ(iter, pl->GetReverseBegin() + 1);
	}

	ASSERT_EQ(pl->GetSize(), 2);
	ASSERT_EQ(pl->GetBegin()->val, 0);
	ASSERT_EQ(pl->GetReverseBegin()->val, test_size - 1);

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, RemoveRangeTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_list_object& o) {
		val_pool[o.val] += 1;
	};
	//--------------------------------------------------------
	List<test_list_object>* pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [mid,end)
	auto iter1 = pl->Remove(pl->GetConstBegin() + 1, pl->GetConstEnd());
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<test_list_object>::ConstIterator>));
	ASSERT_EQ(pl->GetSize(), 1);
	ASSERT_EQ(iter1, pl->GetConstEnd());
	ASSERT_EQ(pl->GetBegin()->val, 0);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [beg,mid)
	auto iter2 = pl->Remove(pl->GetBegin(), pl->GetEnd() - 1);
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<test_list_object>::Iterator>));
	ASSERT_EQ(pl->GetSize(), 1);
	ASSERT_EQ(iter2, pl->GetEnd() - 1);
	ASSERT_EQ((pl->GetEnd() - 1)->val, test_size - 1);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [beg,end)
	auto iter3 = pl->Remove(pl->GetBegin(), pl->GetEnd());
	ASSERT_TRUE((std::is_same_v<decltype(iter3), List<test_list_object>::Iterator>));
	ASSERT_EQ(pl->GetSize(), 0);
	ASSERT_EQ(iter3, pl->GetEnd());

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [mid,mid)
	auto iter4 = pl->Remove(pl->GetBegin() + 1, pl->GetEnd() - 1);
	ASSERT_TRUE((std::is_same_v<decltype(iter4), List<test_list_object>::Iterator>));
	ASSERT_EQ(pl->GetSize(), 2);
	ASSERT_EQ(iter4, pl->GetEnd() - 1);
	ASSERT_EQ((pl->GetEnd() - 1)->val, test_size - 1);
	ASSERT_EQ(pl->GetBegin()->val, 0);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [rmid,rend)
	auto iter5 = pl->Remove(pl->GetConstReverseBegin() + 1, pl->GetConstReverseEnd());
	ASSERT_TRUE((std::is_same_v<decltype(iter5), List<test_list_object>::ConstReverseIterator>));
	ASSERT_EQ(pl->GetSize(), 1);
	ASSERT_EQ(iter5, pl->GetConstReverseEnd());
	ASSERT_EQ(pl->GetBegin()->val, test_size - 1);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [rbeg,rmid)
	auto iter6 = pl->Remove(pl->GetReverseBegin(), pl->GetReverseEnd() - 1);
	ASSERT_TRUE((std::is_same_v<decltype(iter6), List<test_list_object>::ReverseIterator>));
	ASSERT_EQ(pl->GetSize(), 1);
	ASSERT_EQ(iter6, pl->GetReverseEnd() - 1);
	ASSERT_EQ(pl->GetBegin()->val, 0);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [rbeg,rend)
	auto iter7 = pl->Remove(pl->GetReverseBegin(), pl->GetReverseEnd());
	ASSERT_TRUE((std::is_same_v<decltype(iter7), List<test_list_object>::ReverseIterator>));
	ASSERT_EQ(pl->GetSize(), 0);
	ASSERT_EQ(iter7, pl->GetReverseEnd());

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	//--------------------------------------------------------
	memset(val_pool, 0, sizeof(val_pool));
	pl = new List<test_list_object>();

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(pl->EmplaceBack(i, val_rel_func).val, i);
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//remove [rmid,rmid)
	auto iter8 = pl->Remove(pl->GetReverseBegin() + 1, pl->GetReverseEnd() - 1);
	ASSERT_TRUE((std::is_same_v<decltype(iter8), List<test_list_object>::ReverseIterator>));
	ASSERT_EQ(pl->GetSize(), 2);
	ASSERT_EQ(iter8, pl->GetReverseEnd() - 1);
	ASSERT_EQ((pl->GetEnd() - 1)->val, test_size - 1);
	ASSERT_EQ(pl->GetBegin()->val, 0);

	ASSERT_TRUE(CheckListConnection(*pl));
	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, FindTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	//-----------------------------------------------
	auto iter1 = pl->Find(test_list_object(1));
	ASSERT_TRUE((std::is_same_v<decltype(iter1), List<test_list_object>::Iterator>));
	ASSERT_EQ(iter1->val, 1);
	ASSERT_EQ(iter1, pl->GetBegin() + 1);

	auto iter2 = pl->FindByFunction([](const test_list_object& t) {
		return t.val == 2;
	});
	ASSERT_TRUE((std::is_same_v<decltype(iter2), List<test_list_object>::Iterator>));
	ASSERT_EQ(iter2->val, 2);
	ASSERT_EQ(iter2, pl->GetBegin() + 2);

	int buf = 0;
	pl->FindAll(test_list_object(5), [&](const test_list_object& i) {
		buf = i.val;
	});
	ASSERT_EQ(buf, 5);

	pl->FindAllByFunction([](const test_list_object& t) { return t.val == 10; }, [&](const test_list_object& i) { buf = i.val; });
	ASSERT_EQ(buf, 10);
	//-----------------------------------------------
	const List<test_list_object>* pcl = pl;

	auto iter3 = pcl->Find(test_list_object(1));
	ASSERT_TRUE((std::is_same_v<decltype(iter3), List<test_list_object>::ConstIterator>));
	ASSERT_EQ(iter3->val, 1);
	ASSERT_EQ(iter3, pcl->GetConstBegin() + 1);

	auto iter4 = pcl->FindByFunction([](const test_list_object& t) {
		return t.val == 2;
	});
	ASSERT_TRUE((std::is_same_v<decltype(iter4), List<test_list_object>::ConstIterator>));
	ASSERT_EQ(iter4->val, 2);
	ASSERT_EQ(iter4, pcl->GetConstBegin() + 2);

	buf = 0;
	pcl->FindAll(test_list_object(5), [&](const test_list_object& i) {
		buf = i.val;
	});
	ASSERT_EQ(buf, 5);

	pcl->FindAllByFunction([](const test_list_object& t) { return t.val == 10; }, [&](const test_list_object& i) { buf = i.val; });
	ASSERT_EQ(buf, 10);
	//-----------------------------------------------

	delete pl;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, CopyConstructionTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//----------------------------------------------------------
	List<test_list_object>* pl2 = new List<test_list_object>(*pl);
	ASSERT_EQ(pl2->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl2->GetConstBegin(); i != pl2->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl2));

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//----------------------------------------------------------
	delete pl;
	delete pl2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(List, MoveConstructionTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//----------------------------------------------------------
	List<test_list_object>* pl2 = new List<test_list_object>(std::move(*pl));
	ASSERT_EQ(pl2->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl2->GetConstBegin(); i != pl2->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl2));

	ASSERT_EQ(pl->GetSize(), 0);
	//----------------------------------------------------------
	delete pl;
	delete pl2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(List, CopyAssignmentTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//----------------------------------------------------------
	List<test_list_object>* pl2 = new List<test_list_object>();
	ASSERT_EQ(pl2->GetSize(), 0);
	ASSERT_EQ(pl2->GetBegin(), pl2->GetEnd());

	*pl2 = *pl;

	ASSERT_EQ(pl2->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl2->GetConstBegin(); i != pl2->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl2));

	ASSERT_EQ(pl->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));

	List<test_list_object>* pl3 = new List<test_list_object>();
	ASSERT_EQ(pl3->GetSize(), 0);
	ASSERT_EQ(pl3->GetBegin(), pl3->GetEnd());

	*pl2 = *pl3;

	ASSERT_EQ(pl2->GetSize(), 0);
	ASSERT_EQ(pl2->GetBegin(), pl2->GetEnd());
	//----------------------------------------------------------
	delete pl;
	delete pl2;
	delete pl3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(List, MoveAssignmentTest)
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
	}

	ASSERT_EQ(pl->GetSize(), test_size);
	int cnt = 0;
	for (auto i = pl->GetConstBegin(); i != pl->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl));
	//----------------------------------------------------------
	List<test_list_object>* pl2 = new List<test_list_object>();
	ASSERT_EQ(pl2->GetSize(), 0);
	ASSERT_EQ(pl2->GetBegin(), pl2->GetEnd());

	*pl2 = std::move(*pl);

	ASSERT_EQ(pl2->GetSize(), test_size);
	cnt = 0;
	for (auto i = pl2->GetConstBegin(); i != pl2->GetConstEnd(); ++i, ++cnt)
	{
		ASSERT_EQ(i->val, cnt);
	}
	ASSERT_EQ(cnt, test_size);

	ASSERT_TRUE(CheckListConnection(*pl2));

	ASSERT_EQ(pl->GetSize(), 0);

	//----------------------------------------------------------
	List<test_list_object>* pl3 = new List<test_list_object>();
	ASSERT_EQ(pl3->GetSize(), 0);
	ASSERT_EQ(pl3->GetBegin(), pl3->GetEnd());

	*pl2 = std::move(*pl3);

	ASSERT_EQ(pl2->GetSize(), 0);
	ASSERT_EQ(pl2->GetBegin(), pl2->GetEnd());
	ASSERT_EQ(pl3->GetSize(), 0);
	//----------------------------------------------------------
	delete pl;
	delete pl2;
	delete pl3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
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