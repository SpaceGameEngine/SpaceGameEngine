/*
Copyright 2025 creatorlxd

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
#include "gtest/gtest.h"
#include "Stream/Stream.h"
#include "Container/Queue.hpp"

using namespace SpaceGameEngine;

class TestStream : public InputStream, public OutputStream
{
public:
	inline virtual ~TestStream() = default;

	inline virtual bool Read(MemoryData& data) override
	{
		if (m_Content.GetSize())
		{
			data = std::move(m_Content.GetFront());
			m_Content.Pop();
			return true;
		}
		else
			return false;
	}

	inline virtual bool Write(const MemoryData& data) override
	{
		m_Content.Push(MakeMemoryData(data.GetData(), data.GetSize()));
		return true;
	}

private:
	Queue<MemoryData> m_Content;
};