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
#include "Log.h"
#include "SGEString.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(LogWriter, InstanceTest)
{
	LogWriter lw;
}

using TestStringType = StringCore<Char8>;

class TestLogWriterCore
{
public:
	void WriteLog(const Char8* pstr, SizeType size)
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		if (m_Output)
			m_Output->Insert(m_Output->GetEnd(), pstr, pstr + size);
	}

	void SetOutput(TestStringType& output)
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		m_Output = &output;
	}

private:
	Mutex m_Mutex;
	TestStringType* m_Output = nullptr;
};

TEST(LogWriter, WriteLogTest)
{
	TestStringType test_output;
	{
		LogWriter<TestLogWriterCore> lw;
		lw.GetLogWriterCore().SetOutput(test_output);
		Mutex mutex;
		Condition end_cond;
		int cnt = 256;
		for (auto i = 0; i < 256; ++i)
		{
			Thread t([&, i]() {
				Char8 c_buf = i;
				TestStringType str_buf(1024, c_buf);
				for (auto j = 0; j < 1024; ++j)
				{
					lw.WriteLog(str_buf.GetData(), str_buf.GetNormalSize());
					SleepFor(TimeDuration<Microsecond, TimeType>(1));
				}
				RecursiveLock locker(mutex);
				locker.Lock();
				cnt -= 1;
				if (cnt == 0)
					end_cond.NodifyAll();
			});
			t.Detach();
		}
		RecursiveLock locker(mutex);
		locker.Lock();
		end_cond.Wait(locker, [&]() { return cnt == 0; });
	}
	ASSERT_EQ(test_output.GetNormalSize(), 256 * 1024 * 1024);
	int test_cnt[256];
	memset(test_cnt, 0, sizeof(test_cnt));
	for (auto i = test_output.GetConstBegin(); i != test_output.GetConstEnd();)
	{
		Char8 c = *i;
		for (auto j = 0; j < 1024; ++j, ++i)
			ASSERT_EQ(*i, c);
		test_cnt[(SizeType)c] += 1024;
	}
	for (auto i = 0; i < 256; ++i)
		ASSERT_EQ(test_cnt[i], 1024 * 1024);
}