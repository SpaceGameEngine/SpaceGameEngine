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
#include "gtest/gtest.h"
#include "Meta/TypeList.hpp"

using namespace SpaceGameEngine;

TEST(TypeList, InstanceTest)
{
	TypeList<int, float, double> type_list;
	using TypeListType1 = TypeList<>;
	using TypeListType2 = TypeList<int>;
	using TypeListType3 = TypeList<int, float>;
}

template<typename T1, typename T2, typename T3>
struct TestTypeListApplyType
{
	using FirstType = T1;
	using SecondType = T2;
	using ThirdType = T3;
};

TEST(TypeList, ApplyTest)
{
	using TypeListType = TypeList<int, float, double>;
	using AppliedType = TypeListType::Apply<TestTypeListApplyType>;
	ASSERT_TRUE((std::is_same_v<AppliedType::FirstType, int>));
	ASSERT_TRUE((std::is_same_v<AppliedType::SecondType, float>));
	ASSERT_TRUE((std::is_same_v<AppliedType::ThirdType, double>));
}

TEST(TypeList, GetTest)
{
	using TypeListType = TypeList<int, float, double>;
	ASSERT_TRUE((std::is_same_v<TypeListType::Get<0>, int>));
	ASSERT_TRUE((std::is_same_v<TypeListType::Get<1>, float>));
	ASSERT_TRUE((std::is_same_v<TypeListType::Get<2>, double>));
}

template<typename T>
struct TestTypeListFirstIndexType
{
	inline static constexpr const bool Value = std::is_same_v<T, float>;
};

TEST(TypeList, FirstIndexTest)
{
	using TypeListType = TypeList<int, float, double, float>;
	constexpr SizeType first_index = TypeListType::FirstIndex<TestTypeListFirstIndexType>;
	ASSERT_EQ(first_index, 1);
	using TypeListType2 = TypeList<int, double, char>;
	constexpr SizeType first_index2 = TypeListType2::FirstIndex<TestTypeListFirstIndexType>;
	ASSERT_EQ(first_index2, TypeListType2::Size);
}

template<typename T>
struct TestTypeListMapType
{
	using Type = typename std::conditional_t<std::is_integral_v<T>, double, T>;
};

TEST(TypeList, MapTest)
{
	using TypeListType = TypeList<int, float, char>;
	using MappedTypeList = TypeListType::Map<TestTypeListMapType>;
	ASSERT_TRUE((std::is_same_v<MappedTypeList::Get<0>, double>));
	ASSERT_TRUE((std::is_same_v<MappedTypeList::Get<1>, float>));
	ASSERT_TRUE((std::is_same_v<MappedTypeList::Get<2>, double>));
}

TEST(TypeList, IsTypeListTest)
{
	ASSERT_TRUE(IsTypeList<TypeList<>>);
	ASSERT_TRUE((IsTypeList<TypeList<int, float>>));
	ASSERT_TRUE((IsTypeList<TypeList<int, float, double>>));
	ASSERT_FALSE(IsTypeList<void>);
	ASSERT_FALSE(IsTypeList<int>);
}
