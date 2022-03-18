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
#include "CommonAPI.h"
#include "SGEStringForward.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	struct COMMON_API DebugInformation
	{
		const TChar* m_pFileName;
		const TChar* m_pFunctionName;
		UInt32 m_LineNumber;

		DebugInformation(const TChar* file_name, const TChar* func_name, UInt32 line_number);
	};

#ifdef SGE_USE_WIDE_CHAR
#define SGE_FILE __FILEW__
#define SGE_FUNCTION __FUNCTIONW__
#else
#define SGE_FILE __FILE__
#define SGE_FUNCTION __FUNCTION__
#endif
#define SGE_LINE __LINE__
#define SGE_DEBUG_INFORMATION SpaceGameEngine::DebugInformation((const TChar*)SGE_FILE, (const TChar*)SGE_FUNCTION, SGE_LINE)

	/*!
	@}
	*/
}