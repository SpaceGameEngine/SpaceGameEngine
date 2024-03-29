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

#ifdef SGE_WINDOWS
	using Char16 = wchar_t;
#else
	using Char16 = char16_t;
#endif
	using Char = Char16;

#ifdef SGE_CPP20
	using Char8 = char8_t;
#else
	using Char8 = char;
#endif

#if defined(SGE_WINDOWS) && defined(UNICODE)
#define SGE_USE_WIDE_CHAR
#endif

#ifdef SGE_USE_WIDE_CHAR
	using StdTString = std::wstring;
#define StdTCin std::wcin
#define StdTCout std::wcout
#define StdToTString(x) std::to_wstring((x))
	using TChar = Char16;
#define SGE_TSTR_(str) L##str
#else
#ifdef SGE_CPP20
	using StdTString = std::u8string;
#define StdToTString(x) std::u8string((const char8_t*)std::to_string((x)).c_str())
#else
	using StdTString = std::string;
#define StdToTString(x) std::to_string((x))
#endif
#define StdTCin std::cin
#define StdTCout std::cout
	using TChar = Char8;
#define SGE_TSTR_(str) u8##str
#endif

#define SGE_TSTR(str) SGE_TSTR_(str)

#ifdef SGE_WINDOWS
#define SGE_WSTR_(str) L##str
#else
#define SGE_WSTR_(str) u##str
#endif

#define SGE_WSTR(str) SGE_WSTR_(str)

#define SGE_U8STR_(str) u8##str

#define SGE_U8STR(str) SGE_U8STR_(str)

#define SGE_STR(str) SGE_WSTR(str)

	/*!
	@file
	@todo use project manager to define utf8 compile option to set runtime character environment as utf8.
	*/

	/*!
	@}
	*/
}