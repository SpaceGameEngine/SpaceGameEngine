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
	inline TestLogWriterCore()
		: m_Output(nullptr)
	{
	}

	inline TestLogWriterCore(TestStringType& output)
		: m_Output(&output)
	{
	}

	inline void WriteLog(const Char8* pstr, SizeType size)
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		if (m_Output)
			m_Output->Insert(m_Output->GetEnd(), pstr, pstr + size);
	}

	inline void SetOutput(TestStringType& output)
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		m_Output = &output;
	}

private:
	Mutex m_Mutex;
	TestStringType* m_Output;
};

TEST(LogWriter, WriteLogTest)
{
	TestStringType test_output;
	{
		//test LogWriterCore construction
		LogWriter<TestLogWriterCore> lw(test_output);
		//test LogWriterCore method
		lw.SetOutput(test_output);
		Mutex mutex;
		Condition end_cond;
		int cnt = 256;
		for (auto i = 0; i < 256; ++i)
		{
			Thread t([&, i]() {
				Char8 c_buf = i;
				TestStringType str_buf(1024, c_buf);
				for (auto j = 0; j < 64; ++j)
				{
					lw.WriteLog(str_buf.GetData(), str_buf.GetNormalSize());
					//SleepFor(MakeTimeDuration<Microsecond, TimeType>(100));
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
	ASSERT_EQ(test_output.GetNormalSize(), 256 * 1024 * 64);
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
		ASSERT_EQ(test_cnt[i], 1024 * 64);
}

class TestLogWriterCore2
{
public:
	void WriteLog(const Char8* pstr, SizeType size)
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		m_String.Insert(m_String.GetConstEnd(), pstr, pstr + size);
	}

	UTF8String GetString()
	{
		RecursiveLock locker(m_Mutex);
		locker.Lock();
		return m_String;
	}

private:
	Mutex m_Mutex;
	UTF8String m_String;
};

TEST(Logger, InstanceTest)
{
	LogWriter<TestLogWriterCore2> lw;
	Logger<TestLogWriterCore2> l(lw, LogLevel::Warning);
}

TEST(Logger, WriteLogTest)
{
	LogWriter<TestLogWriterCore2> lw;
	Logger<TestLogWriterCore2> l(lw, LogLevel::Warning);
	Date test_date;
	test_date.m_Year = 1;
	test_date.m_Month = 2;
	test_date.m_Day = 3;
	test_date.m_Hour = 4;
	test_date.m_Minute = 5;
	test_date.m_Second = 6;
	DebugInformation test_di(SGE_TSTR("test_file"), SGE_TSTR("test_func"), 78);
	l.WriteLog(test_date, test_di, LogLevel::Information, SGE_U8STR("test non-formatted information"));
	l.WriteLog(test_date, test_di, LogLevel::Warning, SGE_U8STR("test non-formatted warning"));
	l.WriteLog(test_date, test_di, LogLevel::Debug, SGE_U8STR("test {}+{}={} debug"), 1, 2, 3);
	l.WriteLog(test_date, test_di, LogLevel::Exception, SGE_U8STR("test {}+{}={} exception"), 1, 2, 3);
	SleepFor(MakeTimeDuration<Millisecond>(4250));
	UTF8String result = lw.GetString();
	auto lines = Split(result, UTF8String(SGE_U8STR("\n")));
	ASSERT_EQ(lines.GetSize(), 3);
	ASSERT_EQ(lines[0], SGE_U8STR("0001-02-03 04:05:06 test_file:test_func:78 WARNING test non-formatted warning"));
	ASSERT_EQ(lines[1], SGE_U8STR("0001-02-03 04:05:06 test_file:test_func:78 EXCEPTION test 1+2=3 exception"));
	ASSERT_EQ(lines[2].GetSize(), 0);
}