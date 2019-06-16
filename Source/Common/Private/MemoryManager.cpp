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
#include "MemoryManager.h"
#include "Error.h"

void * SpaceGameEngine::StdAllocator::RawNew(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError,size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);
	return new Byte[size];
}

void SpaceGameEngine::StdAllocator::RawDelete(void * ptr)
{
	SGE_ASSERT(NullPointerError,ptr);
	delete[] ptr;
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::MemoryPageHeader::GetFirstMemoryBlock()
{
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<AddressType>(this) + sizeof(MemoryPageHeader));
}

SpaceGameEngine::MemoryManager::FixedSizeAllocator::FixedSizeAllocator()
{
	m_IsInitialized = false;
}

SpaceGameEngine::MemoryManager::FixedSizeAllocator::~FixedSizeAllocator()
{
	if (m_IsInitialized)
	{

	}
}

void SpaceGameEngine::MemoryManager::FixedSizeAllocator::Init(SizeType data_mem_size, SizeType page_mem_size, SizeType alignment)
{
	m_IsInitialized = true;
}

void * SpaceGameEngine::MemoryManager::FixedSizeAllocator::Allocate()
{
	SGE_ASSERT(FixedSizeAllocatorNotInitializedError, m_IsInitialized);
	return nullptr;
}

void SpaceGameEngine::MemoryManager::FixedSizeAllocator::Free(void * ptr)
{
	SGE_ASSERT(FixedSizeAllocatorNotInitializedError, m_IsInitialized);
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::FixedSizeAllocator::GetNextMemoryBlock(MemoryBlockHeader * ptr)
{
	SGE_ASSERT(FixedSizeAllocatorNotInitializedError, m_IsInitialized);
	return nullptr;
}

bool SpaceGameEngine::MemoryManager::FixedSizeAllocator::FixedSizeAllocatorNotInitializedError::Judge(bool is_init)
{
	return !is_init;
}

bool SpaceGameEngine::InvalidAlignmentError::Judge(SizeType alignment)
{
	short cot = 0;
	while (alignment != 0)
	{
		if ((alignment & (SizeType)1) == 1)
			cot += 1;
		alignment >>= 1;
	}
	if (cot > 1)	//0 or 2^n can pass the judgment
		return true;
	else
		return false;
}

SpaceGameEngine::SizeType SpaceGameEngine::GetDefaultAlignment(SizeType size)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	if (size >= 16)
		return 16;
	else
		return 4;
}
