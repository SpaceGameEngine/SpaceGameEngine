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

void * SpaceGameEngine::StdAllocator::RawNew(SizeType size)
{
	SGE_ASSERT(InvalidSizeError,size, 1, SGE_MAX_MEMORY_SIZE);
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

SpaceGameEngine::MemoryManager::SimpleAllocator::SimpleAllocator()
{
	m_IsInitialized = false;
}

SpaceGameEngine::MemoryManager::SimpleAllocator::~SimpleAllocator()
{
	if (m_IsInitialized)
	{

	}
}

void SpaceGameEngine::MemoryManager::SimpleAllocator::Init(SizeType data_mem_size, SizeType page_mem_size, SizeType alignment)
{
	m_IsInitialized = true;
}

void * SpaceGameEngine::MemoryManager::SimpleAllocator::Allocate()
{
	SGE_ASSERT(SimpleAllocatorNotInitializedError, m_IsInitialized);
	return nullptr;
}

void SpaceGameEngine::MemoryManager::SimpleAllocator::Free(void * ptr)
{
	SGE_ASSERT(SimpleAllocatorNotInitializedError, m_IsInitialized);
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::SimpleAllocator::GetNextMemoryBlock(MemoryBlockHeader * ptr)
{
	SGE_ASSERT(SimpleAllocatorNotInitializedError, m_IsInitialized);
	return nullptr;
}

bool SpaceGameEngine::MemoryManager::SimpleAllocator::SimpleAllocatorNotInitializedError::Judge(bool is_init)
{
	return !is_init;
}
