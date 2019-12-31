/*
Copyright 2019 Chenxi Xu (@xsun2001)

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
#include "AllocatorCommon.h"

using namespace SpaceGameEngine::Detail;
namespace SpaceGameEngine::Detail::SegregatedFit
{
	template<SizeType size,
			 typename = std::enable_if_t<is_greater(size, 0)>>
	union MemoryBlock {
		MemoryBlock* next;
		Byte data[size];
	};

	template<SizeType count, SizeType size,
			 typename = std::enable_if_t<is_greater(count, 0)>,
			 typename = std::enable_if_t<is_greater(size, 0)>>
	struct MemoryPage
	{
		MemoryBlock<size> blocks[count];
	};

	struct SegregatedFitAllocator
	{
	private:
	public:
		ALLOCATE_FUNCTION_SIGNATURE
		{
		}

		DEALLOCATE_FUNCTION_SIGNATURE{

		};
	};
}