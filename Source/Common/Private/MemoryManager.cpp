/*
Copyright 2022 creatorlxd

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
#include "GlobalVariable.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#ifdef SGE_WINDOWS
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

void* SpaceGameEngine::StdAllocator::RawNew(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	return _mm_malloc(size, alignment == 0 ? GetDefaultAlignment(size) : alignment);
}

void SpaceGameEngine::StdAllocator::RawDelete(void* ptr)
{
	SGE_ASSERT(NullPointerError, ptr);

	_mm_free(ptr);
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader* SpaceGameEngine::MemoryManager::MemoryPageHeader::GetFirstMemoryBlock()
{
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<AddressType>(this) + sizeof(MemoryPageHeader) + m_Offset);
}

SpaceGameEngine::MemoryManager::FixedSizeAllocator::FixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment)
{
	SGE_ASSERT(InvalidValueError, alloc_mem_size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidValueError, page_mem_size, 1, SGE_MAX_MEMORY_SIZE);
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

void* SpaceGameEngine::MemoryManager::FixedSizeAllocator::Allocate()
{
	RecursiveLock lock(m_Mutex);
	lock.Lock();
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

void SpaceGameEngine::MemoryManager::FixedSizeAllocator::Free(void* ptr)
{
	RecursiveLock lock(m_Mutex);
	lock.Lock();
	SGE_ASSERT(NullPointerError, ptr);
	MemoryBlockHeader* pBlock = reinterpret_cast<MemoryBlockHeader*>(ptr);
	pBlock->m_pNext = m_pFreeMemoryBlocks;
	m_pFreeMemoryBlocks = pBlock;
	m_FreeMemoryBlockQuantity += 1;
}

SpaceGameEngine::MemoryManager::MemoryBlockHeader* SpaceGameEngine::MemoryManager::FixedSizeAllocator::GetNextMemoryBlock(MemoryBlockHeader* ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	return reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<Byte*>(ptr) + m_MemoryBlockSize);
}

bool SpaceGameEngine::InvalidAlignmentError::Judge(SizeType alignment)
{
	return !((alignment & (alignment - 1)) == 0);
}

SpaceGameEngine::SizeType SpaceGameEngine::GetDefaultAlignment(SizeType size)
{
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
	if (size >= 16)
		return 16;
	else
		return alignof(void*);
}

SpaceGameEngine::MemoryManager::~MemoryManager()
{
	for (SizeType i = 0; i < sm_MaxFixedSizeAllocatorQuantity; i++)
	{
		if (m_FixedSizeAllocators[i])
			delete m_FixedSizeAllocators[i];
	}
}

SpaceGameEngine::MemoryManager& SpaceGameEngine::MemoryManager::GetSingleton()
{
	static GlobalVariable<MemoryManager, StdAllocator> g_MemoryManager;
	return g_MemoryManager.Get();
}

void* SpaceGameEngine::MemoryManager::Allocate(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
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
			m_FixedSizeAllocators[index] = new FixedSizeAllocator(size, sm_MemoryPageSize, alignment);
		return m_FixedSizeAllocators[index]->Allocate();
	}
}

void SpaceGameEngine::MemoryManager::Free(void* ptr, SizeType size, SizeType alignment)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
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
	memset(m_FixedSizeAllocators, 0, sizeof(FixedSizeAllocator*) * sm_MaxFixedSizeAllocatorQuantity);
}

SpaceGameEngine::UInt32 SpaceGameEngine::MemoryManager::RequestInformationToIndex(const RequestInformation& request_info)
{
	SGE_ASSERT(InvalidValueError, request_info.m_First, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, request_info.m_Second);
	SGE_ASSERT(InvalidRequestInformationError, request_info);

	return (request_info.m_First << 8) | (request_info.m_Second);
}

bool SpaceGameEngine::MemoryManager::InvalidRequestInformationError::Judge(const RequestInformation& request_info)
{
	return !(request_info.m_First <= 1024 && request_info.m_Second <= 128);
}

void* SpaceGameEngine::MemoryManagerAllocator::RawNew(SizeType size, SizeType alignment)
{
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE - 2 * sizeof(SizeType));
	SGE_ASSERT(InvalidAlignmentError, alignment);

	SizeType real_size = size + std::max<SizeType>(2 * sizeof(SizeType), alignment);
	SizeType real_alignment = (alignment == 0 ? GetDefaultAlignment(size) : alignment);
	SizeType* p = (SizeType*)((AddressType)MemoryManager::GetSingleton().Allocate(real_size, real_alignment) + ((alignment > 2 * sizeof(SizeType)) ? alignment - 2 * sizeof(SizeType) : 0));
	p[0] = real_size;
	p[1] = real_alignment;
	return p + 2;
}

void SpaceGameEngine::MemoryManagerAllocator::RawDelete(void* ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	SizeType* p = ((SizeType*)ptr) - 2;
	SizeType size = p[0];
	SizeType alignment = p[1];
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
	SGE_ASSERT(InvalidAlignmentError, alignment);

	MemoryManager::GetSingleton().Free(((alignment > 2 * sizeof(SizeType)) ? (void*)((AddressType)ptr - alignment) : p), size, alignment);
}

/*
SpaceGameEngine::MemoryManager::MultiThreadBufferedFixedSizeAllocator::MultiThreadBufferedFixedSizeAllocator(SizeType alloc_mem_size, SizeType page_mem_size, SizeType alignment)
	: FixedSizeAllocator(alloc_mem_size, page_mem_size, alignment), m_pPreAllocationBuffer(new Atomic<void*>[sm_PreAllocationBufferQuantity]), m_pFreeBuffer(new Atomic<void*>[sm_FreeBufferQuantity])
{
	for (SizeType i = 0; i < sm_FreeBufferQuantity; ++i)
		m_pFreeBuffer[i].Store(nullptr, MemoryOrder::Release);
}

SpaceGameEngine::MemoryManager::MultiThreadBufferedFixedSizeAllocator::~MultiThreadBufferedFixedSizeAllocator()
{
	//Do not need to free the pre-allocated result in buffer.
	//The FixedSizeAllocator will release them by releasing the whole Memory Pages.

	delete[] m_pPreAllocationBuffer;
	delete[] m_pFreeBuffer;
}

void* SpaceGameEngine::MemoryManager::MultiThreadBufferedFixedSizeAllocator::Allocate()
{
	CallOnce(m_OnceFlag, [this]() {
		for (SizeType i = 0; i < sm_PreAllocationBufferQuantity; ++i)
			m_pPreAllocationBuffer[i].Store(FixedSizeAllocator::Allocate(), MemoryOrder::Release);
	});

	void* re = nullptr;
	SizeType idx = 0;
	while ((re = m_pPreAllocationBuffer[(idx = (idx + 1) % sm_PreAllocationBufferQuantity)].Exchange(nullptr, MemoryOrder::AcquireRelease)) == nullptr)
	{
		Thread::YieldCurrentThread();
	}
	Thread t(
		[this, idx]() {
			void* buf = nullptr;
			for (SizeType i = 0; i < sm_FreeBufferQuantity; ++i)
			{
				buf = m_pFreeBuffer[i].Exchange(nullptr, MemoryOrder::AcquireRelease);
				if (buf != nullptr)
				{
					m_pPreAllocationBuffer[idx].Store(buf, MemoryOrder::Release);
					return;
				}
			}
			m_pPreAllocationBuffer[idx].Store(FixedSizeAllocator::Allocate(), MemoryOrder::Release);
		});
	t.Detach();
	return re;
}

void SpaceGameEngine::MemoryManager::MultiThreadBufferedFixedSizeAllocator::Free(void* ptr)
{
	SGE_ASSERT(NullPointerError, ptr);
	void* buf = ptr;
	for (SizeType i = 0; i < sm_FreeBufferQuantity; ++i)
	{
		buf = m_pFreeBuffer[i].Exchange(buf, MemoryOrder::AcquireRelease);
		if (buf == nullptr)
			return;
	}
	FixedSizeAllocator::Free(buf);
}
*/
