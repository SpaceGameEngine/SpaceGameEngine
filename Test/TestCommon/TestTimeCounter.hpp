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
#include "gtest/gtest.h"

#ifdef SGE_WINDOWS
#include "Time/../../Private/Time/WindowsTimeImplement.h"
#include <Windows.h>

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

TEST(TimeImplement, GetNowTimeTest)
{
	TimeType t1 = SpaceGameEngine::TimeImplement::GetNowTime();
	SleepFor(1000);
	TimeType t2 = SpaceGameEngine::TimeImplement::GetNowTime();

	ASSERT_GT(t2, t1);
	ASSERT_GE(t2 - t1, 900);

	TimeType t3 = SpaceGameEngine::TimeImplement::GetNowTime();
	SleepFor(10000);
	TimeType t4 = SpaceGameEngine::TimeImplement::GetNowTime();

	ASSERT_GT(t4, t3);
	ASSERT_GE(t4 - t3, 9000);
}
#endif

#ifdef SGE_UNIX
#include "Time/../../Private/Time/UnixTimeImplement.h"

TEST(TimeImplement, GetNowTimeTest)
{
	TimeType t1 = SpaceGameEngine::TimeImplement::GetNowTime();
	SleepFor(1000);
	TimeType t2 = SpaceGameEngine::TimeImplement::GetNowTime();

	ASSERT_GT(t2, t1);
	ASSERT_GE(t2 - t1, 900);

	TimeType t3 = SpaceGameEngine::TimeImplement::GetNowTime();
	SleepFor(10000);
	TimeType t4 = SpaceGameEngine::TimeImplement::GetNowTime();

	ASSERT_GT(t4, t3);
	ASSERT_GE(t4 - t3, 9000);
}
#endif

TEST(TimeCounter, GetNowTimeTest)
{
	TimeType t1 = SpaceGameEngine::GetNowTime();
	SleepFor(1000000);
	TimeType t2 = SpaceGameEngine::GetNowTime();

	ASSERT_GT(t2, t1);
	ASSERT_GE(t2 - t1, 950000);
	ASSERT_LT(t2 - t1, 1500000);
}

TEST(TimeStamp, InstanceAndConvertTest)
{
	TimeStamp<Second> t1;
	ASSERT_EQ(t1.GetValue(), 0);

	TimeStamp<Second, double> t2(2500000);
	ASSERT_EQ(t2.GetValue(), 2.5);

	ASSERT_EQ(t2.operator TimeType(), 2500000);
}

TEST(TimeStamp, GetNowTimeStampTest)
{
	auto t1 = GetNowTimeStamp<Second, double>();

	SleepFor(1500000);

	auto t2 = GetNowTimeStamp<Second, double>();

	ASSERT_GT(t2.GetValue(), t1.GetValue());
	ASSERT_GE(t2.GetValue() - t1.GetValue(), 1.4);
}

TEST(TimeStamp, MinusTest)
{
	auto t1 = GetNowTimeStamp<Second, double>();

	SleepFor(1500000);

	auto t2 = GetNowTimeStamp<Second, double>();

	auto td1 = t2 - t1;
	ASSERT_TRUE((std::is_same_v<decltype(td1), TimeDuration<Second, double>>));
	ASSERT_GE(td1.GetValue(), 1.4);
	ASSERT_LT(td1.GetValue(), 100);
}

TEST(TimeDuration, InstanceTest)
{
	TimeDuration<Second, double> t1;
	ASSERT_EQ(t1.GetValue(), 0.0);

	TimeDuration<Second, double> t2(1000);
	ASSERT_EQ(t2.GetValue(), 0.001);
}

TEST(TimeDuration, MinusTest)
{
	TimeDuration<Second, TimeType> t1;
	ASSERT_EQ(t1.GetValue(), 0);

	TimeDuration<Second, TimeType> t2(1000000);
	ASSERT_EQ(t2.GetValue(), 1);

	auto td1 = t2 - t1;

	ASSERT_TRUE((std::is_same_v<decltype(td1), TimeDuration<Second, TimeType>>));
	ASSERT_EQ(td1.GetValue(), 1);
}

TEST(TimeDuration, SelfMinusTest)
{
	TimeDuration<Second, TimeType> t1;
	ASSERT_EQ(t1.GetValue(), 0);

	TimeDuration<Second, TimeType> t2(1000000);
	ASSERT_EQ(t2.GetValue(), 1);

	t2 -= t1;

	ASSERT_EQ(t2.GetValue(), 1);
}

TEST(TimeDuration, AddTest)
{
	TimeDuration<Second, TimeType> t1;
	ASSERT_EQ(t1.GetValue(), 0);

	TimeDuration<Second, TimeType> t2(1000000);
	ASSERT_EQ(t2.GetValue(), 1);

	auto td1 = t2 + t1;

	ASSERT_TRUE((std::is_same_v<decltype(td1), TimeDuration<Second, TimeType>>));
	ASSERT_EQ(td1.GetValue(), 1);
}

TEST(TimeDuration, SelfAddTest)
{
	TimeDuration<Second, TimeType> t1;
	ASSERT_EQ(t1.GetValue(), 0);

	TimeDuration<Second, TimeType> t2(1000000);
	ASSERT_EQ(t2.GetValue(), 1);

	t1 += t2;

	ASSERT_EQ(t1.GetValue(), 1);
}

TEST(TimeDuration, SleepForTest)
{
	auto td1 = MakeTimeDuration<Second, double>(1.5);
	TimeStamp<Microsecond, TimeType> t1 = GetNowTimeStamp<Microsecond, TimeType>();
	SleepFor(td1);
	TimeStamp<Microsecond, TimeType> t2 = GetNowTimeStamp<Microsecond, TimeType>();

	ASSERT_GT(t2.GetValue(), t1.GetValue());
	ASSERT_GE(t2.GetValue() - t1.GetValue(), 1490000);
	ASSERT_LT(t2.GetValue() - t1.GetValue(), 100000000);
}

TEST(TimeDuration, MakeTimeDurationTest)
{
	auto t1 = MakeTimeDuration<Second, TimeType>(1);
	ASSERT_EQ(t1.GetTime(), 1000000);
	ASSERT_EQ(t1.GetValue(), 1);

	auto t2 = MakeTimeDuration<Millisecond, double>(0.5);
	ASSERT_EQ(t2.GetTime(), 500);
	ASSERT_EQ(t2.GetValue(), 0.5);
}

TEST(TimeCounter, AllTest)
{
	TimeCounter<Second, float> tc;
	tc.Start();
	for (int i = 0; i < 10; i++)
	{
		SleepFor(MakeTimeDuration<Second, float>(0.5f));
		tc.Tick();
		auto dt = tc.GetDeltaTime();
		ASSERT_GE(dt, 0.45f);
		ASSERT_LT(dt, 1.0f);
	}
}