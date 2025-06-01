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
#include "Stream/Stream.h"
#include "Container/Vector.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<IsAllocator Allocator = DefaultAllocator>
	class CumulateStream : public InputStream, OutputStream
	{
	public:
		inline virtual bool Read(MemoryData& data) override
		{
			if (m_Content.GetSize() == 0)
				return false;

			data = MakeMemoryData<Allocator>(m_Content.GetData(), m_Content.GetSize());
			m_Content.Clear();
			return true;
		}

		inline virtual bool Write(const MemoryData& data) override
		{
			if (data.GetSize() == 0)
				return false;

			const Byte* ptr = (const Byte*)data.GetData();
			m_Content.Insert(m_Content.GetConstEnd(), ptr, ptr + data.GetSize());
			return true;
		}

	private:
		Vector<Byte, Allocator> m_Content;
	};
}

/*!
@}
*/