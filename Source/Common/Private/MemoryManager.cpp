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
#include <algorithm>
#ifdef SGE_WINDOWS
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

/*!
@file
@todo think about other os's _mm_malloc/_mm_free location.
*/

void * SpaceGameEngine::StdAllocator::RawNew(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError,size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);
	return _mm_malloc(size, alignment == 0 ? GetDefaultAlignment(size) : alignment);
}

void SpaceGameEngine::StdAllocator::RawDelete(void * ptr, SizeType size, SizeType alignment)
{
	SGE_ASSERT(NullPointerError,ptr);
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	_mm_free(ptr);
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::MemoryPageHeader::GetFirstMemoryBlock()
{
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<AddressType>(this) + sizeof(MemoryPageHeader) + m_Offset);
}

SpaceGameEngine::MemoryManager::FixedSizeAllocator::FixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment)
{
	SGE_ASSERT(InvalidSizeError, alloc_mem_size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidSizeError, page_mem_size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	m_pFreeMemoryBlocks = nullptr;
	m_FreeMemoryBlockQuantity = 0;
	
	m_pMemoryPages = nullptr;
	m_MemoryPageQuantity = 0;

	m_MemoryBlockSize = SGE_MEMORY_ALIGN(std::max((SizeType)sizeof(MemoryBlockHeader), alloc_mem_size), alignment);
	m_MemoryPageSize = page_mem_size;
	m_Alignment = alignment;
}

SpaceGameEngine::MemoryManager::FixedSizeAllocator::~FixedSizeAllocator()
{

	//only need to release pages' memory
	MemoryPageHeader* pPage = m_pMemoryPages;
	MemoryPageHeader* pb;
	while (pPage)
	{
		pb = pPage;
		pPage = pPage->m_pNext;
		delete[] reinterpret_cast<Byte*>(pb);
	}
}

void * SpaceGameEngine::MemoryManager::FixedSizeAllocator::Allocate()
{
	if (!m_pFreeMemoryBlocks)
	{
		MemoryPageHeader* pNewPage = reinterpret_cast<MemoryPageHeader*>(new Byte[m_MemoryPageSize]);
		
		m_MemoryPageQuantity += 1;
		if (m_pMemoryPages)
			pNewPage->m_pNext = m_pMemoryPages;
		else
			pNewPage->m_pNext = nullptr;
		m_pMemoryPages = pNewPage;

		AddressType StartAddress = (AddressType)(SGE_MEMORY_ALIGN((AddressType)(pNewPage) + sizeof(MemoryPageHeader), m_Alignment));
		pNewPage->m_Offset = StartAddress - (AddressType)(pNewPage) - sizeof(MemoryPageHeader);
		SizeType MemoryBlockQuantityPerPage = (m_MemoryPageSize - sizeof(MemoryPageHeader) - pNewPage->m_Offset) / m_MemoryBlockSize;
		m_FreeMemoryBlockQuantity += MemoryBlockQuantityPerPage;

		m_pFreeMemoryBlocks = pNewPage->GetFirstMemoryBlock();
		MemoryBlockHeader* pBlock = m_pFreeMemoryBlocks;
		for (SizeType i = 0; i < MemoryBlockQuantityPerPage - 1; i++)
		{
			pBlock->m_pNext = GetNextMemoryBlock(pBlock);
			pBlock = pBlock->m_pNext;
		}
		pBlock->m_pNext = nullptr;
	}
	MemoryBlockHeader* re = m_pFreeMemoryBlocks;
	m_pFreeMemoryBlocks = m_pFreeMemoryBlocks->m_pNext;
	m_FreeMemoryBlockQuantity -= 1;
	return re;
}

void SpaceGameEngine::MemoryManager::FixedSizeAllocator::Free(void * ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	MemoryBlockHeader* pBlock = reinterpret_cast<MemoryBlockHeader*>(ptr);
	pBlock->m_pNext = m_pFreeMemoryBlocks;
	m_pFreeMemoryBlocks = pBlock;
	m_FreeMemoryBlockQuantity += 1;
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader * SpaceGameEngine::MemoryManager::FixedSizeAllocator::GetNextMemoryBlock(MemoryBlockHeader * ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<Byte*>(ptr) + m_MemoryBlockSize);
}

bool SpaceGameEngine::InvalidAlignmentError::Judge(SizeType alignment)
{
	return !(alignment >= 0 && ((alignment & (alignment - 1))) == 0);
}

SpaceGameEngine::SizeType SpaceGameEngine::GetDefaultAlignment(SizeType size)
{
	SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
	if (size >= 16)
		return 16;
	else
		return 4;
}

SpaceGameEngine::MemoryManager::~MemoryManager()
{
	for (SizeType i = 0; i < sm_MaxFixedSizeAllocatorQuantity; i++)
	{
		if (m_FixedSizeAllocators[i])
			delete m_FixedSizeAllocators[i];
	}
}

void * SpaceGameEngine::MemoryManager::Allocate(SizeType size, SizeType alignment)
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
		if (m_FixedSizeAllocators[index])
		{
			return m_FixedSizeAllocators[index]->Allocate();
		}
		else
		{
			m_FixedSizeAllocators[index] = new FixedSizeAllocator(size, sm_MemoryPageSize, alignment);
			return m_FixedSizeAllocators[index]->Allocate();
		}
	}
}

void SpaceGameEngine::MemoryManager::Free(void * ptr, SizeType size, SizeType alignment)
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

SpaceGameEngine::MemoryManager::MemoryManager()
{
	memset(m_FixedSizeAllocators, 0, sizeof(FixedSizeAllocator*)*sm_MaxFixedSizeAllocatorQuantity);
}

SpaceGameEngine::UInt32 SpaceGameEngine::MemoryManager::RequestInformationToIndex(const RequestInformation & request_info)
{
	SGE_ASSERT(InvalidSizeError, request_info.m_First, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, request_info.m_Second);
	SGE_ASSERT(InvalidRequestInformationError, request_info);

	return (request_info.m_Second << 12) | (request_info.m_First);
}

SpaceGameEngine::MemoryManager::RequestInformation SpaceGameEngine::MemoryManager::IndexToRequestInformation(UInt32 index)
{
	return RequestInformation((index&0b111111111111),(index>>12));
}

bool SpaceGameEngine::MemoryManager::InvalidRequestInformationError::Judge(const RequestInformation & request_info)
{
	return !(request_info.m_First <= 2048 && request_info.m_Second <= 128);
}
