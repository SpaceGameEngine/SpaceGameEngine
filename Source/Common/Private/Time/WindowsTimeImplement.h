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
#include "Time/TimeCounter.h"
#include "Platform.hpp"
#include "Error.h"

#ifdef SGE_WINDOWS
#include <Windows.h>
#endif

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	namespace TimeImplement
	{
#ifdef SGE_WINDOWS
		namespace WindowsTimeImplement
		{
			struct GetQPCFrequencyFailedError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("Can not get the qpc frequency");
				static bool Judge(BOOL re_code);
			};

			TimeType GetQPCFrequency();

			struct GetQPCCounterFailedError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("Can not get the qpc counter");
				static bool Judge(BOOL re_code);
			};

			TimeType GetQPCCounter();
		}
#endif
	}

	/*!
	@}
	*/
}