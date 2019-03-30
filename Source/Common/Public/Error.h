/*
Copyright 2019 creatorlxd

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
#include "ForwardDefination.hpp"
#include "SGEString.h"
#include "Platform.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#if defined(DEBUG) || defined(_DEBUG)
#define SGE_DEBUG
#endif

	struct DebugInformation
	{
		const TChar* m_pFileName;
		const TChar* m_pFunctionName;
		int m_LineNumber;

		DebugInformation(const TChar* file_name, const TChar* func_name, int line_number);
	};

#ifdef SGE_USE_WIDE_CHAR
#define SGE_FILE __FILEW__
#define SGE_FUNCTION __FUNCTIONW__
#else
#define SGE_FILE __FILE__
#define SGE_FUNCTION __FUNCTION__
#endif
#define SGE_LINE __LINE__
#define SGE_DEBUG_INFORMATION SpaceGameEngine::DebugInformation(SGE_FILE,SGE_FUNCTION,SGE_LINE)

	void ThrowError(const TChar* error_msg, DebugInformation debug_info);

	/*!
	@brief a example of error class
	*/
	struct BlankError
	{
		/*!
		@brief error message
		*/
		inline static const TChar sm_pContent[] = SGE_TSTR("Blank Error");
		/*!
		@brief a function for judging the condition
		@retval true error happend
		@retval false no error happend
		*/
		static bool Judge();
	};

	/*!
	@brief assert condition when debug
	*/
#ifdef SGE_DEBUG
#define SGE_ASSERT(error_type,...)\
	if(error_type::Judge(__VA_ARGS__)) SpaceGameEngine::ThrowError(error_type::sm_pContent,SGE_DEBUG_INFORMATION);
#else
#define SGE_ASSERT(error_type,...)
#endif

	/*!
	@brief check condition
	*/
#define SGE_CHECK(error_type,...)\
	if(error_type::Judge(__VA_ARGS__)) SpaceGameEngine::ThrowError(error_type::sm_pContent,SGE_DEBUG_INFORMATION);

	struct NullPointerError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Pointer can not be null");
		static bool Judge(const void* ptr);
	};

	struct InvalidSizeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The size is invalid");
		static bool Judge(SizeType size, SizeType min_size, SizeType max_size);
	};

	/*!
	@}
	*/
}