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
#include "Utility/Optional.hpp"
#include "Function.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct test_optional_object_class
{
	test_optional_object_class()
	{
		i = 1;
	}
	~test_optional_object_class()
	{
		i = 0;
	}
	test_optional_object_class(const test_optional_object_class& obj)
	{
		i = 2;
	}
	test_optional_object_class(test_optional_object_class&& obj)
	{
		i = 3;
	}

	test_optional_object_class& operator=(const test_optional_object_class& obj)
	{
		i = 4;
		return *this;
	}
	test_optional_object_class& operator=(test_optional_object_class&& obj)
	{
		i = 5;
		return *this;
	}

	int i;
};

struct test_optional_object_release_class
{
	test_optional_object_release_class(const Function<void()> func)
		: m_Function(func)
	{
	}

	~test_optional_object_release_class()
	{
		m_Function();
	}

	Function<void()> m_Function;
};

TEST(Optional, InstanceTest)
{
	Optional<test_optional_object_class> empty_opt;
	ASSERT_FALSE(empty_opt.HasValue());

	Optional<test_optional_object_class> opt1{test_optional_object_class()};
	ASSERT_TRUE(opt1.HasValue());
	ASSERT_EQ(opt1.Get().i, 3);

	bool has_destruct = false;
	{
		const Optional<test_optional_object_release_class> opt2([&]() {
			has_destruct = true;
		});
		ASSERT_TRUE(opt2.HasValue());
		ASSERT_FALSE(has_destruct);
	}
	ASSERT_TRUE(has_destruct);

	Optional<test_optional_object_class> opt3(OptionalTag::EmptyOptional);
	ASSERT_FALSE(opt3.HasValue());

	Optional<test_optional_object_class> opt4(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 1);
}

TEST(Optional, CopyConstructionTest)
{
	Optional<test_optional_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_optional_object_class> opt3(opt1);
	ASSERT_FALSE(opt3.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt4(opt2);
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 2);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);
}

TEST(Optional, MoveConstructionTest)
{
	Optional<test_optional_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_optional_object_class> opt3(std::move(opt1));
	ASSERT_FALSE(opt3.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt4(std::move(opt2));
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 3);
	ASSERT_FALSE(opt2.HasValue());
}

TEST(Optional, CopyAssignmentTest)
{
	Optional<test_optional_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_optional_object_class> opt3;
	ASSERT_FALSE(opt3.HasValue());

	Optional<test_optional_object_class> opt4;
	ASSERT_FALSE(opt4.HasValue());

	opt4 = opt1;

	ASSERT_FALSE(opt4.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	opt4 = opt2;

	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 2);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	opt4 = opt2;

	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 4);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	opt4 = opt3;

	ASSERT_FALSE(opt4.HasValue());
	ASSERT_FALSE(opt3.HasValue());
}

TEST(Optional, MoveAssignmentTest)
{
	Optional<test_optional_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_optional_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_optional_object_class> opt3(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt3.HasValue());
	ASSERT_EQ(opt3.Get().i, 1);

	Optional<test_optional_object_class> opt4;
	ASSERT_FALSE(opt4.HasValue());

	Optional<test_optional_object_class> opt5;
	ASSERT_FALSE(opt5.HasValue());

	opt5 = std::move(opt1);

	ASSERT_FALSE(opt5.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	opt5 = std::move(opt2);

	ASSERT_TRUE(opt5.HasValue());
	ASSERT_EQ(opt5.Get().i, 3);
	ASSERT_FALSE(opt2.HasValue());

	opt5 = std::move(opt3);

	ASSERT_TRUE(opt5.HasValue());
	ASSERT_EQ(opt5.Get().i, 5);
	ASSERT_FALSE(opt3.HasValue());

	opt5 = std::move(opt4);

	ASSERT_FALSE(opt5.HasValue());
	ASSERT_FALSE(opt4.HasValue());
}

TEST(Optional, GetTest)
{
	Optional<int> opt(1);
	ASSERT_EQ(opt.Get(), 1);
	ASSERT_TRUE((std::is_same_v<decltype(opt.Get()), int&>));

	const Optional<int> copt(2);
	ASSERT_EQ(copt.Get(), 2);
	ASSERT_TRUE((std::is_same_v<decltype(copt.Get()), const int&>));
}

TEST(Optional, SetTest)
{
	Optional<test_optional_object_class> opt;
	ASSERT_FALSE(opt.HasValue());

	opt.Set();

	ASSERT_TRUE(opt.HasValue());
	ASSERT_EQ(opt.Get().i, 1);
}

TEST(Optional, ClearTest)
{
	Optional<test_optional_object_class> opt(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt.HasValue());
	ASSERT_EQ(opt.Get().i, 1);

	opt.Clear();

	ASSERT_FALSE(opt.HasValue());

	opt.Set(test_optional_object_class());

	ASSERT_TRUE(opt.HasValue());
	ASSERT_EQ(opt.Get().i, 3);
}

TEST(Optional, EqualityTest)
{
	Optional<int> opt1;
	ASSERT_FALSE(opt1.HasValue());

	const Optional<int> opt2(0);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get(), 0);

	Optional<int> opt3(1);
	ASSERT_TRUE(opt3.HasValue());
	ASSERT_EQ(opt3.Get(), 1);

	ASSERT_TRUE(opt1 == opt1);
	ASSERT_TRUE(opt2 == opt2);
	ASSERT_TRUE(opt3 == 1);
	ASSERT_TRUE(1 == opt3);

	ASSERT_FALSE(opt1 == opt2);
	ASSERT_FALSE(opt2 == opt1);
	ASSERT_FALSE(opt2 == opt3);
	ASSERT_FALSE(opt3 == opt2);
	ASSERT_FALSE(opt2 == 1);
	ASSERT_FALSE(1 == opt2);

	ASSERT_TRUE(opt1 != opt2);
	ASSERT_TRUE(opt2 != opt1);
	ASSERT_TRUE(opt2 != opt3);
	ASSERT_TRUE(opt3 != opt2);
	ASSERT_TRUE(opt2 != 1);
	ASSERT_TRUE(1 != opt2);

	ASSERT_FALSE(opt1 != opt1);
	ASSERT_FALSE(opt2 != opt2);
	ASSERT_FALSE(opt3 != 1);
	ASSERT_FALSE(1 != opt3);
}