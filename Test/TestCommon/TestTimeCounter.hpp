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

#ifdef SGE_WINDOWS
#include "Time/../../Private/Time/WindowsTimeImplement.h"

TEST(WindowsTimeImplement, GetQPCFrequencyTest)
{
	TimeType fre1 = SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCFrequency();
	ASSERT_GT(fre1, 0);
	TimeType fre2 = SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCFrequency();
	ASSERT_GT(fre2, 0);

	ASSERT_EQ(fre1, fre2);
}

TEST(WindowsTimeImplement, GetQPCCounterTest)
{
	TimeType t1 = SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCCounter();
	ASSERT_GT(t1, 0);
	TimeType t2 = SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCCounter();
	ASSERT_GT(t2, 0);

	ASSERT_GE(t2, t1);
}
#endif