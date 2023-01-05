/*
Copyright 2023 creatorlxd

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
#include "MetaData/MetaData.h"
#include "MemoryManager.h"
#include <cstring>

SpaceGameEngine::MetaData::MetaData()
{
	m_pName = nullptr;
	m_Size = 0;
	m_Alignment = 0;
	m_pDefaultConstructor = nullptr;
	m_pCopyConstructor = nullptr;
	m_pMoveConstructor = nullptr;
	m_pCopyAssignment = nullptr;
	m_pMoveAssignment = nullptr;
	m_pDestructor = nullptr;
	m_pComparison = nullptr;
}

SpaceGameEngine::MetaData::MetaData(const char* name, SizeType size, SizeType alignment, void (*default_constructor)(void*), void (*copy_constructor)(void*, const void*), void (*move_constructor)(void*, void*), void* (*copy_assignment)(void*, const void*), void* (*move_assignment)(void*, void*), void (*destructor)(void*), bool (*comparison)(const void*, const void*))
	: m_pName(name), m_Size(size), m_Alignment(alignment), m_pDefaultConstructor(default_constructor), m_pCopyConstructor(copy_constructor), m_pMoveConstructor(move_constructor), m_pCopyAssignment(copy_assignment), m_pMoveAssignment(move_assignment), m_pDestructor(destructor), m_pComparison(comparison)
{
}

bool SpaceGameEngine::operator==(const MetaData& meta_data1, const MetaData& meta_data2)
{
	return (strcmp(meta_data1.m_pName, meta_data2.m_pName) == 0) && (meta_data1.m_Size == meta_data2.m_Size) && (meta_data1.m_Alignment == meta_data2.m_Alignment);
}
