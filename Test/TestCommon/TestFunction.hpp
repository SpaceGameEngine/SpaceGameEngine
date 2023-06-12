/*
Copyright 2023 creatorlxd

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
#include "Function.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

void func_(int i)
{
}
void func_2(int i, int i2)
{
}
int func_3(int i, int i2)
{
	return 0;
}

struct functor
{
	int operator()()
	{
		return 1;
	}

private:
	int array_content[32];
};

struct test_func_class
{
	int test()
	{
		return 1;
	}
	int test2() const
	{
		return 2;
	}
};

struct test_func_class2
{
	test_func_class2()
		: mi(0)
	{
	}
	test_func_class2(const test_func_class2& t)
		: mi(1)
	{
	}
	test_func_class2& operator=(const test_func_class2& t)
	{
		mi = 2;
		return *this;
	}
	test_func_class2(test_func_class2&& t)
		: mi(3)
	{
	}
	test_func_class2& operator=(test_func_class2&& t)
	{
		mi = 4;
		return *this;
	}
	~test_func_class2()
	{
		++destruction_count;
		mi = -1;
	}
	int operator()() const
	{
		return mi;
	}

	int mi;
	static int destruction_count;
};

int test_func_class2::destruction_count = 0;

class test_func_class3 : public test_func_class2
{
public:
	using test_func_class2::operator=;

private:
	int array_content[32];
};

TEST(Function, IsCorrectFunctionTest)
{
	ASSERT_TRUE((IsCorrectFunction<decltype(func_), void(int)>::Value));
	ASSERT_FALSE((IsCorrectFunction<decltype(func_2), void(int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(func_2), void(int, int)>::Value));
	ASSERT_FALSE((IsCorrectFunction<decltype(func_3), void(int, int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(func_3), int(int, int)>::Value));
	ASSERT_TRUE((IsCorrectFunction<functor, int(void)>::Value));
	ASSERT_FALSE((IsCorrectFunction<int, void()>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(&test_func_class::test), int(test_func_class*)>::Value));
	ASSERT_TRUE((IsCorrectFunction<decltype(&test_func_class::test2), int(const test_func_class*)>::Value));
}

TEST(Function, IsFunctionTest)
{
	Function<void()> func([]() {});
	ASSERT_TRUE(Function<void()>::IsFunction<decltype(func)>::Value);
	ASSERT_FALSE(Function<void()>::IsFunction<int>::Value);
}

TEST(Function, ConstructionTest)
{
	auto lambda = [](void) -> int { return 1; };
	Function<int(void)> func(lambda);	 // use lambda
	ASSERT_EQ((int (*)(void))lambda, (int (*)(void))func.Get<decltype(lambda)>());
	ASSERT_EQ(lambda(), func());

	auto mutable_lambda = [i = 0](void) mutable -> int { return ++i; };
	Function<int(void)> func2(mutable_lambda);	  // use mutable lambda
	ASSERT_EQ(memcmp(&mutable_lambda, &(func2.Get<decltype(mutable_lambda)>()), sizeof(mutable_lambda)), 0);
	ASSERT_EQ(mutable_lambda(), func2());

	Function<void(int)> func3(&func_);	  // use function pointer
	ASSERT_EQ(func3.Get<decltype(&func_)>(), &func_);

	Function<int(test_func_class*)> func4 = &test_func_class::test;	   // use member function pointer
	test_func_class tc;
	ASSERT_EQ(func4(&tc), tc.test());

	Function<int(void)> func5 = functor();	  // use functor
	ASSERT_EQ(func5(), functor()());

	Function<int(void)> func6((int (*)(void)) nullptr);	   // use nullptr
	ASSERT_FALSE(func6.IsValid());
}

TEST(Function, AssignmentTest)
{
	auto lambda = [](void) -> int { return 1; };
	Function<int(void)> func;
	func = lambda;	  // use lambda
	ASSERT_EQ((int (*)(void))lambda, (int (*)(void))func.Get<decltype(lambda)>());
	ASSERT_EQ(lambda(), func());

	auto mutable_lambda = [i = 0](void) mutable -> int { return ++i; };
	Function<int(void)> func2;
	func2 = mutable_lambda;	   // use mutable lambda
	ASSERT_EQ(memcmp(&mutable_lambda, &(func2.Get<decltype(mutable_lambda)>()), sizeof(mutable_lambda)), 0);
	ASSERT_EQ(mutable_lambda(), func2());

	Function<void(int)> func3;
	func3 = &func_;	   // use function pointer
	ASSERT_EQ(func3.Get<decltype(&func_)>(), &func_);

	Function<int(test_func_class*)> func4;
	func4 = &test_func_class::test;	   // use member function pointer
	test_func_class tc;
	ASSERT_EQ(func4(&tc), tc.test());

	Function<int(void)> func5;
	func5 = functor();	  // use functor
	ASSERT_EQ(func5(), functor()());

	Function<int(void)> func6;
	func6 = ((int (*)(void)) nullptr);	  // use nullptr
	ASSERT_FALSE(func6.IsValid());

	Function<int(void)> func7 = test_func_class2();	   // test assignment with small size content
	ASSERT_EQ(func7(), 3);
	func7 = test_func_class2();
	ASSERT_EQ(func7(), 4);
	ASSERT_FALSE((std::is_assignable_v<test_func_class2, decltype(mutable_lambda)>));
	int destruction_count = test_func_class2::destruction_count;
	func7 = mutable_lambda;	   // assign with small unassignable content
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	ASSERT_EQ(func7(), 2);	  // 2 because of the calling of mutable_lambda above
	func7 = mutable_lambda;
	ASSERT_EQ(func7(), 2);	  // new mutable lambda
	auto mutable_lambda2 = [i = 0, j = 0, k = 0]() mutable -> int {
		return ++i + ++j + ++k;
	};
	func7 = mutable_lambda2;	// assign with large unassignable content
	ASSERT_EQ(func7(), 3);
	func7 = mutable_lambda2;
	ASSERT_EQ(func7(), 3);		   // new mutable lambda
	func7 = test_func_class3();	   // assign with large content
	ASSERT_EQ(func7(), 3);
	func7 = test_func_class3();
	ASSERT_EQ(func7(), 4);
	destruction_count = test_func_class2::destruction_count;
	func7 = []() { return 1; };	   // assign with small content;
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	ASSERT_EQ(func7(), 1);
}

TEST(Function, CopyConstructionTest)
{
	Function<int(void)> func = test_func_class2();
	Function<int(void)> func2(func);
	ASSERT_EQ(func2(), 1);

	Function<int(void)> func3 = test_func_class3();
	Function<int(void)> func4(func3);
	ASSERT_EQ(func4(), 1);
}

TEST(Function, MoveConstructionTest)
{
	Function<int(void)> func = test_func_class2();
	int destruction_count = test_func_class2::destruction_count;
	Function<int(void)> func2(std::move(func));
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	ASSERT_EQ(func2(), 3);

	Function<int(void)> func3 = test_func_class3();
	destruction_count = test_func_class2::destruction_count;
	Function<int(void)> func4(std::move(func3));
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 0);
	ASSERT_EQ(func4(), 3);
}

TEST(Function, CopyAssignmentTest)
{
	Function<int(void)> func = test_func_class2();
	Function<int(void)> func2 = test_func_class2();
	func = func2;
	ASSERT_EQ(func(), 2);	 // same type
	Function<int(void)> func3 = test_func_class3();
	func = func3;
	ASSERT_EQ(func(), 1);	 // small to large
	func = func2;
	ASSERT_EQ(func(), 1);	 // large to small
	Function<int(void)> func4;
	int destruction_count = test_func_class2::destruction_count;
	func = func4;	 // small to empty
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	ASSERT_FALSE(func.IsValid());
	func = func3;	 // empty to large
	ASSERT_EQ(func(), 1);
	destruction_count = test_func_class2::destruction_count;
	func = func4;	 // large to empty
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	ASSERT_FALSE(func.IsValid());
	func = func2;	 // empty to small
	ASSERT_EQ(func(), 1);
}

TEST(Function, MoveAssignmentTest)
{
	Function<int(void)> func = test_func_class2();
	int destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class2());	   // same type small
	ASSERT_EQ(func(), 4);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 2);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class3());	   // small to large
	ASSERT_EQ(func(), 3);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 2);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class3());	   // same type large
	ASSERT_EQ(func(), 3);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 2);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class2());	   // large to small
	ASSERT_EQ(func(), 3);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 3);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>();	 // small to empty
	ASSERT_FALSE(func.IsValid());
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class3());	   // empty to large
	ASSERT_EQ(func(), 3);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>();	 // large to empty
	ASSERT_FALSE(func.IsValid());
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 1);
	destruction_count = test_func_class2::destruction_count;
	func = Function<int(void)>(test_func_class2());	   // empty to small
	ASSERT_EQ(func(), 3);
	ASSERT_EQ(test_func_class2::destruction_count - destruction_count, 2);
}

// todo

TEST(Function, InvokeTest)
{
	Function<int()> func([]() {	   // use lambda
		return 1;
	});
	const Function<int()>& cfunc_ref = func;

	ASSERT_EQ(func(), 1);
	ASSERT_EQ(cfunc_ref(), 1);

	Function<int()> func2([i = 0]() mutable {	 // use mutable lambda
		return ++i;
	});
	const Function<int()>& cfunc_ref2 = func2;

	ASSERT_EQ(func2(), 1);
	ASSERT_EQ(func2(), 2);
	ASSERT_EQ(cfunc_ref2(), 3);
	ASSERT_EQ(cfunc_ref2(), 4);

	Function<int(int, int)> func3(&func_3);	   // use function pointer
	const Function<int(int, int)>& cfunc_ref3 = func3;

	ASSERT_EQ(func3(0, 0), 0);
	ASSERT_EQ(cfunc_ref3(0, 0), 0);

	Function<int(test_func_class*)> func4(&test_func_class::test);	  // use member function pointer
	const Function<int(test_func_class*)>& cfunc_ref4 = func4;
	test_func_class tc;

	ASSERT_EQ(func4(&tc), 1);
	ASSERT_EQ(cfunc_ref4(&tc), 1);

	Function<int(void)> func5 = functor();	  // use functor
	const Function<int(void)>& cfunc_ref5 = func5;

	ASSERT_EQ(func5(), 1);
	ASSERT_EQ(cfunc_ref5(), 1);
}

TEST(Function, GetMetaDataTest)
{
	Function<void(int)> func(&func_);
	ASSERT_EQ(func.GetMetaData(), GetMetaData<decltype(&func_)>());
}
