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
#include "Error.h"
#include <iostream>
#include <Windows.h>

SpaceGameEngine::Error::Error()
{
	m_ErrorMessage = SGE_TSTR("Empty Error");
}

SpaceGameEngine::Error::~Error()
{
}

void SpaceGameEngine::Error::ShowError()const
{
	/*!@todo use sge's output*/
	StdTCout << m_ErrorMessage.ToStdTString() << std::endl;
}

void SpaceGameEngine::ThrowError(const Error & error, ErrorLevel error_level)
{
	switch (error_level)
	{
	case ErrorLevel::Information:
		error.ShowError();
		break;
	case ErrorLevel::Warning:
		error.ShowError();
#ifdef _DEBUG
		DebugBreak();
#endif
		break;
	case ErrorLevel::Fatal:
		error.ShowError();
#ifdef _DEBUG
		DebugBreak();
#endif
		abort();
		break;
	default:
		error.ShowError();
		break;
	}
}

void SpaceGameEngine::AssertNullPointer(const void * ptr)
{
	SGE_ASSERT(ptr, NullPointerError, ErrorLevel::Fatal);
}

void SpaceGameEngine::CheckNullPointer(const void * ptr)
{
	SGE_CHECK(ptr, NullPointerError, ErrorLevel::Fatal);
}

void SpaceGameEngine::AssertInvalidSize(SizeType size, SizeType min_size, SizeType max_size)
{
	SGE_ASSERT(size >= min_size && size <= max_size, InvalidSizeError, ErrorLevel::Fatal);
}

void SpaceGameEngine::CheckInvalidSize(SizeType size, SizeType min_size, SizeType max_size)
{
	SGE_CHECK(size >= min_size && size <= max_size, InvalidSizeError, ErrorLevel::Fatal);
}

SpaceGameEngine::NullPointerError::NullPointerError()
{
	m_ErrorMessage = SGE_TSTR("Pointer can not be null");
}

SpaceGameEngine::InvalidSizeError::InvalidSizeError()
{
	m_ErrorMessage = SGE_TSTR("The size is invalid");
}
