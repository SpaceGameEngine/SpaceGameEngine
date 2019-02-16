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
#include "StdTStringImplement.h"
#include "Error.h"

SpaceGameEngine::StdTStringImplement::StdTStringImplement()
{
}

SpaceGameEngine::StdTStringImplement::StdTStringImplement(const StdTStringImplement & str)
{
	m_Content = str.m_Content;
}

SpaceGameEngine::StdTStringImplement::StdTStringImplement(const StdTString & str)
{
	m_Content = str;
}

SpaceGameEngine::StdTStringImplement::StdTStringImplement(const StdTChar * pstr)
{
	AssertNullPointer(pstr);
	m_Content = pstr;
}

SpaceGameEngine::StdTStringImplement::StdTStringImplement(StdTStringImplement && str)
{
	m_Content = std::move(str.m_Content);
}

SpaceGameEngine::StdTStringImplement & SpaceGameEngine::StdTStringImplement::operator=(const StdTStringImplement & str)
{
	m_Content = str.m_Content;
	return *this;
}

SpaceGameEngine::StdTStringImplement & SpaceGameEngine::StdTStringImplement::operator=(const StdTString & str)
{
	m_Content = str;
	return *this;
}

SpaceGameEngine::StdTStringImplement & SpaceGameEngine::StdTStringImplement::operator=(const StdTChar * pstr)
{
	AssertNullPointer(pstr);
	m_Content = pstr;
	return *this;
}

SpaceGameEngine::StdTStringImplement & SpaceGameEngine::StdTStringImplement::operator=(StdTStringImplement && str)
{
	m_Content = std::move(str.m_Content);
	return *this;
}

bool SpaceGameEngine::StdTStringImplement::operator==(const StdTStringImplement & str) const
{
	return m_Content == str.m_Content;
}

bool SpaceGameEngine::StdTStringImplement::operator==(const StdTString & str)const
{
	return m_Content == str;
}

bool SpaceGameEngine::StdTStringImplement::operator==(const StdTChar * pstr)const
{
	AssertNullPointer(pstr);
	return m_Content == pstr;
}

SpaceGameEngine::StdTString SpaceGameEngine::StdTStringImplement::ToStdTString()const
{
	return m_Content;
}

const SpaceGameEngine::StdTChar * SpaceGameEngine::StdTStringImplement::GetCString() const
{
	return m_Content.c_str();
}

SpaceGameEngine::StdTChar * SpaceGameEngine::StdTStringImplement::GetData()
{
	return m_Content.data();
}

SpaceGameEngine::SizeType SpaceGameEngine::StdTStringImplement::GetSize()
{
	return m_Content.size();
}
