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
#include "GlobalVariable.h"
#include "Error.h"

SpaceGameEngine::GlobalVariableManager::~GlobalVariableManager()
{
	while (m_Content.GetSize() != 0)
	{
		Function<void(), StdAllocator> rel_func(std::move(m_Content.GetTop()));
		m_Content.Pop();
		rel_func();
	}
}

SpaceGameEngine::GlobalVariableManager& SpaceGameEngine::GlobalVariableManager::GetSingleton()
{
	static GlobalVariableManager g_GlobalVariableManager;
	return g_GlobalVariableManager;
}

SpaceGameEngine::GlobalVariableManager::GlobalVariableManager()
{
}

void SpaceGameEngine::GlobalVariableManager::Add(Function<void(), StdAllocator>&& rel_func)
{
	RecursiveLock lock(m_Mutex);
	lock.Lock();
	m_Content.Push(std::move(rel_func));
}
