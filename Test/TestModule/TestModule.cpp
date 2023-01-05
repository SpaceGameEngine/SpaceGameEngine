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
#include "TestModule.h"
#include "Module/ModuleManager.h"

using namespace SpaceGameEngine;

int APlusB(int a, int b)
{
	return a + b;
}

SpaceGameEngine::MemoryManager* GetMemoryManagerAddress()
{
	return &SpaceGameEngine::MemoryManager::GetSingleton();
}

class TestModuleModule : public Module
{
public:
	static int sm_load_count;

	virtual void OnLoad() override
	{
		sm_load_count += 1;
	}

	virtual void OnUnload() override
	{
		sm_load_count -= 1;
	}
};

int TestModuleModule::sm_load_count = 0;

SGE_MODULE(TestModule);

int GetTestModuleLoadCount()
{
	return TestModuleModule::sm_load_count;
}