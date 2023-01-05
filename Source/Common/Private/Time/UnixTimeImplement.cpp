/*
Copyright 2023 creatorlxd

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
#include "UnixTimeImplement.h"

#ifdef SGE_UNIX

using namespace SpaceGameEngine;

bool SpaceGameEngine::TimeImplement::UnixTimeImplement::GetTimeOfDayFailedError::Judge(int re_code)
{
	return re_code != 0;
}

TimeType SpaceGameEngine::TimeImplement::GetNowTime()
{
	timeval t;
	SGE_CHECK(UnixTimeImplement::GetTimeOfDayFailedError, gettimeofday(&t, NULL));
	return t.tv_sec * 1000000 + t.tv_usec;
}

#endif