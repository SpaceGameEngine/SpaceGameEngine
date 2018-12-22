/*
Copyright 2018 creatorlxd

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
#include "String.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	enum class ErrorLevel
	{
		/*!
		@brief outputs the message.
		*/
		Information,
		/*!
		@brief outputs the error message,means that the error may cause a fatal,the program will enter debug mode in VS when debug.
		*/
		Warning,
		/*!
		@brief outputs the error message and the program will enter debug mode in VS when debug then will crash.
		*/
		Fatal
	};

	class Error
	{
	public:
		Error();
		virtual ~Error();
		void ShowError()const;
	protected:
		String m_ErrorMessage;
	};

	void ThrowError(const Error& error, ErrorLevel error_level);

	/*!
	@brief assert condition when debug
	*/
#ifdef _DEBUG
#define SGE_ASSERT(expr,error_type,error_level)\
	if(!(expr)) ThrowError(error_type(),error_level);
#else
#define SGE_ASSERT(expr,error_type,error_level)
#endif

	/*!
	@brief check condition
	*/
#define SGE_CHECK(expr,error_type,error_level)\
	if(!(expr)) ThrowError(error_type(),error_level);

	class NullPointerError :public Error
	{
	public:
		NullPointerError();
	};

	void AssertNullPointer(const void* ptr);
	void CheckNullPointer(const void* ptr);

	class InvalidSizeError :public Error
	{
	public:
		InvalidSizeError();
	};

	void AssertInvalidSize(SizeType size, SizeType min_size, SizeType max_size);
	void CheckInvalidSize(SizeType size, SizeType min_size, SizeType max_size);

	/*!
	@}
	*/
}