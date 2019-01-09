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
#include "../../Public/ForwardDefination.hpp"
#include "../../Public/String.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!@todo use my allocator*/
	class StdTStringImplement
	{
	public:
		StdTStringImplement();
		StdTStringImplement(const StdTStringImplement& str);
		StdTStringImplement(const StdTString& str);
		StdTStringImplement(const StdTChar* pstr);

		StdTStringImplement& operator = (const StdTStringImplement& str);
		StdTStringImplement& operator = (const StdTString& str);
		StdTStringImplement& operator = (const StdTChar* pstr);

		bool operator ==(const StdTStringImplement& str)const;
		bool operator ==(const StdTString& str)const;
		bool operator ==(const StdTChar* pstr)const;

		StdTString ToStdTString()const;
		const StdTChar* GetConstData()const;
		StdTChar* GetData();
		SizeType GetSize();
	private:
		StdTString m_Content;
	};

	/*!
	@}
	*/
}