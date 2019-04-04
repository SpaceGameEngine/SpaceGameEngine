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

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!@todo using template and supported customed allocator and char type*/
	class StdTStringImplement
	{
	public:
		StdTStringImplement();
		StdTStringImplement(const StdTStringImplement& str);
		StdTStringImplement(const StdTString& str);
		StdTStringImplement(const TChar* pstr);
		StdTStringImplement(StdTStringImplement&& str);

		StdTStringImplement& operator = (const StdTStringImplement& str);
		StdTStringImplement& operator = (const StdTString& str);
		StdTStringImplement& operator = (const TChar* pstr);
		StdTStringImplement& operator = (StdTStringImplement&& str);

		bool operator ==(const StdTStringImplement& str)const;
		bool operator ==(const StdTString& str)const;
		bool operator ==(const TChar* pstr)const;

		StdTString ToStdTString()const;
		const TChar* ToCString()const;
		TChar* GetData();
		SizeType GetSize();
	private:
		StdTString m_Content;
	};

	/*!
	@}
	*/
}