/*
Copyright 2021 creatorlxd

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
#pragma once
#include "gtest/gtest.h"
#include "Module/Module.h"
#include "Module/ModuleManager.h"

using namespace SpaceGameEngine;

class Test1Module : public Module
{
public:
	inline static int sm_load_count = 0;
	inline static int sm_instance_count = 0;

	inline Test1Module()
	{
		sm_instance_count += 1;
	}

	inline virtual ~Test1Module()
	{
		sm_instance_count -= 1;
	}

	inline virtual void OnLoad() override
	{
		sm_load_count += 1;
	}

	inline virtual void OnUnload() override
	{
		sm_load_count -= 1;
	}
};

TEST(Module, InstanceTest)
{
	ASSERT_EQ(Test1Module::sm_instance_count, 0);
	Module* pmod = DefaultAllocator::New<Test1Module>();
	ASSERT_EQ(Test1Module::sm_instance_count, 1);

	ASSERT_EQ(Test1Module::sm_load_count, 0);
	pmod->OnLoad();
	ASSERT_EQ(Test1Module::sm_load_count, 1);
	pmod->OnUnload();
	ASSERT_EQ(Test1Module::sm_load_count, 0);

	DefaultAllocator::Delete(pmod);
	ASSERT_EQ(Test1Module::sm_instance_count, 0);
}

//same as SGE_MODULE when SGE_USE_DLL is not defined.
#define SGE_TEST_STATIC_MODULE(name) SGE_STATIC_MODULE(SGE_STR(#name), name##Module);

TEST(ModuleManager, StaticModuleTest)
{
	SGE_TEST_STATIC_MODULE(Test1);
	//SGE_STATIC_MODULE(SGE_STR("Test1"), Test1Module);

	ASSERT_EQ(Test1Module::sm_instance_count, 1);
	ASSERT_EQ(Test1Module::sm_load_count, 0);
	ModuleManager::GetSingleton().LoadModule(SGE_STR("Test1"));
	ASSERT_EQ(Test1Module::sm_instance_count, 1);
	ASSERT_EQ(Test1Module::sm_load_count, 1);
}