/*
Copyright 2025 creatorlxd

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
#include "Meta/Trait.hpp"

using namespace SpaceGameEngine;

TEST(RemoveCVRef, Test)
{
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int&&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<int&&>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<const int>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<volatile int>, int>));
	ASSERT_TRUE((std::is_same_v<RemoveCVRefType<volatile int&>, int>));
}

TEST(TypeWrapper, Test)
{
	ASSERT_TRUE((std::is_same_v<TypeWrapper<const int&>::Type, const int&>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int&>::Type, int&>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int>::Type, int>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int*>::Type, int*>));
	ASSERT_TRUE((std::is_same_v<TypeWrapper<int&&>::Type, int&&>));
}

TEST(ValueWrapper, Test)
{
	ASSERT_EQ(ValueWrapper<42>::Value, 42);
	ASSERT_EQ(ValueWrapper<'a'>::Value, 'a');
	ASSERT_EQ(ValueWrapper<3.14>::Value, 3.14);
}

template<typename T, typename U>
struct TestBindFirstWithTypeStruct1
{
	using FirstType = T;
	using SecondType = U;
};

template<typename T, typename U, typename V>
struct TestBindFirstWithTypeStruct2
{
	using FirstType = T;
	using SecondType = U;
	using ThirdType = V;
};

template<SizeType A, SizeType B>
struct TestBindFirstWithValueStruct1
{
	inline static constexpr const SizeType Result = A + B;
};

template<SizeType A, SizeType B, SizeType C>
struct TestBindFirstWithValueStruct2
{
	inline static constexpr const SizeType Result = A + B + C;
};

TEST(BindFirstWithType, Test)
{
	using BoundFunction1 = BindFirstWithType<TestBindFirstWithTypeStruct1, const int>;
	ASSERT_TRUE((std::is_same_v<BoundFunction1::Type<double>::FirstType, const int>));
	ASSERT_TRUE((std::is_same_v<BoundFunction1::Type<double>::SecondType, double>));

	using BoundFunction2 = BindFirstWithType<TestBindFirstWithTypeStruct2, const int, double>;
	ASSERT_TRUE((std::is_same_v<BoundFunction2::Type<float>::FirstType, const int>));
	ASSERT_TRUE((std::is_same_v<BoundFunction2::Type<float>::SecondType, double>));
	ASSERT_TRUE((std::is_same_v<BoundFunction2::Type<float>::ThirdType, float>));
}

TEST(BindFirstWithValue, Test)
{
	using BoundFunction1 = BindFirstWithValue<TestBindFirstWithValueStruct1, 10>;
	ASSERT_EQ(BoundFunction1::Type<32>::Result, 42);

	using BoundFunction2 = BindFirstWithValue<TestBindFirstWithValueStruct2, 10, 20>;
	ASSERT_EQ(BoundFunction2::Type<12>::Result, 42);
}

TEST(Compose, Test)
{
	using ComposedType1 = Compose<TestBindFirstWithTypeStruct1, TypeWrapper>::Type<int, float>;
	ASSERT_TRUE((std::is_same_v<ComposedType1::Type::FirstType, int>));
	ASSERT_TRUE((std::is_same_v<ComposedType1::Type::SecondType, float>));
}