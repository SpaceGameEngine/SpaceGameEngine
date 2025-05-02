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
#include "Utility/AutoReleaseBuffer.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

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