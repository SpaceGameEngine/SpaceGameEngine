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
#include "ForwardDefinition.hpp"
#include "Platform.hpp"
#include <string>
#ifndef SGE_MACOS
#include <cuchar>
#endif

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	//#if defined(SGE_WINDOWS) && defined(UNICODE)
//#define SGE_USE_WIDE_CHAR
//#endif
//
//#ifdef SGE_USE_WIDE_CHAR
//	using StdString = std::wstring;
//#define StdTCin std::wcin
//#define StdTCout std::wcout
//#define StdToString std::to_wstring
//	using Char = wchar_t;
//#define SGE_STR(str) L##str
//#else
//	using StdString = std::string;
//#define StdTCin std::cin
//#define StdTCout std::cout
//#define StdToString std::to_string
//	using Char = char;
//#define SGE_STR(str) str
//#endif
//
//#ifndef SGE_USE_WIDE_CHAR
///*!
//@file
//@todo check whether the other os's runtime character set is utf-8 or not.
//If not, find a way to make it be the utf-8.
//*/
//#if defined(SGE_WINDOWS)
//#pragma execution_character_set("utf-8")	//set the runtime character set to utf-8
//#endif
//#endif
#ifdef SGE_WINDOWS
	using Char16 = wchar_t;
#else
	using Char16 = char16_t;
#endif
	using Char = Char16;

#define SGE_USE_WIDE_CHAR
#define StdCout std::wcout
#define StdString std::wstring
#define StdToString std::to_wstring

#ifdef SGE_WINDOWS
#define SGE_STR_(str) L##str
#else
#define SGE_STR_(str) u##str
#endif

#define SGE_STR(str) SGE_STR_(str)
#ifndef SGE_WINDOWS
#define __FILEW__ SGE_STR(__FILE__)
#define __FUNCTIONW__ SGE_STR(__FUNCTION__)
#endif
	/*!
	@}
	*/
}