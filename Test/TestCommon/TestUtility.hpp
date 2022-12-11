/*
Copyright 2022 creatorlxd

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
#include <random>
#include <cmath>
#include "Utility/ControllableObject.hpp"
#include "Utility/Utility.hpp"
#include "Utility/AutoReleaseBuffer.h"
#include "Utility/Endian.h"
#include "Utility/DebugInformation.h"
#include "Utility/FixedSizeBuffer.hpp"
#include "Utility/Optional.hpp"
#include "SGEString.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

class test_cmp
{
	int i;

public:
	bool operator==(const test_cmp&) const = delete;
};
class test_cmp2
{
	int i;
};
bool operator==(const test_cmp2&, const test_cmp2&) = delete;

struct test_controllable_object_class
{
	test_controllable_object_class()
	{
		i = 1;
	}
	~test_controllable_object_class()
	{
		i = 0;
	}
	test_controllable_object_class(const test_controllable_object_class& obj)
	{
		i = 2;
	}
	test_controllable_object_class(test_controllable_object_class&& obj)
	{
		i = 3;
	}

	test_controllable_object_class& operator=(const test_controllable_object_class& obj)
	{
		i = 4;
		return *this;
	}
	test_controllable_object_class& operator=(test_controllable_object_class&& obj)
	{
		i = 5;
		return *this;
	}

	int i;
};

struct test_auto_release_buffer_class
{
	test_auto_release_buffer_class(const Function<void()> func)
		: m_Function(func)
	{
	}

	~test_auto_release_buffer_class()
	{
		m_Function();
	}

	Function<void()> m_Function;
};

TEST(ControllableObject, InstanceTest)
{
	ControllableObject<int> test;
	test.Init(1);
	ASSERT_TRUE(test.IsInitialized());
	ASSERT_EQ(test.Get(), 1);
	test.Release();
	ASSERT_FALSE(test.IsInitialized());
}

TEST(ControllableObject, CopyTest)
{
	ControllableObject<test_controllable_object_class> test;
	test.Init();
	ASSERT_EQ(test.Get().i, 1);
	ControllableObject<test_controllable_object_class> test2(test);
	ASSERT_EQ(test2.Get().i, 2);
	ControllableObject<test_controllable_object_class> test3(std::move(test2));
	ASSERT_EQ(test3.Get().i, 3);
	ControllableObject<test_controllable_object_class> test4;
	test4.Init();
	test4 = test;
	ASSERT_EQ(test4.Get().i, 4);
	ControllableObject<test_controllable_object_class> test5;
	test5.Init();
	test5 = std::move(test);
	ASSERT_EQ(test5.Get().i, 5);
	test_controllable_object_class _test, _test2;
	ControllableObject<test_controllable_object_class> test6(_test);
	ASSERT_EQ(test6.Get().i, 2);
	ControllableObject<test_controllable_object_class> test7(std::move(_test));
	ASSERT_EQ(test7.Get().i, 3);
	ControllableObject<test_controllable_object_class> test8;
	test8.Init();
	test8 = _test2;
	ASSERT_EQ(test8.Get().i, 4);
	test8 = std::move(_test2);
	ASSERT_EQ(test8.Get().i, 5);

	ControllableObject<int, StdAllocator> test9(9);
	ControllableObject<int, MemoryManagerAllocator> test10(10);
	test10 = test9;
	ASSERT_EQ(test9, test10);
	ASSERT_EQ(test10.Get(), 9);
}

TEST(ControllableObject, EqualityTest)
{
	ControllableObject<int> test(1), test2(2), test3(1);
	ASSERT_FALSE(test == test2);
	ASSERT_TRUE(test == test3);
	ASSERT_FALSE(test == 2);
	ASSERT_TRUE(test2 == 2);
	ControllableObject<int> test4, test5;
	ASSERT_FALSE(test4 == test);
	ASSERT_TRUE(test4 == test5);
	ASSERT_FALSE(test4 == 0);
}

TEST(AutoReleaseBuffer, InstanceAndNewObjectTest)
{
	AutoReleaseBuffer* arbuf = DefaultAllocator::New<AutoReleaseBuffer>();
	int* pi = arbuf->NewObject<int>();
	*pi = 10;
	ASSERT_EQ(*pi, 10);
	int flag = 0;
	test_auto_release_buffer_class* pc = arbuf->NewObject<test_auto_release_buffer_class>(
		[&]() {
			flag = 1;
		});
	ASSERT_EQ(flag, 0);
	DefaultAllocator::Delete(arbuf);
	ASSERT_EQ(flag, 1);
}

TEST(Less, LessTest)
{
	ASSERT_TRUE(Less<int>::Compare(1, 2));
	ASSERT_FALSE(Less<int>::Compare(1, 1));
	ASSERT_FALSE(Less<int>::Compare(2, 1));
}

TEST(Greater, GreaterTest)
{
	ASSERT_TRUE(Greater<int>::Compare(2, 1));
	ASSERT_FALSE(Greater<int>::Compare(1, 1));
	ASSERT_FALSE(Greater<int>::Compare(1, 2));
}

TEST(Digits10, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<10>(num), (UInt64)(std::log10(num)) + 1);
	}
}

TEST(Digits2, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<2>(num), (UInt64)(std::log2(num)) + 1);
	}
}

TEST(Digits16, CorrectnessTest)
{
	std::random_device rd;
	for (int i = 0; i <= 10000; ++i)
	{
		UInt64 num = (rd() % UINT64_MAX) + 1;
		ASSERT_EQ(Digits<16>(num), (UInt64)(std::log2(num) / 4.0) + 1);
	}
}

TEST(Endian, ChangeEndianTest)
{
	UInt32 test_val = 0x12345678;
	ChangeEndian(test_val, Endian::Big, Endian::Little);
	ASSERT_EQ(test_val, 0x78563412);
}

DebugInformation TestDebugInformation()
{
	return DebugInformation(SGE_DEBUG_INFORMATION);
}

TEST(DebugInformation, DebugInformationTest)
{
	DebugInformation di = TestDebugInformation();
	TString<> filename(di.m_pFileName);
	TString<> funcname(di.m_pFunctionName);
	ASSERT_NE(filename.Find(SGE_TSTR("TestUtility.hpp"), filename.GetConstBegin(), filename.GetConstEnd()), filename.GetConstEnd());
	ASSERT_NE(funcname.Find(SGE_TSTR("TestDebugInformation"), funcname.GetConstBegin(), funcname.GetConstEnd()), funcname.GetConstEnd());
	ASSERT_EQ(di.m_LineNumber, 220);
}

TEST(FixedSizeBuffer, InstanceTest)
{
	FixedSizeBuffer<32> buffer;
}

TEST(FixedSizeBuffer, AppendTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, SetSizeAndClearTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
	buffer.SetSize(2);
	ASSERT_EQ(buffer.GetSize(), 2);
	ASSERT_EQ(buffer.GetFreeSize(), 30);
	buffer.Clear();
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
}

TEST(FixedSizeBuffer, CopyConstructionTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, MoveConstructionTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, CopyOperatorTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, MoveOperatorTest)
{
	FixedSizeBuffer<32> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorCopyConstructionTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorMoveConstructionTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2(std::move(buffer));
	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorCopyOperatorTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = buffer;

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);

	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);
}

TEST(FixedSizeBuffer, AnotherAllocatorMoveOperatorTest)
{
	FixedSizeBuffer<32, MemoryManagerAllocator> buffer;
	ASSERT_EQ(buffer.GetSize(), 0);
	ASSERT_EQ(buffer.GetFreeSize(), 32);
	UInt32 test_data = 123456;
	buffer.Append(&test_data, sizeof(test_data));
	ASSERT_EQ(buffer.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer.GetData(), test_data);

	FixedSizeBuffer<32, StdAllocator> buffer2;
	ASSERT_EQ(buffer2.GetSize(), 0);
	ASSERT_EQ(buffer2.GetFreeSize(), 32);

	buffer2 = std::move(buffer);

	ASSERT_EQ(buffer2.GetSize(), sizeof(test_data));
	ASSERT_EQ(buffer2.GetFreeSize(), 32 - sizeof(test_data));
	ASSERT_EQ(*(UInt32*)buffer2.GetData(), test_data);
}

TEST(Optional, InstanceTest)
{
	Optional<test_controllable_object_class> empty_opt;
	ASSERT_FALSE(empty_opt.HasValue());

	Optional<test_controllable_object_class> opt1{test_controllable_object_class()};
	ASSERT_TRUE(opt1.HasValue());
	ASSERT_EQ(opt1.Get().i, 3);

	bool has_destruct = false;
	{
		const Optional<test_auto_release_buffer_class> opt2([&]() {
			has_destruct = true;
		});
		ASSERT_TRUE(opt2.HasValue());
		ASSERT_FALSE(has_destruct);
	}
	ASSERT_TRUE(has_destruct);

	Optional<test_controllable_object_class> opt3(OptionalTag::EmptyOptional);
	ASSERT_FALSE(opt3.HasValue());

	Optional<test_controllable_object_class> opt4(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 1);
}

TEST(Optional, CopyConstructionTest)
{
	Optional<test_controllable_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_controllable_object_class> opt3(opt1);
	ASSERT_FALSE(opt3.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt4(opt2);
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 2);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);
}

TEST(Optional, MoveConstructionTest)
{
	Optional<test_controllable_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_controllable_object_class> opt3(std::move(opt1));
	ASSERT_FALSE(opt3.HasValue());
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt4(std::move(opt2));
	ASSERT_TRUE(opt4.HasValue());
	ASSERT_EQ(opt4.Get().i, 3);
	ASSERT_FALSE(opt2.HasValue());
}

TEST(Optional, CopyAssignmentTest)
{
	Optional<test_controllable_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_controllable_object_class> opt3;
	ASSERT_FALSE(opt3.HasValue());

	Optional<test_controllable_object_class> opt4;
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
	Optional<test_controllable_object_class> opt1;
	ASSERT_FALSE(opt1.HasValue());

	Optional<test_controllable_object_class> opt2(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt2.HasValue());
	ASSERT_EQ(opt2.Get().i, 1);

	Optional<test_controllable_object_class> opt3(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt3.HasValue());
	ASSERT_EQ(opt3.Get().i, 1);

	Optional<test_controllable_object_class> opt4;
	ASSERT_FALSE(opt4.HasValue());

	Optional<test_controllable_object_class> opt5;
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
	Optional<test_controllable_object_class> opt;
	ASSERT_FALSE(opt.HasValue());

	opt.Set();

	ASSERT_TRUE(opt.HasValue());
	ASSERT_EQ(opt.Get().i, 1);
}

TEST(Optional, ClearTest)
{
	Optional<test_controllable_object_class> opt(OptionalTag::DefaultValue);
	ASSERT_TRUE(opt.HasValue());
	ASSERT_EQ(opt.Get().i, 1);

	opt.Clear();

	ASSERT_FALSE(opt.HasValue());

	opt.Set(test_controllable_object_class());

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

	ASSERT_FALSE(opt1 == opt2);
	ASSERT_FALSE(opt2 == opt1);
	ASSERT_FALSE(opt2 == opt3);
	ASSERT_FALSE(opt3 == opt2);

	ASSERT_TRUE(opt1 != opt2);
	ASSERT_TRUE(opt2 != opt1);
	ASSERT_TRUE(opt2 != opt3);
	ASSERT_TRUE(opt3 != opt2);

	ASSERT_FALSE(opt1 != opt1);
	ASSERT_FALSE(opt2 != opt2);
}