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
#include "Module/ModuleManager.h"
#include "CommonModule.h"

bool SpaceGameEngine::EmptyModuleNameError::Judge(const String& name)
{
	return name.GetSize() == 0;
}

bool SpaceGameEngine::ConflictingModuleNameError::Judge(const Map<String, Module*>& m, const String& name)
{
	return m.Find(name) != m.GetConstEnd();
}

SpaceGameEngine::ModuleManager::ModuleManager()
{
	AddModule(SGE_STR("Common"), DefaultAllocator::New<CommonModule>());
}

void SpaceGameEngine::ModuleManager::AddModule(const String& name, Module* pmod)
{
	SGE_ASSERT(EmptyModuleNameError, name);
	SGE_ASSERT(ConflictingModuleNameError, m_ModuleMap, name);
	SGE_ASSERT(NullPointerError, pmod);

	m_ModuleMap.Insert(name, pmod);	   //avoid pmod adding itself again when call its OnLoad()
	pmod->OnLoad();
	m_Modules.Push(pmod);
}

SpaceGameEngine::ModuleManager::~ModuleManager()
{
	for (auto i = m_StaticModules.GetBegin(); i != m_StaticModules.GetEnd(); ++i)
	{
		DefaultAllocator::Delete(i->m_Second);
	}

	while (m_Modules.GetSize() != 0)
	{
		Module* pmod = m_Modules.GetTop();
		m_Modules.Pop();
		pmod->OnUnload();
		DefaultAllocator::Delete(pmod);
	}

	while (m_DllHandles.GetSize() != 0)
	{
		DllHandle dll = m_DllHandles.GetTop();
		m_DllHandles.Pop();
		UnloadDll(dll);
	}
}

void SpaceGameEngine::ModuleManager::LoadModule(const String& name)
{
	SGE_ASSERT(EmptyModuleNameError, name);

	if (m_ModuleMap.Find(name) != m_ModuleMap.GetEnd())	   //already loaded
		return;

	//load from static
	auto siter = m_StaticModules.Find(name);
	if (siter != m_StaticModules.GetEnd())
	{
		AddModule(name, siter->m_Second);
		m_StaticModules.Remove(siter);
		return;
	}
	//load from dynamic
#ifdef SGE_WINDOWS	  //unix platform do not use this solution
	DllHandle dll = LoadDll(GetDllPath(name));
	Module* (*pget_module)() = nullptr;
	pget_module = decltype(pget_module)(GetAddressFromDll(dll, SGE_STR("GetModule")));
	AddModule(name, pget_module());
	m_DllHandles.Push(dll);
#endif
	//do not throw error here, because if the module can not be founded, the LoadDll will throw the error.
}
