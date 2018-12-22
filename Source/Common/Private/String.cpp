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
#include "../Public/String.h"
#include "../Private/StringImplement/StdTStringImplement.h"
#include "../Public/Error.h"

SpaceGameEngine::String::String()
{
	m_pImplement = DefaultAllocator::New<StringImplement>();
}

SpaceGameEngine::String::String(const StdTString & str)
{
	m_pImplement = DefaultAllocator::New<StringImplement>(str);
}

SpaceGameEngine::String::String(const StdTChar * pstr)
{
	AssertNullPointer(pstr);
	m_pImplement = DefaultAllocator::New<StringImplement>(pstr);
}

SpaceGameEngine::String & SpaceGameEngine::String::operator=(const StdTString & str)
{
	*m_pImplement = str;
	return *this;
}

SpaceGameEngine::String & SpaceGameEngine::String::operator=(const StdTChar * pstr)
{
	AssertNullPointer(pstr);
	*m_pImplement = pstr;
	return *this;
}

bool SpaceGameEngine::String::operator==(const StdTString & str)const
{
	return *m_pImplement == str;
}

bool SpaceGameEngine::String::operator==(const StdTChar * pstr)const
{
	AssertNullPointer(pstr);
	return *m_pImplement == pstr;
}

SpaceGameEngine::String::~String()
{
	DefaultAllocator::Delete(m_pImplement);
}

SpaceGameEngine::StdTString SpaceGameEngine::String::ToStdTString()const
{
	return m_pImplement->ToStdTString();
}

const SpaceGameEngine::StdTChar * SpaceGameEngine::String::GetConstData() const
{
	return m_pImplement->GetConstData();
}

SpaceGameEngine::StdTChar * SpaceGameEngine::String::GetData()
{
	return m_pImplement->GetData();
}

SpaceGameEngine::SizeType SpaceGameEngine::String::GetSize()
{
	return m_pImplement->GetSize();
}
