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
#pragma once
#include "Time/TimeCounter.h"
#include "Error.h"
#include "CommonAPI.h"
#ifdef SGE_UNIX
#include <sys/time.h>
#endif

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	namespace TimeImplement
	{
#ifdef SGE_UNIX
		namespace UnixTimeImplement
		{
			struct COMMON_API GetTimeOfDayFailedError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("Can not get the time of day");
				static bool Judge(int re_code);
			};
		}

		/*!
		@brief return the now time represented by microsecond.
		*/
		COMMON_API TimeType GetNowTime();
#endif
	}

	/*!
	@}
	*/
}