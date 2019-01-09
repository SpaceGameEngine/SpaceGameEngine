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
#include "MemoryManager.h"
#include <string>
#include <tchar.h>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef UNICODE
	using StdTString = std::wstring;
#define StdTCin std::wcin
#define StdTCout std::wcout
#else
	using StdTString = std::string;
#define StdTCin std::cin
#define StdTCout std::cout
#endif
	using StdTChar = TCHAR;
#define SGE_TSTR(str) _T(str)

	class StdTStringImplement;
	using StringImplement = StdTStringImplement;

	class String
	{
	public:
		String();
		String(const String& str);
		String(const StdTString& str);
		String(const StdTChar* pstr);
		~String();

		String& operator = (const String& str);
		String& operator = (const StdTString& str);
		String& operator = (const StdTChar* pstr);

		bool operator ==(const String& str)const;
		bool operator ==(const StdTString& str)const;
		bool operator ==(const StdTChar* pstr)const;

		StdTString ToStdTString()const;
		const StdTChar* GetConstData()const;
		StdTChar* GetData();
		SizeType GetSize();
	private:
		StringImplement* m_pImplement;
	};

	/*!
	@}
	*/
}