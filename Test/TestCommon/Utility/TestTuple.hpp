/*
Copyright 2026 creatorlxd

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
#include "Utility/Tuple.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct TupleTestObject
{
	// value: the data carried by this object
	// state: records which constructor/assignment was last invoked
	//   0 - default constructed
	//   1 - value constructed (from int)
	//   2 - copy constructed
	//   3 - move constructed
	//   4 - copy assigned
	//   5 - move assigned
	int value;
	int state;

	TupleTestObject()
		: value(0), state(0)
	{
	}

	explicit TupleTestObject(int val)
		: value(val), state(1)
	{
	}

	TupleTestObject(const TupleTestObject& other)
		: value(other.value), state(2)
	{
	}

	TupleTestObject(TupleTestObject&& other)
		: value(other.value), state(3)
	{
	}

	TupleTestObject& operator=(const TupleTestObject& other)
	{
		value = other.value;
		state = 4;
		return *this;
	}

	TupleTestObject& operator=(TupleTestObject&& other)
	{
		value = other.value;
		state = 5;
		return *this;
	}
};

// A type implicitly convertible from TupleTestObject to test cross-type copy/move
struct TupleTestObjectAlt
{
	int value;
	int state;

	TupleTestObjectAlt()
		: value(0), state(0)
	{
	}

	TupleTestObjectAlt(const TupleTestObject& other)
		: value(other.value), state(2)
	{
	}

	TupleTestObjectAlt(TupleTestObject&& other)
		: value(other.value), state(3)
	{
	}

	TupleTestObjectAlt& operator=(const TupleTestObject& other)
	{
		value = other.value;
		state = 4;
		return *this;
	}

	TupleTestObjectAlt& operator=(TupleTestObject&& other)
	{
		value = other.value;
		state = 5;
		return *this;
	}
};

TEST(Tuple, DefaultConstructorTest)
{
	Tuple<> tuple1;
	Tuple<int> tuple2;
	Tuple<int, double> tuple3;
	Tuple<int, double, char> tuple4;
}

TEST(Tuple, ValueConstructorTest)
{
	// TupleTestObject(7): value=7, state=1 (value constructed)
	// Forwarded as rvalue into Tuple: move constructor fires, so state=3, value preserved
	Tuple<TupleTestObject> t1(TupleTestObject(7));
	ASSERT_EQ(t1.Get<0>().value, 7);
	ASSERT_EQ(t1.Get<0>().state, 3);

	Tuple<TupleTestObject, TupleTestObject> t2(TupleTestObject(1), TupleTestObject(2));
	ASSERT_EQ(t2.Get<0>().value, 1);
	ASSERT_EQ(t2.Get<0>().state, 3);
	ASSERT_EQ(t2.Get<1>().value, 2);
	ASSERT_EQ(t2.Get<1>().state, 3);

	Tuple<TupleTestObject, TupleTestObject, TupleTestObject> t3(TupleTestObject(10), TupleTestObject(20), TupleTestObject(30));
	ASSERT_EQ(t3.Get<0>().value, 10);
	ASSERT_EQ(t3.Get<0>().state, 3);
	ASSERT_EQ(t3.Get<1>().value, 20);
	ASSERT_EQ(t3.Get<1>().state, 3);
	ASSERT_EQ(t3.Get<2>().value, 30);
	ASSERT_EQ(t3.Get<2>().state, 3);
}

TEST(Tuple, CopyConstructorTest)
{
	// same-type copy construction: state=2, value preserved from source
	const Tuple<TupleTestObject> t1(TupleTestObject(7));
	Tuple<TupleTestObject> t2(t1);
	ASSERT_EQ(t2.Get<0>().value, 7);
	ASSERT_EQ(t2.Get<0>().state, 2);

	const Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(1), TupleTestObject(2));
	Tuple<TupleTestObject, TupleTestObject> t4(t3);
	ASSERT_EQ(t4.Get<0>().value, 1);
	ASSERT_EQ(t4.Get<0>().state, 2);
	ASSERT_EQ(t4.Get<1>().value, 2);
	ASSERT_EQ(t4.Get<1>().state, 2);
}

TEST(Tuple, MoveConstructorTest)
{
	// same-type move construction: state=3, value preserved from source
	Tuple<TupleTestObject> t1(TupleTestObject(7));
	Tuple<TupleTestObject> t2(std::move(t1));
	ASSERT_EQ(t2.Get<0>().value, 7);
	ASSERT_EQ(t2.Get<0>().state, 3);

	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(1), TupleTestObject(2));
	Tuple<TupleTestObject, TupleTestObject> t4(std::move(t3));
	ASSERT_EQ(t4.Get<0>().value, 1);
	ASSERT_EQ(t4.Get<0>().state, 3);
	ASSERT_EQ(t4.Get<1>().value, 2);
	ASSERT_EQ(t4.Get<1>().state, 3);
}

TEST(Tuple, CopyAssignmentTest)
{
	// same-type copy assignment: state=4, value preserved from source
	Tuple<TupleTestObject> t1(TupleTestObject(7));
	Tuple<TupleTestObject> t2;
	t2 = t1;
	ASSERT_EQ(t2.Get<0>().value, 7);
	ASSERT_EQ(t2.Get<0>().state, 4);

	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(1), TupleTestObject(2));
	Tuple<TupleTestObject, TupleTestObject> t4;
	t4 = t3;
	ASSERT_EQ(t4.Get<0>().value, 1);
	ASSERT_EQ(t4.Get<0>().state, 4);
	ASSERT_EQ(t4.Get<1>().value, 2);
	ASSERT_EQ(t4.Get<1>().state, 4);
}

TEST(Tuple, MoveAssignmentTest)
{
	// same-type move assignment: state=5, value preserved from source
	Tuple<TupleTestObject> t1(TupleTestObject(7));
	Tuple<TupleTestObject> t2;
	t2 = std::move(t1);
	ASSERT_EQ(t2.Get<0>().value, 7);
	ASSERT_EQ(t2.Get<0>().state, 5);

	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(1), TupleTestObject(2));
	Tuple<TupleTestObject, TupleTestObject> t4;
	t4 = std::move(t3);
	ASSERT_EQ(t4.Get<0>().value, 1);
	ASSERT_EQ(t4.Get<0>().state, 5);
	ASSERT_EQ(t4.Get<1>().value, 2);
	ASSERT_EQ(t4.Get<1>().state, 5);
}

TEST(Tuple, CrossTypeCopyConstructorTest)
{
	// Tuple<int, float> -> Tuple<double, double>: implicit numeric conversion via copy ctor
	const Tuple<int, float> t1(3, 1.5f);
	Tuple<double, double> t2(t1);
	double t2_0 = t2.Get<0>();
	double t2_1 = t2.Get<1>();
	ASSERT_DOUBLE_EQ(t2_0, 3.0);
	ASSERT_DOUBLE_EQ(t2_1, 1.5);

	// Tuple<TupleTestObject, TupleTestObject> -> Tuple<TupleTestObjectAlt, TupleTestObjectAlt>: state=2, value preserved
	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(10), TupleTestObject(20));
	Tuple<TupleTestObjectAlt, TupleTestObjectAlt> t4(t3);
	ASSERT_EQ(t4.Get<0>().value, 10);
	ASSERT_EQ(t4.Get<0>().state, 2);
	ASSERT_EQ(t4.Get<1>().value, 20);
	ASSERT_EQ(t4.Get<1>().state, 2);

	// Tuple<TupleTestObject> -> Tuple<TupleTestObjectAlt>: state=2, value preserved
	const Tuple<TupleTestObject> t5(TupleTestObject(42));
	Tuple<TupleTestObjectAlt> t6(t5);
	ASSERT_EQ(t6.Get<0>().value, 42);
	ASSERT_EQ(t6.Get<0>().state, 2);

	// multi-element cross-type: Tuple<TupleTestObject, int> -> Tuple<TupleTestObjectAlt, double>
	const Tuple<TupleTestObject, int> t7(TupleTestObject(99), 7);
	Tuple<TupleTestObjectAlt, double> t8(t7);
	ASSERT_EQ(t8.Get<0>().value, 99);
	ASSERT_EQ(t8.Get<0>().state, 2);
	double t8_1 = t8.Get<1>();
	ASSERT_DOUBLE_EQ(t8_1, 7.0);
}

TEST(Tuple, CrossTypeMoveConstructorTest)
{
	// Tuple<int, float> -> Tuple<double, double>: implicit numeric conversion via move ctor
	Tuple<int, float> t1(3, 1.5f);
	Tuple<double, double> t2(std::move(t1));
	double t2_0 = t2.Get<0>();
	double t2_1 = t2.Get<1>();
	ASSERT_DOUBLE_EQ(t2_0, 3.0);
	ASSERT_DOUBLE_EQ(t2_1, 1.5);

	// Tuple<TupleTestObject, TupleTestObject> -> Tuple<TupleTestObjectAlt, TupleTestObjectAlt>: state=3, value preserved
	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(10), TupleTestObject(20));
	Tuple<TupleTestObjectAlt, TupleTestObjectAlt> t4(std::move(t3));
	ASSERT_EQ(t4.Get<0>().value, 10);
	ASSERT_EQ(t4.Get<0>().state, 3);
	ASSERT_EQ(t4.Get<1>().value, 20);
	ASSERT_EQ(t4.Get<1>().state, 3);

	// Tuple<TupleTestObject> -> Tuple<TupleTestObjectAlt>: state=3, value preserved
	Tuple<TupleTestObject> t5(TupleTestObject(42));
	Tuple<TupleTestObjectAlt> t6(std::move(t5));
	ASSERT_EQ(t6.Get<0>().value, 42);
	ASSERT_EQ(t6.Get<0>().state, 3);

	// multi-element cross-type: Tuple<TupleTestObject, int> -> Tuple<TupleTestObjectAlt, double>
	Tuple<TupleTestObject, int> t7(TupleTestObject(99), 7);
	Tuple<TupleTestObjectAlt, double> t8(std::move(t7));
	ASSERT_EQ(t8.Get<0>().value, 99);
	ASSERT_EQ(t8.Get<0>().state, 3);
	double t8_1 = t8.Get<1>();
	ASSERT_DOUBLE_EQ(t8_1, 7.0);
}

TEST(Tuple, CrossTypeCopyAssignmentTest)
{
	// Tuple<int, float> -> Tuple<double, double>: implicit numeric conversion via copy assignment
	const Tuple<int, float> t1(3, 1.5f);
	Tuple<double, double> t2;
	t2 = t1;
	double t2_0 = t2.Get<0>();
	double t2_1 = t2.Get<1>();
	ASSERT_DOUBLE_EQ(t2_0, 3.0);
	ASSERT_DOUBLE_EQ(t2_1, 1.5);

	// Tuple<TupleTestObject, TupleTestObject> -> Tuple<TupleTestObjectAlt, TupleTestObjectAlt>: state=4, value preserved
	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(10), TupleTestObject(20));
	Tuple<TupleTestObjectAlt, TupleTestObjectAlt> t4;
	t4 = t3;
	ASSERT_EQ(t4.Get<0>().value, 10);
	ASSERT_EQ(t4.Get<0>().state, 4);
	ASSERT_EQ(t4.Get<1>().value, 20);
	ASSERT_EQ(t4.Get<1>().state, 4);

	// Tuple<TupleTestObject> -> Tuple<TupleTestObjectAlt>: state=4, value preserved
	const Tuple<TupleTestObject> t5(TupleTestObject(42));
	Tuple<TupleTestObjectAlt> t6;
	t6 = t5;
	ASSERT_EQ(t6.Get<0>().value, 42);
	ASSERT_EQ(t6.Get<0>().state, 4);

	// multi-element cross-type: Tuple<TupleTestObject, int> -> Tuple<TupleTestObjectAlt, double>
	const Tuple<TupleTestObject, int> t7(TupleTestObject(99), 7);
	Tuple<TupleTestObjectAlt, double> t8;
	t8 = t7;
	ASSERT_EQ(t8.Get<0>().value, 99);
	ASSERT_EQ(t8.Get<0>().state, 4);
	double t8_1 = t8.Get<1>();
	ASSERT_DOUBLE_EQ(t8_1, 7.0);
}

TEST(Tuple, CrossTypeMoveAssignmentTest)
{
	// Tuple<int, float> -> Tuple<double, double>: implicit numeric conversion via move assignment
	Tuple<int, float> t1(3, 1.5f);
	Tuple<double, double> t2;
	t2 = std::move(t1);
	double t2_0 = t2.Get<0>();
	double t2_1 = t2.Get<1>();
	ASSERT_DOUBLE_EQ(t2_0, 3.0);
	ASSERT_DOUBLE_EQ(t2_1, 1.5);

	// Tuple<TupleTestObject, TupleTestObject> -> Tuple<TupleTestObjectAlt, TupleTestObjectAlt>: state=5, value preserved
	Tuple<TupleTestObject, TupleTestObject> t3(TupleTestObject(10), TupleTestObject(20));
	Tuple<TupleTestObjectAlt, TupleTestObjectAlt> t4;
	t4 = std::move(t3);
	ASSERT_EQ(t4.Get<0>().value, 10);
	ASSERT_EQ(t4.Get<0>().state, 5);
	ASSERT_EQ(t4.Get<1>().value, 20);
	ASSERT_EQ(t4.Get<1>().state, 5);

	// Tuple<TupleTestObject> -> Tuple<TupleTestObjectAlt>: state=5, value preserved
	Tuple<TupleTestObject> t5(TupleTestObject(42));
	Tuple<TupleTestObjectAlt> t6;
	t6 = std::move(t5);
	ASSERT_EQ(t6.Get<0>().value, 42);
	ASSERT_EQ(t6.Get<0>().state, 5);

	// multi-element cross-type: Tuple<TupleTestObject, int> -> Tuple<TupleTestObjectAlt, double>
	Tuple<TupleTestObject, int> t7(TupleTestObject(99), 7);
	Tuple<TupleTestObjectAlt, double> t8;
	t8 = std::move(t7);
	ASSERT_EQ(t8.Get<0>().value, 99);
	ASSERT_EQ(t8.Get<0>().state, 5);
	double t8_1 = t8.Get<1>();
	ASSERT_DOUBLE_EQ(t8_1, 7.0);
}

TEST(Tuple, GetTest)
{
	Tuple<int, double, char> t(100, 3.14, 'z');
	int t_0 = t.Get<0>();
	double t_1 = t.Get<1>();
	char t_2 = t.Get<2>();
	ASSERT_EQ(t_0, 100);
	ASSERT_DOUBLE_EQ(t_1, 3.14);
	ASSERT_EQ(t_2, 'z');

	// const get
	const Tuple<int, double, char> ct(200, 2.71, 'a');
	int ct_0 = ct.Get<0>();
	double ct_1 = ct.Get<1>();
	char ct_2 = ct.Get<2>();
	ASSERT_EQ(ct_0, 200);
	ASSERT_DOUBLE_EQ(ct_1, 2.71);
	ASSERT_EQ(ct_2, 'a');
}

TEST(Tuple, GetModifyTest)
{
	Tuple<int, double> t(1, 2.0);
	t.Get<0>() = 42;
	t.Get<1>() = 9.9;
	int val0 = t.Get<0>();
	double val1 = t.Get<1>();
	ASSERT_EQ(val0, 42);
	ASSERT_DOUBLE_EQ(val1, 9.9);
}

TEST(Tuple, GetRestTest)
{
	Tuple<int, double, char> t(1, 2.5, 'b');
	auto& rest0 = t.GetRest<0>();
	double rest0_0 = rest0.Get<0>();
	char rest0_1 = rest0.Get<1>();
	ASSERT_DOUBLE_EQ(rest0_0, 2.5);
	ASSERT_EQ(rest0_1, 'b');

	const Tuple<int, double, char> ct(3, 4.5, 'c');
	const auto& crest0 = ct.GetRest<0>();
	double crest0_0 = crest0.Get<0>();
	char crest0_1 = crest0.Get<1>();
	ASSERT_DOUBLE_EQ(crest0_0, 4.5);
	ASSERT_EQ(crest0_1, 'c');
}

TEST(Tuple, MakeTupleTest)
{
	auto t = MakeTuple(1, 2.0, 'k');
	int t_0 = t.Get<0>();
	double t_1 = t.Get<1>();
	char t_2 = t.Get<2>();
	ASSERT_EQ(t_0, 1);
	ASSERT_DOUBLE_EQ(t_1, 2.0);
	ASSERT_EQ(t_2, 'k');

	auto t2 = MakeTuple(std::string("hello"));
	std::string t2_0 = t2.Get<0>();
	ASSERT_EQ(t2_0, std::string("hello"));
}

TEST(Tuple, ForwardAsTupleTest)
{
	int x = 10;
	double y = 3.14;
	auto t = ForwardAsTuple(x, y);
	int t_0 = t.Get<0>();
	double t_1 = t.Get<1>();
	ASSERT_EQ(t_0, 10);
	ASSERT_DOUBLE_EQ(t_1, 3.14);

	// modifying through the reference tuple modifies the original
	t.Get<0>() = 99;
	ASSERT_EQ(x, 99);
}

TEST(Tuple, ConcatTuplesTest)
{
	auto t1 = MakeTuple(1, 2.0);
	auto t2 = MakeTuple('a', true);
	auto t3 = ConcatTuples(t1, t2);
	int t3_0 = t3.Get<0>();
	double t3_1 = t3.Get<1>();
	char t3_2 = t3.Get<2>();
	bool t3_3 = t3.Get<3>();
	ASSERT_EQ(t3_0, 1);
	ASSERT_DOUBLE_EQ(t3_1, 2.0);
	ASSERT_EQ(t3_2, 'a');
	ASSERT_EQ(t3_3, true);
}

TEST(Tuple, ConcatMultipleTuplesTest)
{
	auto t1 = MakeTuple(1);
	auto t2 = MakeTuple(2.0);
	auto t3 = MakeTuple('x');
	auto result = ConcatTuples(t1, t2, t3);
	int result_0 = result.Get<0>();
	double result_1 = result.Get<1>();
	char result_2 = result.Get<2>();
	ASSERT_EQ(result_0, 1);
	ASSERT_DOUBLE_EQ(result_1, 2.0);
	ASSERT_EQ(result_2, 'x');
}

TEST(Tuple, IsTupleConceptTest)
{
	static_assert(IsTuple<Tuple<>>);
	static_assert(IsTuple<Tuple<int>>);
	static_assert(IsTuple<Tuple<int, double>>);
	static_assert(!IsTuple<int>);
	static_assert(!IsTuple<double>);
}
