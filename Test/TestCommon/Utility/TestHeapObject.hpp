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
#include "Utility/HeapObject.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct TestHeapObjectTestObject
{
	int value;
	static int construct_count;
	static int destruct_count;
	static int copy_construct_count;
	static int copy_assignment_count;
	static int move_construct_count;
	static int move_assignment_count;

	TestHeapObjectTestObject()
		: value(0)
	{
		++construct_count;
	}

	explicit TestHeapObjectTestObject(int v)
		: value(v)
	{
		++construct_count;
	}

	TestHeapObjectTestObject(const TestHeapObjectTestObject& other)
		: value(other.value)
	{
		++copy_construct_count;
	}

	TestHeapObjectTestObject(TestHeapObjectTestObject&& other) noexcept
		: value(other.value)
	{
		++move_construct_count;
		other.value = 0;
	}

	TestHeapObjectTestObject& operator=(const TestHeapObjectTestObject& other)
	{
		if (this != &other)
		{
			value = other.value;
			++copy_assignment_count;
		}
		return *this;
	}

	TestHeapObjectTestObject& operator=(TestHeapObjectTestObject&& other) noexcept
	{
		if (this != &other)
		{
			value = other.value;
			other.value = 0;
			++move_assignment_count;
		}
		return *this;
	}

	~TestHeapObjectTestObject()
	{
		++destruct_count;
	}

	static void ResetCounters()
	{
		construct_count = 0;
		destruct_count = 0;
		copy_construct_count = 0;
		copy_assignment_count = 0;
		move_construct_count = 0;
		move_assignment_count = 0;
	}
};

int TestHeapObjectTestObject::construct_count = 0;
int TestHeapObjectTestObject::destruct_count = 0;
int TestHeapObjectTestObject::copy_construct_count = 0;
int TestHeapObjectTestObject::copy_assignment_count = 0;
int TestHeapObjectTestObject::move_construct_count = 0;
int TestHeapObjectTestObject::move_assignment_count = 0;

TEST(HeapObject, DefaultConstructionTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj;
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);
		ASSERT_EQ(obj.Get().value, 0);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 1);
}

TEST(HeapObject, ConstructionWithValueTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj(42);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);
		ASSERT_EQ(obj.Get().value, 42);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 1);
}

TEST(HeapObject, CopyConstructionTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj1(100);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		HeapObject<TestHeapObjectTestObject> obj2(obj1);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj1.Get().value, 100);
		ASSERT_EQ(obj2.Get().value, 100);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, MoveConstructionTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj1(200);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		HeapObject<TestHeapObjectTestObject> obj2(std::move(obj1));
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj2.Get().value, 200);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, CopyAssignmentTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj1(150);
		HeapObject<TestHeapObjectTestObject> obj2(50);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		obj2 = obj1;
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj1.Get().value, 150);
		ASSERT_EQ(obj2.Get().value, 150);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, MoveAssignmentTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject> obj1(250);
		HeapObject<TestHeapObjectTestObject> obj2(75);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		obj2 = std::move(obj1);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj2.Get().value, 250);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, AnotherAllocatorCopyConstructionTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject, StdAllocator> obj1(100);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		HeapObject<TestHeapObjectTestObject, MemoryManagerAllocator> obj2(obj1);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj1.Get().value, 100);
		ASSERT_EQ(obj2.Get().value, 100);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, AnotherAllocatorMoveConstructionTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject, StdAllocator> obj1(200);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		HeapObject<TestHeapObjectTestObject, MemoryManagerAllocator> obj2(std::move(obj1));
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj2.Get().value, 200);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, AnotherAllocatorCopyAssignmentTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject, StdAllocator> obj1(150);
		HeapObject<TestHeapObjectTestObject, MemoryManagerAllocator> obj2(50);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		obj2 = obj1;
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj1.Get().value, 150);
		ASSERT_EQ(obj2.Get().value, 150);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, AnotherAllocatorMoveAssignmentTest)
{
	TestHeapObjectTestObject::ResetCounters();
	{
		HeapObject<TestHeapObjectTestObject, StdAllocator> obj1(250);
		HeapObject<TestHeapObjectTestObject, MemoryManagerAllocator> obj2(75);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		obj2 = std::move(obj1);
		ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
		ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
		ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 1);
		ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 0);

		ASSERT_EQ(obj2.Get().value, 250);
	}
	ASSERT_EQ(TestHeapObjectTestObject::construct_count, 2);
	ASSERT_EQ(TestHeapObjectTestObject::copy_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::copy_assignment_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_construct_count, 0);
	ASSERT_EQ(TestHeapObjectTestObject::move_assignment_count, 1);
	ASSERT_EQ(TestHeapObjectTestObject::destruct_count, 2);
}

TEST(HeapObject, GetMethodTest)
{
	HeapObject<TestHeapObjectTestObject> obj(300);

	ASSERT_EQ(obj.Get().value, 300);

	obj.Get().value = 400;
	ASSERT_EQ(obj.Get().value, 400);
}

TEST(HeapObject, ConstGetMethodTest)
{
	const HeapObject<TestHeapObjectTestObject> obj(500);

	ASSERT_EQ(obj.Get().value, 500);
}

TEST(HeapObject, ConversionOperatorTest)
{
	HeapObject<TestHeapObjectTestObject> obj(600);
	TestHeapObjectTestObject& ref = obj;
	ASSERT_EQ(obj.Get().value, 600);
	ref.value = 650;
	ASSERT_EQ(obj.Get().value, 650);

	const TestHeapObjectTestObject& cref = obj;
	ASSERT_EQ(cref.value, 650);
}