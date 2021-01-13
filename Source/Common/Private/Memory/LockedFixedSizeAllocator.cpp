﻿/*
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
#include "Memory/LockedFixedSizeAllocator.h"
#include "Error.h"
#include <algorithm>

using namespace SpaceGameEngine;

/*!
@file
@todo think about other os's _mm_malloc/_mm_free location.
*/

MemoryBlockHeader* MemoryPageHeader::GetFirstMemoryBlock()
{
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<AddressType>(this) + sizeof(MemoryPageHeader) + m_Offset);
}

LockedFixedSizeAllocator::LockedFixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment)
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

LockedFixedSizeAllocator::~LockedFixedSizeAllocator()
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

void* LockedFixedSizeAllocator::Allocate()
{
	ScopedLock lock(mutex);

	if (!m_pFreeMemoryBlocks)
	{
		MemoryPageHeader* pNewPage = reinterpret_cast<MemoryPageHeader*>(new Byte[m_MemoryPageSize]);

		m_MemoryPageQuantity += 1;
		pNewPage->m_pNext = m_pMemoryPages;
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

void LockedFixedSizeAllocator::Free(void* ptr)
{
	ScopedLock lock(mutex);

	SGE_ASSERT(NullPointerError, ptr);
	MemoryBlockHeader* pBlock = reinterpret_cast<MemoryBlockHeader*>(ptr);
	pBlock->m_pNext = m_pFreeMemoryBlocks;
	m_pFreeMemoryBlocks = pBlock;
	m_FreeMemoryBlockQuantity += 1;
}

MemoryBlockHeader* LockedFixedSizeAllocator::GetNextMemoryBlock(MemoryBlockHeader* ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<Byte*>(ptr) + m_MemoryBlockSize);
}
