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
#include "Utility/Variant.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct TestVariantMoveOnlyType
{
	int value;
	TestVariantMoveOnlyType(int v)
		: value(v)
	{
	}
	TestVariantMoveOnlyType(const TestVariantMoveOnlyType&) = delete;
	TestVariantMoveOnlyType(TestVariantMoveOnlyType&& other)
		: value(other.value)
	{
		other.value = 0;
	}
	TestVariantMoveOnlyType& operator=(const TestVariantMoveOnlyType&) = delete;
	TestVariantMoveOnlyType& operator=(TestVariantMoveOnlyType&& other)
	{
		value = other.value;
		other.value = 0;
		return *this;
	}
};

struct TestVariantMoveTracker
{
	int value;
	bool moved = false;
	TestVariantMoveTracker(int v)
		: value(v)
	{
	}
	TestVariantMoveTracker(const TestVariantMoveTracker&) = default;
	TestVariantMoveTracker(TestVariantMoveTracker&& other)
		: value(other.value), moved(false)
	{
		other.moved = true;
	}
	TestVariantMoveTracker& operator=(const TestVariantMoveTracker&) = default;
	TestVariantMoveTracker& operator=(TestVariantMoveTracker&& other)
	{
		value = other.value;
		moved = false;
		other.moved = true;
		return *this;
	}
};

struct TestVariantComplexTypeForEmplace
{
	int a;
	float b;
	TestVariantComplexTypeForEmplace(int x, float y)
		: a(x), b(y)
	{
	}
};

struct TestVariantTrackedType
{
	inline static int destructor_count = 0;
	TestVariantTrackedType() = default;
	~TestVariantTrackedType()
	{
		destructor_count++;
	}
};

struct TestVariantComplexType
{
	int* data;
	TestVariantComplexType(int value)
		: data(new int(value))
	{
	}
	~TestVariantComplexType()
	{
		delete data;
	}
	TestVariantComplexType(const TestVariantComplexType& other)
		: data(new int(*other.data))
	{
	}
	TestVariantComplexType(TestVariantComplexType&& other)
		: data(other.data)
	{
		other.data = nullptr;
	}
	TestVariantComplexType& operator=(const TestVariantComplexType& other)
	{
		if (this != &other)
		{
			delete data;
			data = new int(*other.data);
		}
		return *this;
	}
	TestVariantComplexType& operator=(TestVariantComplexType&& other)
	{
		if (this != &other)
		{
			delete data;
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}
};

struct TestVariantTestVisitor
{
	template<SizeType Index>
	int Visit(int value)
	{
		return value * 2;
	}

	template<SizeType Index>
	int Visit(float value)
	{
		return static_cast<int>(value * 3);
	}

	template<SizeType Index>
	int Visit(double value)
	{
		return static_cast<int>(value * 4);
	}
};

struct TestVariantConstTestVisitor
{
	template<SizeType Index>
	int Visit(const int& value)
	{
		return value + 1;
	}

	template<SizeType Index>
	int Visit(const float& value)
	{
		return static_cast<int>(value) + 2;
	}

	template<SizeType Index>
	int Visit(const double& value)
	{
		return static_cast<int>(value) + 3;
	}
};

struct TestVariantVisitorStateTracker
{
	mutable int state = 0;

	template<SizeType Index>
	int Visit(int value) &
	{
		state = 1;
		return value * 2;
	}

	template<SizeType Index>
	int Visit(float value) &
	{
		state = 1;
		return static_cast<int>(value * 3);
	}

	template<SizeType Index>
	int Visit(double value) &
	{
		state = 1;
		return static_cast<int>(value * 4);
	}

	template<SizeType Index>
	int Visit(int value) const&
	{
		state = 2;
		return value + 10;
	}

	template<SizeType Index>
	int Visit(float value) const&
	{
		state = 2;
		return static_cast<int>(value) + 20;
	}

	template<SizeType Index>
	int Visit(double value) const&
	{
		state = 2;
		return static_cast<int>(value) + 30;
	}

	template<SizeType Index>
	int Visit(int value) &&
	{
		state = 3;
		return value + 100;
	}

	template<SizeType Index>
	int Visit(float value) &&
	{
		state = 3;
		return static_cast<int>(value) + 200;
	}

	template<SizeType Index>
	int Visit(double value) &&
	{
		state = 3;
		return static_cast<int>(value) + 300;
	}
};

TEST(Variant, SizeAndAlignmentTest)
{
	struct A
	{
		UInt8 a;
	};
	struct B
	{
		UInt16 b;
	};
	struct C
	{
		UInt32 c;
	};
	ASSERT_EQ((Variant<A, B, C>::Size), sizeof(C));
	ASSERT_EQ((Variant<A, B, C>::Alignment), alignof(C));
}

TEST(Variant, DefaultConstructorTest)
{
	Variant<int, float, double> variant;
	ASSERT_EQ(variant.GetTypeIndex(), 0);
	ASSERT_EQ(variant.Get<0>(), 0);

	Variant<EmptyType, TestVariantComplexTypeForEmplace> variant2;
	ASSERT_EQ(variant2.GetTypeIndex(), 0);
}

TEST(Variant, ConstructorWithTypeIndexTest)
{
	Variant<int, float, double> variant1(InPlaceIndex<1>, 1.0f);
	ASSERT_EQ(variant1.GetTypeIndex(), 1);
	ASSERT_FLOAT_EQ(variant1.Get<1>(), 1.0f);

	Variant<int, float, double> variant2(InPlaceIndex<2>, 3.14);
	ASSERT_EQ(variant2.GetTypeIndex(), 2);
	ASSERT_DOUBLE_EQ(variant2.Get<2>(), 3.14);
}

TEST(Variant, CopyConstructorTest)
{
	Variant<int, float, double> variant1;
	variant1.Emplace<1>(3.14f);

	Variant<int, float, double> variant2(variant1);
	ASSERT_EQ(variant2.GetTypeIndex(), 1);
	ASSERT_FLOAT_EQ(variant2.Get<1>(), 3.14f);
}

TEST(Variant, MoveConstructorTest)
{
	Variant<int, TestVariantMoveOnlyType> variant1;
	variant1.Emplace<1>(42);

	Variant<int, TestVariantMoveOnlyType> variant2(std::move(variant1));
	ASSERT_EQ(variant2.GetTypeIndex(), 1);
	ASSERT_EQ(variant2.Get<1>().value, 42);
}

TEST(Variant, CopyAssignmentTest)
{
	// Same type assignment
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<0>(10);

		Variant<int, float, double> variant2;
		variant2.Emplace<0>(20);

		variant2 = variant1;
		ASSERT_EQ(variant2.GetTypeIndex(), 0);
		ASSERT_EQ(variant2.Get<0>(), 10);
	}

	// Different type assignment
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<1>(3.14f);

		Variant<int, float, double> variant2;
		variant2.Emplace<0>(10);

		variant2 = variant1;
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_FLOAT_EQ(variant2.Get<1>(), 3.14f);
	}
}

TEST(Variant, MoveAssignmentTest)
{
	// Same type assignment
	{
		Variant<int, TestVariantMoveTracker> variant1;
		variant1.Emplace<1>(100);

		Variant<int, TestVariantMoveTracker> variant2;
		variant2.Emplace<1>(200);

		variant2 = std::move(variant1);
		ASSERT_TRUE(variant1.Get<1>().moved);
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_EQ(variant2.Get<1>().value, 100);
	}

	// Different type assignment
	{
		Variant<int, TestVariantMoveTracker> variant1;
		variant1.Emplace<1>(100);

		Variant<int, TestVariantMoveTracker> variant2;
		variant2.Emplace<0>(42);

		variant2 = std::move(variant1);
		ASSERT_TRUE(variant1.Get<1>().moved);
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_EQ(variant2.Get<1>().value, 100);
	}
}

TEST(Variant, EmplaceTest)
{
	// Emplace with constructor arguments
	{
		Variant<int, TestVariantComplexTypeForEmplace, double> variant;
		auto& ref = variant.Emplace<1>(42, 3.14f);

		ASSERT_EQ(variant.GetTypeIndex(), 1);
		ASSERT_EQ(ref.a, 42);
		ASSERT_FLOAT_EQ(ref.b, 3.14f);
		ASSERT_EQ(variant.Get<1>().a, 42);
	}

	// Emplace changing type
	{
		Variant<int, float, double> variant;
		variant.Emplace<0>(10);
		ASSERT_EQ(variant.GetTypeIndex(), 0);

		variant.Emplace<1>(3.14f);
		ASSERT_EQ(variant.GetTypeIndex(), 1);
		ASSERT_FLOAT_EQ(variant.Get<1>(), 3.14f);
	}
}

TEST(Variant, GetTest)
{
	// Non-const Get
	{
		Variant<int, float, double> variant;
		variant.Emplace<2>(2.718);

		ASSERT_EQ(variant.GetTypeIndex(), 2);
		ASSERT_DOUBLE_EQ(variant.Get<2>(), 2.718);
	}

	// Const Get
	{
		Variant<int, float, double> variant;
		variant.Emplace<1>(1.5f);

		const auto& const_variant = variant;
		ASSERT_FLOAT_EQ(const_variant.Get<1>(), 1.5f);
	}
}

TEST(Variant, QueryTest)
{
	// Non-const Query - success
	{
		Variant<int, float, double> variant;
		variant.Emplace<1>(2.5f);

		auto* ptr = variant.Query<1>();
		ASSERT_NE(ptr, nullptr);
		ASSERT_FLOAT_EQ(*ptr, 2.5f);
	}

	// Non-const Query - failure
	{
		Variant<int, float, double> variant;
		variant.Emplace<0>(42);

		auto* ptr = variant.Query<1>();
		ASSERT_EQ(ptr, nullptr);
	}

	// Const Query - success
	{
		Variant<int, float, double> variant;
		variant.Emplace<2>(3.14159);

		const auto& const_variant = variant;
		auto* ptr = const_variant.Query<2>();
		ASSERT_NE(ptr, nullptr);
		ASSERT_DOUBLE_EQ(*ptr, 3.14159);
	}

	// Const Query - failure
	{
		Variant<int, float, double> variant;
		variant.Emplace<0>(42);

		const auto& const_variant = variant;
		auto* ptr = const_variant.Query<2>();
		ASSERT_EQ(ptr, nullptr);
	}
}

TEST(Variant, VisitTest)
{
	// Non-const Visit (by value)
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<0>(10);
		ASSERT_EQ(variant1.Visit(TestVariantTestVisitor{}), 20);

		Variant<int, float, double> variant2;
		variant2.Emplace<1>(5.0f);
		ASSERT_EQ(variant2.Visit(TestVariantTestVisitor{}), 15);

		Variant<int, float, double> variant3;
		variant3.Emplace<2>(7.0);
		ASSERT_EQ(variant3.Visit(TestVariantTestVisitor{}), 28);
	}

	// Const Visit
	{
		Variant<int, float, double> variant;
		variant.Emplace<1>(10.0f);

		const auto& const_variant = variant;
		ASSERT_EQ(const_variant.Visit(TestVariantConstTestVisitor{}), 12);
	}

	// Test Visitor passed as lvalue reference (&)
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<0>(5);

		TestVariantVisitorStateTracker visitor1;
		ASSERT_EQ(variant1.Visit(visitor1), 10);
		ASSERT_EQ(visitor1.state, 1);

		Variant<int, float, double> variant2;
		variant2.Emplace<1>(5.0f);

		TestVariantVisitorStateTracker visitor2;
		ASSERT_EQ(variant2.Visit(visitor2), 15);
		ASSERT_EQ(visitor2.state, 1);

		Variant<int, float, double> variant3;
		variant3.Emplace<2>(5.0);

		TestVariantVisitorStateTracker visitor3;
		ASSERT_EQ(variant3.Visit(visitor3), 20);
		ASSERT_EQ(visitor3.state, 1);
	}

	// Test Visitor passed as const lvalue reference (const &)
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<0>(5);

		const TestVariantVisitorStateTracker const_visitor1;
		ASSERT_EQ(variant1.Visit(const_visitor1), 15);
		ASSERT_EQ(const_visitor1.state, 2);

		Variant<int, float, double> variant2;
		variant2.Emplace<1>(5.0f);

		const TestVariantVisitorStateTracker const_visitor2;
		ASSERT_EQ(variant2.Visit(const_visitor2), 25);
		ASSERT_EQ(const_visitor2.state, 2);

		Variant<int, float, double> variant3;
		variant3.Emplace<2>(5.0);

		const TestVariantVisitorStateTracker const_visitor3;
		ASSERT_EQ(variant3.Visit(const_visitor3), 35);
		ASSERT_EQ(const_visitor3.state, 2);
	}

	// Test Visitor passed as rvalue reference (&&)
	{
		Variant<int, float, double> variant1;
		variant1.Emplace<0>(5);

		TestVariantVisitorStateTracker rvalue_visitor1;
		ASSERT_EQ(variant1.Visit(std::move(rvalue_visitor1)), 105);
		ASSERT_EQ(rvalue_visitor1.state, 3);

		Variant<int, float, double> variant2;
		variant2.Emplace<1>(5.0f);

		TestVariantVisitorStateTracker rvalue_visitor2;
		ASSERT_EQ(variant2.Visit(std::move(rvalue_visitor2)), 205);
		ASSERT_EQ(rvalue_visitor2.state, 3);

		Variant<int, float, double> variant3;
		variant3.Emplace<2>(5.0);

		TestVariantVisitorStateTracker rvalue_visitor3;
		ASSERT_EQ(variant3.Visit(std::move(rvalue_visitor3)), 305);
		ASSERT_EQ(rvalue_visitor3.state, 3);
	}
}

TEST(Variant, DestructorTest)
{
	// Destructor called on Variant destruction
	{
		TestVariantTrackedType::destructor_count = 0;
		{
			Variant<int, TestVariantTrackedType> variant;
			variant.Emplace<1>();
		}
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 1);
	}

	// Destructor called on Emplace
	{
		TestVariantTrackedType::destructor_count = 0;
		Variant<int, TestVariantTrackedType> variant;
		variant.Emplace<1>();
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 0);

		variant.Emplace<0>(42);
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 1);
	}
}

TEST(Variant, ComplexTypeTest)
{
	Variant<int, TestVariantComplexType> variant1;
	variant1.Emplace<1>(100);
	ASSERT_EQ(*variant1.Get<1>().data, 100);

	Variant<int, TestVariantComplexType> variant2(variant1);
	ASSERT_EQ(*variant2.Get<1>().data, 100);
	ASSERT_EQ(*variant1.Get<1>().data, 100);

	Variant<int, TestVariantComplexType> variant3;
	variant3 = variant2;
	ASSERT_EQ(*variant3.Get<1>().data, 100);
}

struct TestUniqueVariantSimpleVisitor
{
	int operator()(int value)
	{
		return value * 2;
	}

	int operator()(float value)
	{
		return static_cast<int>(value * 3);
	}

	int operator()(double value)
	{
		return static_cast<int>(value * 4);
	}
};

struct TestUniqueVariantConstVisitor
{
	int operator()(const int& value) const
	{
		return value + 1;
	}

	int operator()(const float& value) const
	{
		return static_cast<int>(value) + 2;
	}

	int operator()(const double& value) const
	{
		return static_cast<int>(value) + 3;
	}
};

struct TestUniqueVariantStateTracker
{
	mutable int state = 0;

	int operator()(int value) &
	{
		state = 1;
		return value * 2;
	}

	int operator()(float value) &
	{
		state = 1;
		return static_cast<int>(value * 3);
	}

	int operator()(double value) &
	{
		state = 1;
		return static_cast<int>(value * 4);
	}

	int operator()(int value) const&
	{
		state = 2;
		return value + 10;
	}

	int operator()(float value) const&
	{
		state = 2;
		return static_cast<int>(value) + 20;
	}

	int operator()(double value) const&
	{
		state = 2;
		return static_cast<int>(value) + 30;
	}

	int operator()(int value) &&
	{
		state = 3;
		return value + 100;
	}

	int operator()(float value) &&
	{
		state = 3;
		return static_cast<int>(value) + 200;
	}

	int operator()(double value) &&
	{
		state = 3;
		return static_cast<int>(value) + 300;
	}
};

TEST(UniqueVariant, DefaultConstructorTest)
{
	UniqueVariant<int, float, double> variant;
	ASSERT_EQ(variant.GetTypeIndex(), 0);
	ASSERT_EQ((variant.Get<int>()), 0);

	UniqueVariant<EmptyType, TestVariantComplexTypeForEmplace> variant2;
	ASSERT_EQ(variant2.GetTypeIndex(), 0);
}

TEST(UniqueVariant, ConstructorWithTypeTest)
{
	UniqueVariant<int, float, double> variant1(InPlaceType<float>, 1.0f);
	ASSERT_EQ(variant1.GetTypeIndex(), 1);
	ASSERT_FLOAT_EQ((variant1.Get<float>()), 1.0f);
	UniqueVariant<int, float, double> variant2(InPlaceType<double>, 3.14);
	ASSERT_EQ(variant2.GetTypeIndex(), 2);
	ASSERT_DOUBLE_EQ((variant2.Get<double>()), 3.14);
}

TEST(UniqueVariant, CopyConstructorTest)
{
	UniqueVariant<int, float, double> variant1;
	variant1.Emplace<float>(3.14f);

	UniqueVariant<int, float, double> variant2(variant1);
	ASSERT_EQ(variant2.GetTypeIndex(), 1);
	ASSERT_FLOAT_EQ((variant2.Get<float>()), 3.14f);
}

TEST(UniqueVariant, MoveConstructorTest)
{
	UniqueVariant<int, TestVariantMoveOnlyType> variant1;
	variant1.Emplace<TestVariantMoveOnlyType>(42);

	UniqueVariant<int, TestVariantMoveOnlyType> variant2(std::move(variant1));
	ASSERT_EQ(variant2.GetTypeIndex(), 1);
	ASSERT_EQ((variant2.Get<TestVariantMoveOnlyType>().value), 42);
}

TEST(UniqueVariant, CopyAssignmentTest)
{
	// Same type assignment
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<int>(10);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<int>(20);

		variant2 = variant1;
		ASSERT_EQ(variant2.GetTypeIndex(), 0);
		ASSERT_EQ((variant2.Get<int>()), 10);
	}

	// Different type assignment
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<float>(3.14f);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<int>(10);

		variant2 = variant1;
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_FLOAT_EQ((variant2.Get<float>()), 3.14f);
	}
}

TEST(UniqueVariant, MoveAssignmentTest)
{
	// Same type assignment
	{
		UniqueVariant<int, TestVariantMoveTracker> variant1;
		variant1.Emplace<TestVariantMoveTracker>(100);

		UniqueVariant<int, TestVariantMoveTracker> variant2;
		variant2.Emplace<TestVariantMoveTracker>(200);

		variant2 = std::move(variant1);
		ASSERT_TRUE((variant1.Get<TestVariantMoveTracker>().moved));
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_EQ((variant2.Get<TestVariantMoveTracker>().value), 100);
	}

	// Different type assignment
	{
		UniqueVariant<int, TestVariantMoveTracker> variant1;
		variant1.Emplace<TestVariantMoveTracker>(100);

		UniqueVariant<int, TestVariantMoveTracker> variant2;
		variant2.Emplace<int>(42);

		variant2 = std::move(variant1);
		ASSERT_TRUE((variant1.Get<TestVariantMoveTracker>().moved));
		ASSERT_EQ(variant2.GetTypeIndex(), 1);
		ASSERT_EQ((variant2.Get<TestVariantMoveTracker>().value), 100);
	}
}

TEST(UniqueVariant, EmplaceTest)
{
	// Emplace with constructor arguments
	{
		UniqueVariant<int, TestVariantComplexTypeForEmplace, double> variant;
		auto& ref = variant.Emplace<TestVariantComplexTypeForEmplace>(42, 3.14f);

		ASSERT_EQ(variant.GetTypeIndex(), 1);
		ASSERT_EQ(ref.a, 42);
		ASSERT_FLOAT_EQ(ref.b, 3.14f);
		ASSERT_EQ((variant.Get<TestVariantComplexTypeForEmplace>().a), 42);
	}

	// Emplace changing type
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<int>(10);
		ASSERT_EQ(variant.GetTypeIndex(), 0);

		variant.Emplace<float>(3.14f);
		ASSERT_EQ(variant.GetTypeIndex(), 1);
		ASSERT_FLOAT_EQ((variant.Get<float>()), 3.14f);
	}
}

TEST(UniqueVariant, GetTest)
{
	// Non-const Get
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<double>(2.718);

		ASSERT_EQ(variant.GetTypeIndex(), 2);
		ASSERT_DOUBLE_EQ((variant.Get<double>()), 2.718);
	}

	// Const Get
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<float>(1.5f);

		const auto& const_variant = variant;
		ASSERT_FLOAT_EQ((const_variant.Get<float>()), 1.5f);
	}
}

TEST(UniqueVariant, QueryTest)
{
	// Non-const Query - success
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<float>(2.5f);

		auto* ptr = variant.Query<float>();
		ASSERT_NE(ptr, nullptr);
		ASSERT_FLOAT_EQ(*ptr, 2.5f);
	}

	// Non-const Query - failure
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<int>(42);

		auto* ptr = variant.Query<float>();
		ASSERT_EQ(ptr, nullptr);
	}

	// Const Query - success
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<double>(3.14159);

		const auto& const_variant = variant;
		auto* ptr = const_variant.Query<double>();
		ASSERT_NE(ptr, nullptr);
		ASSERT_DOUBLE_EQ(*ptr, 3.14159);
	}

	// Const Query - failure
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<int>(42);

		const auto& const_variant = variant;
		auto* ptr = const_variant.Query<double>();
		ASSERT_EQ(ptr, nullptr);
	}
}

TEST(UniqueVariant, VisitTest)
{
	// Non-const Visit (by value)
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<int>(10);
		ASSERT_EQ((variant1.Visit(TestUniqueVariantSimpleVisitor{})), 20);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<float>(5.0f);
		ASSERT_EQ((variant2.Visit(TestUniqueVariantSimpleVisitor{})), 15);

		UniqueVariant<int, float, double> variant3;
		variant3.Emplace<double>(7.0);
		ASSERT_EQ((variant3.Visit(TestUniqueVariantSimpleVisitor{})), 28);
	}

	// Const Visit
	{
		UniqueVariant<int, float, double> variant;
		variant.Emplace<float>(10.0f);

		const auto& const_variant = variant;
		ASSERT_EQ((const_variant.Visit(TestUniqueVariantConstVisitor{})), 12);
	}

	// Test Visitor passed as lvalue reference (&)
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<int>(5);

		TestUniqueVariantStateTracker visitor1;
		ASSERT_EQ((variant1.Visit(visitor1)), 10);
		ASSERT_EQ(visitor1.state, 1);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<float>(5.0f);

		TestUniqueVariantStateTracker visitor2;
		ASSERT_EQ((variant2.Visit(visitor2)), 15);
		ASSERT_EQ(visitor2.state, 1);

		UniqueVariant<int, float, double> variant3;
		variant3.Emplace<double>(5.0);

		TestUniqueVariantStateTracker visitor3;
		ASSERT_EQ((variant3.Visit(visitor3)), 20);
		ASSERT_EQ(visitor3.state, 1);
	}

	// Test Visitor passed as const lvalue reference (const &)
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<int>(5);

		const TestUniqueVariantStateTracker const_visitor1;
		ASSERT_EQ((variant1.Visit(const_visitor1)), 15);
		ASSERT_EQ(const_visitor1.state, 2);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<float>(5.0f);

		const TestUniqueVariantStateTracker const_visitor2;
		ASSERT_EQ((variant2.Visit(const_visitor2)), 25);
		ASSERT_EQ(const_visitor2.state, 2);

		UniqueVariant<int, float, double> variant3;
		variant3.Emplace<double>(5.0);

		const TestUniqueVariantStateTracker const_visitor3;
		ASSERT_EQ((variant3.Visit(const_visitor3)), 35);
		ASSERT_EQ(const_visitor3.state, 2);
	}

	// Test Visitor passed as rvalue reference (&&)
	{
		UniqueVariant<int, float, double> variant1;
		variant1.Emplace<int>(5);

		TestUniqueVariantStateTracker rvalue_visitor1;
		ASSERT_EQ((variant1.Visit(std::move(rvalue_visitor1))), 105);
		ASSERT_EQ(rvalue_visitor1.state, 3);

		UniqueVariant<int, float, double> variant2;
		variant2.Emplace<float>(5.0f);

		TestUniqueVariantStateTracker rvalue_visitor2;
		ASSERT_EQ((variant2.Visit(std::move(rvalue_visitor2))), 205);
		ASSERT_EQ(rvalue_visitor2.state, 3);

		UniqueVariant<int, float, double> variant3;
		variant3.Emplace<double>(5.0);

		TestUniqueVariantStateTracker rvalue_visitor3;
		ASSERT_EQ((variant3.Visit(std::move(rvalue_visitor3))), 305);
		ASSERT_EQ(rvalue_visitor3.state, 3);
	}
}

TEST(UniqueVariant, DestructorTest)
{
	// Destructor called on UniqueVariant destruction
	{
		TestVariantTrackedType::destructor_count = 0;
		{
			UniqueVariant<int, TestVariantTrackedType> variant;
			variant.Emplace<TestVariantTrackedType>();
		}
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 1);
	}

	// Destructor called on Emplace
	{
		TestVariantTrackedType::destructor_count = 0;
		UniqueVariant<int, TestVariantTrackedType> variant;
		variant.Emplace<TestVariantTrackedType>();
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 0);

		variant.Emplace<int>(42);
		ASSERT_EQ(TestVariantTrackedType::destructor_count, 1);
	}
}

TEST(UniqueVariant, ComplexTypeTest)
{
	UniqueVariant<int, TestVariantComplexType> variant1;
	variant1.Emplace<TestVariantComplexType>(100);
	ASSERT_EQ(*(variant1.Get<TestVariantComplexType>().data), 100);

	UniqueVariant<int, TestVariantComplexType> variant2(variant1);
	ASSERT_EQ(*(variant2.Get<TestVariantComplexType>().data), 100);
	ASSERT_EQ(*(variant1.Get<TestVariantComplexType>().data), 100);

	UniqueVariant<int, TestVariantComplexType> variant3;
	variant3 = variant2;
	ASSERT_EQ(*(variant3.Get<TestVariantComplexType>().data), 100);
}