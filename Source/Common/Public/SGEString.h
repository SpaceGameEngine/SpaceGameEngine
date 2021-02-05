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

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#if defined(SGE_WINDOWS) && defined(UNICODE)
#define SGE_USE_WIDE_CHAR
#endif

#ifdef SGE_USE_WIDE_CHAR
	using StdTString = std::wstring;
#define StdTCin std::wcin
#define StdTCout std::wcout
#define StdToString std::to_wstring
	using TChar = wchar_t;
#define SGE_TSTR(str) L##str
#else
	using StdTString = std::string;
#define StdTCin std::cin
#define StdTCout std::cout
#define StdToString std::to_string
	using TChar = char;
#define SGE_TSTR(str) str
#endif

#ifndef SGE_USE_WIDE_CHAR
/*!
@file
@todo check whether the other os's runtime character set is utf-8 or not.
If not, find a way to make it be the utf-8.
*/
#if defined(SGE_WINDOWS)
#pragma execution_character_set("utf-8")	//set the runtime character set to utf-8
#endif
#endif

	class StdTStringImplement;
	using StringImplement = StdTStringImplement;

	/*!
	@todo using template and supported customed allocator and char type
	*/
	class String
	{
	public:
		String();
		String(const String& str);
		String(const StdTString& str);
		String(const TChar* pstr);
		String(String&& str);
		~String();

		String& operator=(const String& str);
		String& operator=(const StdTString& str);
		String& operator=(const TChar* pstr);
		String& operator=(String&& str);

		bool operator==(const String& str) const;
		bool operator==(const StdTString& str) const;
		bool operator==(const TChar* pstr) const;

		StdTString ToStdTString() const;
		const TChar* ToCString() const;
		TChar* GetData();
		SizeType GetSize();

	private:
		StringImplement* m_pImplement;
	};

	/*!
	@}
	*/
}