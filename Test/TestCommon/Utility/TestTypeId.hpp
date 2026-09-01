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
#include "Utility/TypeId.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct TestTypeId
{
};

struct TestTypeId2
{
};

SGE_DECLARE_TYPE_ID(, TestTypeId);
SGE_DEFINE_TYPE_ID(, TestTypeId);
SGE_DECLARE_TYPE_ID(, TestTypeId2);
SGE_DEFINE_TYPE_ID(, TestTypeId2);

TEST(TypeId, Test)
{
	UInt64 id1 = GetTypeId<TestTypeId>();
	UInt64 id2 = GetTypeId<TestTypeId2>();
	ASSERT_NE(id1, 0);
	ASSERT_NE(id2, 0);
	ASSERT_NE(id1, id2);
}