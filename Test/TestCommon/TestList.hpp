/*
Copyright 2021 creatorlxd

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
#include "Container/List.hpp"

using namespace SpaceGameEngine;

TEST(List, InstanceTest)
{
	List<int> l;
	ASSERT_TRUE((std::is_same_v<decltype(l)::ValueType, int>));
	ASSERT_TRUE((std::is_same_v<decltype(l)::AllocatorType, DefaultAllocator>));
	ASSERT_EQ(l.GetSize(), 0);
}