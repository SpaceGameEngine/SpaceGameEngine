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

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	class Error;

	enum class ErrorLevel;

	void ThrowError(const Error& error,ErrorLevel error_level);

	/*!
	@brief assert condition when debug
	*/
#ifdef _DEBUG
#define SGE_ASSERT(expr,error,error_level)\
	if(!(expr)) ThrowError(error,error_level);
#else
#define SGE_ASSERT(expr,error,error_level)
#endif

	/*!
	@brief check condition
	*/
#define SGE_CHECK(expr,error,error_level)\
	if(!(expr)) ThrowError(error,error_level);

	/*!
	@}
	*/
}
