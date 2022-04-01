/*
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
#include "Utility/AutoReleaseBuffer.h"

SpaceGameEngine::AutoReleaseBuffer::AutoReleaseBuffer()
{
}

SpaceGameEngine::AutoReleaseBuffer::~AutoReleaseBuffer()
{
	for (auto i = m_Functions.GetBegin(); i != m_Functions.GetEnd(); i += 1)
		(*i)();
}

SpaceGameEngine::AutoReleaseBuffer::AutoReleaseBuffer(AutoReleaseBuffer&& a)
	: m_Functions(std::move(a.m_Functions))
{
}

SpaceGameEngine::AutoReleaseBuffer& SpaceGameEngine::AutoReleaseBuffer::operator=(AutoReleaseBuffer&& a)
{
	m_Functions = std::move(a.m_Functions);
	return *this;
}
