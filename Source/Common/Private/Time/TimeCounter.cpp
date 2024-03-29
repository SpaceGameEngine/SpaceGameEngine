﻿/*
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
#include "Time/TimeCounter.h"
#include <thread>
#include <chrono>

using namespace SpaceGameEngine;

#ifdef SGE_WINDOWS
#include "WindowsTimeImplement.h"
#elif defined(SGE_UNIX)
#include "UnixTimeImplement.h"
#endif

TimeType SpaceGameEngine::GetNowTime()
{
	return TimeImplement::GetNowTime();
}

void SpaceGameEngine::SleepFor(TimeType microsecond)
{
	std::this_thread::sleep_for(std::chrono::microseconds(microsecond));
}