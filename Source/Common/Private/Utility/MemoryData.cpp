/*
Copyright 2026 creatorlxd

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
#include "Utility/MemoryData.h"
#include "Error.h"

using namespace SpaceGameEngine;

SpaceGameEngine::MemoryData::~MemoryData()
{
	RawClear();
}

SpaceGameEngine::MemoryData::MemoryData(void* ptr, SizeType size, FreeFunction free_func)
	: m_pContent(ptr), m_Size(size), m_pFreeFunction(free_func)
{
}

SpaceGameEngine::MemoryData::MemoryData(MemoryData&& md)
	: m_pContent(md.m_pContent), m_Size(md.m_Size), m_pFreeFunction(md.m_pFreeFunction)
{
	md.m_pContent = nullptr;
	md.m_Size = 0;
	md.m_pFreeFunction = nullptr;
}

MemoryData& SpaceGameEngine::MemoryData::operator=(MemoryData&& md)
{
	SGE_ASSERT(SelfAssignmentError, this, &md);

	RawClear();

	m_pContent = md.m_pContent;
	m_Size = md.m_Size;
	m_pFreeFunction = md.m_pFreeFunction;

	md.m_pContent = nullptr;
	md.m_Size = 0;
	md.m_pFreeFunction = nullptr;

	return *this;
}

void SpaceGameEngine::MemoryData::Clear()
{
	RawClear();
	m_pContent = nullptr;
	m_Size = 0;
	m_pFreeFunction = nullptr;
}

void* SpaceGameEngine::MemoryData::GetData()
{
	return m_pContent;
}

const void* SpaceGameEngine::MemoryData::GetData() const
{
	return m_pContent;
}

SizeType SpaceGameEngine::MemoryData::GetSize() const
{
	return m_Size;
}

void SpaceGameEngine::MemoryData::RawClear()
{
	if (m_pFreeFunction)
		m_pFreeFunction(m_pContent, m_Size);
}
