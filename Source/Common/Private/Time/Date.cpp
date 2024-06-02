/*
Copyright 2024 creatorlxd

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
#include "Time/Date.h"
#include <ctime>

using namespace SpaceGameEngine;

Date SpaceGameEngine::GetLocalDate()
{
	time_t now_time;
	time(&now_time);
	struct tm date_time;
#ifdef SGE_WINDOWS
	localtime_s(&date_time, &now_time);
#elif defined(SGE_POSIX)
	localtime_r(&now_time, &date_time);
#else
#error this os has not been supported.
#endif
	Date re;
	re.m_Second = date_time.tm_sec;
	re.m_Minute = date_time.tm_min;
	re.m_Hour = date_time.tm_hour;
	re.m_Day = date_time.tm_mday;
	re.m_Month = date_time.tm_mon + 1;
	re.m_Year = date_time.tm_year + 1900;
	return re;
}

bool SpaceGameEngine::InvalidDateError::Judge(const Date& date)
{
	return !(
		(date.m_Second < 60) &&
		(date.m_Minute < 60) &&
		(date.m_Hour < 24) &&
		(date.m_Day > 0 && date.m_Day < 32) &&
		(date.m_Month > 0 && date.m_Month < 13) &&
		(date.m_Year > 1899));
}
