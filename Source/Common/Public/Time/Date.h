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
#pragma once
#include "SGEString.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	/*!
	@todo add calendar functions
	*/
	struct COMMON_API Date
	{
		UInt8 m_Second = 0;
		UInt8 m_Minute = 0;
		UInt8 m_Hour = 0;
		UInt8 m_Day = 0;
		UInt8 m_Month = 0;
		UInt32 m_Year = 0;
	};

	COMMON_API Date GetLocalDate();

	struct InvalidDateError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("Date is invalid.");
		static COMMON_API bool Judge(const Date& date);
	};

}

/*!
@}
*/