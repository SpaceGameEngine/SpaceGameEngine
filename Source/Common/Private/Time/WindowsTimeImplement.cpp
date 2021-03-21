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
#include "WindowsTimeImplement.h"

#ifdef SGE_WINDOWS

using namespace SpaceGameEngine;

bool SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCFrequencyFailedError::Judge(BOOL re_code)
{
	return re_code == 0;
}

TimeType SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCFrequency()
{
	static bool is_first_time = true;
	static TimeType re = 0;
	if (is_first_time)
	{
		LARGE_INTEGER Frequency;
		SGE_CHECK(GetQPCFrequencyFailedError, QueryPerformanceFrequency(&Frequency));
		re = Frequency.QuadPart;
		is_first_time = false;
	}
	return re;
}

bool SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCCounterFailedError::Judge(BOOL re_code)
{
	return re_code == 0;
}

TimeType SpaceGameEngine::TimeImplement::WindowsTimeImplement::GetQPCCounter()
{
	static LARGE_INTEGER li_buf;
	SGE_CHECK(GetQPCCounterFailedError, QueryPerformanceCounter(&li_buf));
	return li_buf.QuadPart;
}
#endif