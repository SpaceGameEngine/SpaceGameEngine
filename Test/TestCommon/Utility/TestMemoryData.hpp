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
#include "Utility/MemoryData.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(MemoryData, DefaultConstructionTest)
{
	MemoryData md;
	ASSERT_EQ(md.GetData(), nullptr);
	ASSERT_EQ(md.GetSize(), 0);
}

TEST(MemoryData, ConstructionAndDestructionTest)
{
	int test_data = 42;

	static bool freed = false;
	freed = false;

	auto free_func = [](void* ptr, SizeType size) {
		freed = true;
	};

	MemoryData* pmd = new MemoryData(&test_data, sizeof(test_data), free_func);
	ASSERT_EQ(pmd->GetSize(), sizeof(test_data));
	ASSERT_EQ(pmd->GetData(), &test_data);
	ASSERT_EQ(*(int*)pmd->GetData(), 42);
	ASSERT_FALSE(freed);
	delete pmd;
	ASSERT_TRUE(freed);
}

TEST(MemoryData, MoveConstructionTest)
{
	int* memory = DefaultAllocator::New<int>(42);

	static bool freed = false;
	freed = false;

	auto free_func = [](void* ptr, SizeType size) {
		freed = true;
		DefaultAllocator::Delete((int*)ptr);
	};

	MemoryData* pmd1 = new MemoryData(memory, sizeof(int), free_func);
	ASSERT_EQ(*(int*)(pmd1->GetData()), 42);
	ASSERT_EQ(pmd1->GetSize(), sizeof(int));

	MemoryData* pmd2 = new MemoryData(std::move(*pmd1));

	ASSERT_EQ(*(int*)(pmd2->GetData()), 42);
	ASSERT_EQ(pmd2->GetSize(), sizeof(int));

	ASSERT_EQ(pmd1->GetData(), nullptr);
	ASSERT_EQ(pmd1->GetSize(), 0);

	delete pmd1;
	ASSERT_FALSE(freed);

	delete pmd2;
	ASSERT_TRUE(freed);
}

TEST(MemoryData, MoveAssignmentTest)
{
	int* memory1 = DefaultAllocator::New<int>(42);
	int* memory2 = DefaultAllocator::New<int>(84);

	static bool freed1 = false;
	static bool freed2 = false;
	freed1 = false;
	freed2 = false;

	auto free_func1 = [](void* ptr, SizeType size) {
		freed1 = true;
		DefaultAllocator::Delete(static_cast<int*>(ptr));
	};

	auto free_func2 = [](void* ptr, SizeType size) {
		freed2 = true;
		DefaultAllocator::Delete(static_cast<int*>(ptr));
	};

	MemoryData* pmd1 = new MemoryData(memory1, sizeof(int), free_func1);
	MemoryData* pmd2 = new MemoryData(memory2, sizeof(int), free_func2);

	ASSERT_EQ(*static_cast<const int*>(pmd1->GetData()), 42);
	ASSERT_EQ(*static_cast<const int*>(pmd2->GetData()), 84);

	*pmd1 = std::move(*pmd2);

	ASSERT_EQ(*static_cast<const int*>(pmd1->GetData()), 84);
	ASSERT_EQ(pmd1->GetSize(), sizeof(int));

	ASSERT_EQ(pmd2->GetData(), nullptr);
	ASSERT_EQ(pmd2->GetSize(), 0);

	ASSERT_TRUE(freed1);
	ASSERT_FALSE(freed2);

	delete pmd1;
	delete pmd2;
	ASSERT_TRUE(freed1);
	ASSERT_TRUE(freed2);
}

TEST(MemoryData, ClearTest)
{
	int* memory = DefaultAllocator::New<int>(42);

	static bool freed = false;
	freed = false;

	auto free_func = [](void* ptr, SizeType size) {
		freed = true;
		DefaultAllocator::Delete(static_cast<int*>(ptr));
	};

	MemoryData md(memory, sizeof(int), free_func);
	ASSERT_EQ(*static_cast<const int*>(md.GetData()), 42);
	ASSERT_EQ(md.GetSize(), sizeof(int));
	ASSERT_FALSE(freed);

	md.Clear();

	ASSERT_EQ(md.GetData(), nullptr);
	ASSERT_EQ(md.GetSize(), 0);
	ASSERT_TRUE(freed);

	freed = false;
	md.Clear();
	ASSERT_EQ(md.GetData(), nullptr);
	ASSERT_EQ(md.GetSize(), 0);
	ASSERT_FALSE(freed);
}

TEST(MemoryData, Test)
{
	const char* test_str = "TestMemoryData";
	size_t str_len = strlen(test_str) + 1;

	auto md = MakeMemoryData<DefaultAllocator>(test_str, str_len);

	ASSERT_NE(md.GetData(), test_str);
	ASSERT_EQ(md.GetSize(), str_len);
	ASSERT_STREQ(static_cast<const char*>(md.GetData()), test_str);
}