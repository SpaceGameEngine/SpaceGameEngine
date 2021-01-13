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
#include "Memory/SegregatedFitAllocator.h"
#include "Error.h"
#include <algorithm>
#include <cstring>

bool SpaceGameEngine::InvalidAlignmentError::Judge(SizeType alignment)
{
	return (alignment & (alignment - 1)) != 0;
}

SpaceGameEngine::SizeType SpaceGameEngine::GetDefaultAlignment(SizeType size)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	return size >= 16 ? 16 : 4;
}

SpaceGameEngine::SegregatedFitAllocator::~SegregatedFitAllocator()
{
	for (auto& m_FixedSizeAllocator : m_FixedSizeAllocators)
	{
		delete m_FixedSizeAllocator;
	}
}

void* SpaceGameEngine::SegregatedFitAllocator::Allocate(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	if (size > sm_MaxMemoryBlockSize)
	{
		return _mm_malloc(size, alignment);
	}
	else
	{
		SGE_ASSERT(InvalidRequestInformationError, RequestInformation(size, alignment));
		UInt32 index = RequestInformationToIndex(RequestInformation(size, alignment));
		if (!m_FixedSizeAllocators[index])
			m_FixedSizeAllocators[index] = new LockedFixedSizeAllocator(size, sm_MemoryPageSize, alignment);
		return m_FixedSizeAllocators[index]->Allocate();
	}
}

void SpaceGameEngine::SegregatedFitAllocator::Free(void* ptr, SizeType size, SizeType alignment)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	if (size > sm_MaxMemoryBlockSize)
	{
		_mm_free(ptr);
	}
	else
	{
		SGE_ASSERT(InvalidRequestInformationError, RequestInformation(size, alignment));
		UInt32 index = RequestInformationToIndex(RequestInformation(size, alignment));
		SGE_ASSERT(NullPointerError, m_FixedSizeAllocators[index]);
		m_FixedSizeAllocators[index]->Free(ptr);
	}
}

SpaceGameEngine::SegregatedFitAllocator::SegregatedFitAllocator()
{
	memset(m_FixedSizeAllocators, 0, sizeof(LockedFixedSizeAllocator*) * sm_MaxFixedSizeAllocatorQuantity);
}

SpaceGameEngine::UInt32 SpaceGameEngine::SegregatedFitAllocator::RequestInformationToIndex(const RequestInformation& request_info)
{
	SGE_ASSERT(InvalidSizeError, request_info.m_First, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, request_info.m_Second);
	SGE_ASSERT(InvalidRequestInformationError, request_info);

	return (request_info.m_First << 8) | (request_info.m_Second);
}

bool SpaceGameEngine::SegregatedFitAllocator::InvalidRequestInformationError::Judge(const RequestInformation& request_info)
{
	return !(request_info.m_First <= 1024 && request_info.m_Second <= 128);
}