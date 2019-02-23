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
	
}

SpaceGameEngine::MemoryManager::SimpleAllocator::~SimpleAllocator()
{
	Clear();
}

void SpaceGameEngine::MemoryManager::SimpleAllocator::Set(SizeType data_mem_size, SizeType page_mem_size, SizeType alignment)
{
}

void SpaceGameEngine::MemoryManager::SimpleAllocator::Clear()
{
}

void * SpaceGameEngine::MemoryManager::SimpleAllocator::Allocate()
{
	return nullptr;
}

void SpaceGameEngine::MemoryManager::SimpleAllocator::Free(void * ptr)
{
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::SimpleAllocator::GetNextMemoryBlock(MemoryBlockHeader * ptr)
{
	return nullptr;
}
