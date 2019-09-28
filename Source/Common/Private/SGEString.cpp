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
#include "SGEString.h"
#include "StringImplement/StdTStringImplement.h"
#include "Error.h"
#include "MemoryManager.h"

SpaceGameEngine::String::String()
{
	m_pImplement = DefaultAllocator::New<StringImplement>();
}

SpaceGameEngine::String::String(const String& str)
{
	m_pImplement = DefaultAllocator::New<StringImplement>(*str.m_pImplement);
}

SpaceGameEngine::String::String(const StdTString& str)
{
	m_pImplement = DefaultAllocator::New<StringImplement>(str);
}

SpaceGameEngine::String::String(const TChar* pstr)
{
	SGE_ASSERT(NullPointerError, pstr);
	m_pImplement = DefaultAllocator::New<StringImplement>(pstr);
}

SpaceGameEngine::String::String(String&& str)
{
	m_pImplement = DefaultAllocator::New<StringImplement>(std::move(*str.m_pImplement));
}

SpaceGameEngine::String& SpaceGameEngine::String::operator=(const String& str)
{
	*m_pImplement = *str.m_pImplement;
	return *this;
}

SpaceGameEngine::String& SpaceGameEngine::String::operator=(const StdTString& str)
{
	*m_pImplement = str;
	return *this;
}

SpaceGameEngine::String& SpaceGameEngine::String::operator=(const TChar* pstr)
{
	SGE_ASSERT(NullPointerError, pstr);
	*m_pImplement = pstr;
	return *this;
}

SpaceGameEngine::String& SpaceGameEngine::String::operator=(String&& str)
{
	*m_pImplement = std::move(*str.m_pImplement);
	return *this;
}

bool SpaceGameEngine::String::operator==(const String& str) const
{
	return *m_pImplement == *str.m_pImplement;
}

bool SpaceGameEngine::String::operator==(const StdTString& str) const
{
	return *m_pImplement == str;
}

bool SpaceGameEngine::String::operator==(const TChar* pstr) const
{
	SGE_ASSERT(NullPointerError, pstr);
	return *m_pImplement == pstr;
}

SpaceGameEngine::String::~String()
{
	DefaultAllocator::Delete(m_pImplement);
}

SpaceGameEngine::StdTString SpaceGameEngine::String::ToStdTString() const
{
	return m_pImplement->ToStdTString();
}

const SpaceGameEngine::TChar* SpaceGameEngine::String::ToCString() const
{
	return m_pImplement->ToCString();
}

SpaceGameEngine::TChar* SpaceGameEngine::String::GetData()
{
	return m_pImplement->GetData();
}

SpaceGameEngine::SizeType SpaceGameEngine::String::GetSize()
{
	return m_pImplement->GetSize();
}
