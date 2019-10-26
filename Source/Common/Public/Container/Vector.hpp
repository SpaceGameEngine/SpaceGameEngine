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
#include "TypeDefinition.hpp"
#include "MemoryManager.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Allocator = DefaultAllocator>
	class Vector
	{
	public:
		inline Vector();

		inline ~Vector();

	private:
		void* m_pContent;
		SizeType m_RealSize;
		SizeType m_Size;
	};

	template<typename T, typename Allocator>
	inline Vector<T, Allocator>::Vector()
	{
		m_RealSize = 4;
		m_Size = 0;
		m_pContent = Allocator::template RawNew(m_RealSize * sizeof(T), alignof(T));
	}

	template<typename T, typename Allocator>
	inline Vector<T, Allocator>::~Vector()
	{
		if (m_pContent)
			Allocator::template RawDelete(m_pContent, m_RealSize * sizeof(T), alignof(T));
	}

	/*!
	@}
	*/
}